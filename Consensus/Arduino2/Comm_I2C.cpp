#include "Comm_I2C.h"

bool Comm_I2C::sync;

Comm_I2C::Comm_I2C(int _addr){
	addr = _addr;
  sync = false;
}

void Comm_I2C::msgAnalyse(int id, String data_str){

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
  
  while(!sync){
    Wire.onReceive(msgSyncCallback);
    delay(1000);
    Wire.beginTransmission(BROADCAST_ADDR);
    Wire.write('a');
    Wire.endTransmission();
  }

  Serial.println("Ready!");
  delay(1000);
}

String Comm_I2C::floatToString(float num){

	char* str;
	dtostrf(num,5,2,str);
	String str2(str);

	return str2;

}



