#ifndef _COMM_I2C
#define _COMM_I2C

#include <Wire.h>
#include <Arduino.h>
#define BROADCAST_ADDR 0

class Comm_I2C {

  private:

    static void msgSyncCallback(int);

  protected:

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
    void msgAnalyse(int id, String data_str);
    int msgBroadcast(int id, String data_str);
    int msgSend(int id, int dest_addr, String data_str);
    void msgSync();

    // Flags
    volatile static bool sync;
};

#endif
