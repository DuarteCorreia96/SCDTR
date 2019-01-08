#include "Comm_I2C.h"

volatile bool Comm_I2C::sync;
int Comm_I2C::ndev = 1;
int Comm_I2C::consensus_cnt = 0;

Comm_I2C::Comm_I2C(int _addr) {
  addr = _addr;
}

/*void Comm_I2C::getOtherU(float _u2) {
  u2 = _u2;
}*/

void Comm_I2C::msgAnalyse(char id, int src, String data_str) {

  //Serial.println(id);

  switch (id) {
    case 'c':
      //Serial.println(consensus_cnt);
      consensus_data[consensus_cnt] = data_str;
      //Serial.println(consensus_data[consensus_cnt]);
      ++consensus_cnt;      
      if(consensus_cnt == ndev-1){ // Copies of d vector from all other nodes received
        all_copies = true;
        consensus_cnt = 0;
      }
      break;

    case 'n':
      calib_flag = true;
      break;

    case 'h':
      hello_flag = true;
      break;

    case 'a':
      /*d_ext[src-1] = atof(data_str.c_str());
      ++consensus_cnt;
      if(consensus_cnt == ndev-1) {
        calc_ext_ill = true;
        consensus_cnt = 0;
      }*/
      break;

    default:
      break;
  }

}

int Comm_I2C::msgSend(char id, int dest_addr, String data_str) {

  Wire.beginTransmission(dest_addr);
  Wire.write(id);
  Wire.write(addr);
  Wire.write(data_str.c_str());
  return Wire.endTransmission(); // Returns 0 if the msg was sent successfully (source of problems, sometimes the program hangs here!)

}

void Comm_I2C::msgBroadcast(char id, String data_str) {

  /*Wire.beginTransmission(RASP_ADDR);
  Wire.write(id);
  Wire.write(' ');
  Wire.write(addr + 48);
  Wire.write(data_str.c_str());*/

  msgSend(id, BROADCAST_ADDR, data_str);

  //return Wire.endTransmission(); // Returns 0 if the msg was sent successfully
}

void Comm_I2C::sayHi(){

  msgBroadcast('h',"");
      
  unsigned long init = micros();
  while(micros() - init < 5000000){
    if(hello_flag){
      hello_flag = false;
      init = micros();
    }  
  }
  
}



void Comm_I2C::findNodes(){

  int _ndev = 0;
  int error;
  
  /*unsigned long init = micros();
  while(micros() - init < 5000000){
    if(hello_flag){
      hello_flag = false;
      _ndev++;
      //init = micros();
    }  
  }*/

  for(int address = 1; address < MAX_LUM; address++){

    if(address == addr)  continue;

    Wire.beginTransmission(address);
    Wire.write('x');
    error = Wire.endTransmission();

    // Addresses are assigned sequentially
    if(error != 0)  break;
    
    Serial.println(address);
    _ndev++;
        
  }

  ndev = _ndev + 1;
  Serial.println("Search complete");
}

int Comm_I2C::getAddr() const {

  return addr;
}

/*void Comm_I2C::msgSyncCallback(int num) {
  if(Wire.available() > 0){
      //char c = Wire.read();
      if(Wire.read() == 'a'){
        //Serial.write(c);
        sync = true;
      }
    }
}*/

void Comm_I2C::msgSync(int addr) {

  sync = false;

  while (!sync) {
    //Serial.println(sync);
    //Wire.onReceive(msgSyncCallback);
    delayMicroseconds(2000);
    //Serial.println(sync);
    Wire.beginTransmission(addr);
    Wire.write('a');
    Wire.endTransmission();
  }

  sync = false;

  //Serial.println("Sync!");
  delayMicroseconds(2000);
}

String Comm_I2C::floatToString(float num) {

  char str[8];
  dtostrf(num, 7, 2, str);
  String str2(str);

  return str2;

}
