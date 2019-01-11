#ifndef _COMM_I2C
#define _COMM_I2C

#include <Wire.h>
#include <Arduino.h>

#define BROADCAST_ADDR 0
#define RASP_ADDR 127
#define MAX_LUM 4

class Comm_I2C {

  private:
    
    int consensus_cnt;

  protected:
    uint8_t addr;
    int ndev;
    String consensus_data[MAX_LUM-1];

    String floatToString(float num);

    volatile bool hello_flag;
    volatile bool restartConsensus;
    volatile bool calib_flag;
    volatile bool all_copies;

  public:

    Comm_I2C() {};
    Comm_I2C(int addr);
    //~Comm_I2C();
    int getAddr() const;
    void msgAnalyse(char id, int src, String data_str);
    void msgBroadcast(char id, String data_str);
    int msgSend(char id, int dest_addr, String data_str);
    void msgSync(int addr);
    void findNodes();
    void sayHi();
    
    // Flags
    volatile bool sync;
};

#endif
