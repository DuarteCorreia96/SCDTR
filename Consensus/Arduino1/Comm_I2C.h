#ifndef _COMM_I2C
#define _COMM_I2C

#include <Wire.h>
#include <Arduino.h>
#define BROADCAST_ADDR 0
#define RASP_ADDR 4
#define OTHER_ADDR 2
#define LOWB 50
#define UPPB 150

class Comm_I2C {

  private:

    static void msgSyncCallback(int);

  protected:
    float u2 = 0;
    int addr;
    static int iter;
    String floatToString(float num);
    String consensus_data;

  public:

    bool consensus_flag;
    Comm_I2C() {};
    Comm_I2C(int addr);
    //~Comm_I2C();
    int getAddr() const;
    void msgAnalyse(char id, String data_str);
    int msgBroadcast(char id, String data_str);
    int msgSend(char id, int dest_addr, String data_str);
    void msgSync();
    void getOtherU(float _u2);

    // Flags
    volatile static bool sync;
};

#endif
