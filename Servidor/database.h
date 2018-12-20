#ifndef DATABASE_H
#define DATABASE_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
#include <chrono>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <boost/circular_buffer.hpp>
using namespace std;

class Database {

private:
	// to define the capacity of the buffers
	int capacity;

	struct Info {
		float data;
		std::chrono::system_clock::time_point timestamp; 
	};

	struct Buffer {
		
		boost::circular_buffer<struct Info> ilum; 				// for illuminance values
		boost::circular_buffer<struct Info> dutyCycle; 		// for duty cycle values

		int lastRead = 0;
	};


	Buffer buffs[127]; 													        // num of nodes for the T gets

	int numBuffers = 0;																	// num of nodes for the T gets
	std::chrono::system_clock::time_point last_restart;	// time of the last restart 

public:

	Database(int capacity);

	~Database();

	void insertBuffer(int address, float lux, float dc);

	void clearBuffers();

	std::string getCurrentValues(char variable, int desk);
	std::string getLastMinuteValues(char message[]);

	// for the T gets
	int getNumBuffers();

	int getLastReadState(int address);

	void setLastReadState(int address);

	std::string getStreamValues(int address, char type);

	// temp variables to save the info
	int occupancy = 0;
	float ilumLowB = 0;
	float extilum = 0;
	float refilum = 0;
	float instPow = 0;
	float instPowT = 0;
	float accumEn = 0;
	float accumEnT = 0;
	float accumConf = 0;
	float accumConfT = 0;
	float accumVar = 0;
	float accumVarT = 0;
	float timeSeconds = 0;

	// last sender
	int last_sender = -1;
};

#endif