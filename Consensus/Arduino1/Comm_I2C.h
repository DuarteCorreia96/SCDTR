#ifndef _COMM_I2C
#define _COMM_I2C

#include <Wire.h>
#include <Arduino.h>
//using namespace std;

class Comm_I2C{

	protected:
			
		int addr;
		String consensus_data;
		String floatToString(float num);
	
	public:

		Comm_I2C() {};
		Comm_I2C(int addr);
		//~Comm_I2C();
		int getAddr() const;
    const char* getConsensusData();
		void msgAnalyse(int id, String data_str);
		int msgBroadcast(int id, String data_str);
		int msgSend(int id, int dest_addr, String data_str);

};

#endif
