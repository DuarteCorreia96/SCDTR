#ifndef _COMM_I2C
#define _COMM_I2C

#include "node.h"
#include <string> 
#define BROADCAST_ADDR 0

using namespace std;

class Comm_I2C : public Node {

	protected:
		
		int addr;
    static int iter;
		string consensus_data;

	public:

		Comm_I2C() {};
		Comm_I2C(int addr);

		int getAddr() const;
		const char* getConsensusData();
};

#endif
