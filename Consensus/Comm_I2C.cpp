#include "Comm_I2C.h"

Comm_I2C::Comm_I2C(int _addr)
{
	addr = _addr;
}

void Comm_I2C::msgAnalyse(int id, String data_str)
{
	if(id == 1) //Consensus Identifier
	{
    Serial.println("Changed consensus flag to true");
		consensus_flag = true;
		consensus_data = data_str;
	}

}

int Comm_I2C::msgSend(int id, int src_addr, int dest_addr, char* data_str)
{
	Wire.beginTransmission(dest_addr);
	Wire.write(id);
	Wire.write(src_addr);
	Wire.write(data_str);
	
	return Wire.endTransmission(); // Returns 0 if the msg was sent successfully
	
}

const char* Comm_I2C::getConsensusData()
{
  return consensus_data.c_str();
}

int Comm_I2C::getAddr() const
{
  return addr;
}

