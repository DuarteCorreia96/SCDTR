#ifndef _COMM_I2C
#define _COMM_I2C

#include <Wire.h>
#include <Arduino.h>
#include "Vector.h"
#define BROADCAST_ADDR 0
//#define RASP_ADDR 4
//#define OTHER_ADDR 2
#define LOWB 50
#define UPPB 150

#define MAX_LUM 5 // Choose max number of luminaries accordingly!!

class Comm_I2C {

  private:

    //static void msgSyncCallback(int);

  protected:
    float u2 = 0;
    uint8_t addr;
    static int ndev;
    String floatToString(float num);
    //Vector<String> consensus_data;
    String consensus_data[MAX_LUM-1];
    bool calib_flag;
    bool all_copies;

  public:

    bool consensus_flag;
    Comm_I2C() {};
    Comm_I2C(int addr);
    //~Comm_I2C();
    int getAddr() const;
    void msgAnalyse(char id, String data_str);
    void msgBroadcast(char id, String data_str);
    int msgSend(char id, int dest_addr, String data_str);
    void msgSync(int addr);
    void findNodes();
    void sayHi();
    //void getOtherU(float _u2);
    

    // Flags
    volatile static bool sync;
    bool hello_flag = false;
};

#endif
