#ifndef _COMM_I2C
#define _COMM_I2C

#include <Wire.h>
#include <Arduino.h>
//using namespace std;

class Comm_I2C
{
	private:
		int addr;
		String consensus_data;
	
	public:
		// FLAGS
		bool consensus_flag;
		
		Comm_I2C();
		Comm_I2C(int addr);
		//~Comm_I2C();
		void msgAnalyse(int id, String data_str);
		int msgSend(int id, int src_addr, int dest_addr, char* data_str);
    const char* Comm_I2C::getConsensusData();
    int getAddr() const;
		//int msgBroadcast(int id, int src_addr, String data_str);
};

#endif
