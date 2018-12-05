#include "Comm_I2C.h"

bool Comm_I2C::sync;
int Comm_I2C::iter;

Comm_I2C::Comm_I2C(int _addr){
	addr = _addr;
}

bool Comm_I2C::calib(){

  if (iter > 2) return true;

  if(calib_flag){

    msgSync();

    setPWM(0);
    o = readIlluminance();

    setPWM(255);
    float x_max = readIlluminance();
    k[0] = (x_max - o)/100;

    setPWM(0);
    msgSync();
    delay(500);

    float x_max2 = readIlluminance();
    k[1] = (x_max2 - o)/100;

    calib_flag = !calib_flag;
    msgSync();
    ++iter;
    calib();
  }
  else{

    msgSync();

    setPWM(0);
    msgSync();
    setPWM(255);

    calib_flag = !calib_flag;
    msgSync();
    ++iter;
    calib();
  }  
}

void Comm_I2C::msgAnalyse(int id, String data_str){

  	if(id == 1){
			Serial.println("Consensus Flag -> T");
			consensus_flag = true;
			consensus_data = data_str;
    }

}

int Comm_I2C::msgSend(int id, int dest_addr, String data_str){

	Wire.beginTransmission(dest_addr);
	Wire.write(id);
	Wire.write(" ");
	Wire.write(addr);
	Wire.write(" ");
	Wire.write(data_str.c_str());
	
	return Wire.endTransmission(); // Returns 0 if the msg was sent successfully
	
}

int Comm_I2C::msgBroadcast(int id, String data_str){

	return msgSend(id,BROADCAST_ADDR,data_str);; // Returns 0 if the msg was sent successfully	
}


const char* Comm_I2C::getConsensusData(){

  return consensus_data.c_str();
}

int Comm_I2C::getAddr() const{

  return addr;
}

void Comm_I2C::msgSyncCallback(int num){
  if(Wire.available() > 0){
      if(Wire.read() == 'a') 
        sync = true;
  }
}

void Comm_I2C::msgSync(){

  sync = false;
  
  while(!sync){
    Wire.onReceive(msgSyncCallback);
    delay(200);
    Wire.beginTransmission(BROADCAST_ADDR);
    Wire.write('a');
    Wire.endTransmission();
  }

  sync = false;

  Serial.println("Sync!");
  delay(200);
}

String Comm_I2C::floatToString(float num){

	char* str;
	dtostrf(num,5,2,str);
	String str2(str);

	return str2;

}



