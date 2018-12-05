#ifndef _COMM_I2C
#define _COMM_I2C

#include <Wire.h>
#include <Arduino.h>
#include "node.h"
#define BROADCAST_ADDR 0

class Comm_I2C : public Node {

  	private:
  
		static void msgSyncCallback(int);
		static bool sync;

	protected:
		
		int addr;
    	static int iter;
		String consensus_data;
		String floatToString(float num);

	public:

		Comm_I2C() {};
		Comm_I2C(int addr);
		//~Comm_I2C();
    	bool calib();
		int getAddr() const;
		const char* getConsensusData();
		void msgAnalyse(int id, String data_str);
		int msgBroadcast(int id, String data_str);
		int msgSend(int id, int dest_addr, String data_str);
		void msgSync();

		// Flags
		bool calib_flag;
    bool consensus_flag;
};

#endif
