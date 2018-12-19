#include "Comm_I2C.h"

volatile bool Comm_I2C::sync;
int Comm_I2C::iter;

Comm_I2C::Comm_I2C(int _addr){
	addr = _addr;
}


void Comm_I2C::msgAnalyse(int id, String data_str){

  	switch(id){
      case 1:
        //Serial.println("Consensus Flag -> T");
        consensus_flag = true;
        consensus_data = data_str;
        break;

      case 2:
        consensus_data = data_str;

      default:
        break;
    }

}

int Comm_I2C::msgSend(int id, int dest_addr, String data_str){

	Wire.beginTransmission(dest_addr);
	Wire.write(id);
	Wire.write(addr);
	Wire.write(data_str.c_str());
 
	return Wire.endTransmission(); // Returns 0 if the msg was sent successfully
	
}

int Comm_I2C::msgBroadcast(int id, String data_str){

	return msgSend(id,BROADCAST_ADDR,data_str); // Returns 0 if the msg was sent successfully	
}


int Comm_I2C::getAddr() const{

  return addr;
}

void Comm_I2C::msgSyncCallback(int num){
  /*if(Wire.available() > 0){
      //char c = Wire.read();      
      if(Wire.read() == 'a'){
        //Serial.write(c);
        sync = true;
      }        
  }*/
}

void Comm_I2C::msgSync(){

  sync = false;
  
  while(!sync){
    //Serial.println(sync);
    //Wire.onReceive(msgSyncCallback);
    delayMicroseconds(500);
    //Serial.println(sync);
    Wire.beginTransmission(BROADCAST_ADDR);
    Wire.write('a');
    Wire.endTransmission();
  }

  sync = false;

  //Serial.println("Sync!");
  delayMicroseconds(500);
}

String Comm_I2C::floatToString(float num){

	char str[8];
	dtostrf(num,7,2,str);
	String str2(str);

	return str2;
}
