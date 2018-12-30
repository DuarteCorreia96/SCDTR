#ifndef __DATA_STORAGE__
#define __DATA_STORAGE__

#include <boost/circular_buffer.hpp>
#include <iostream>
#include <chrono>
#include <ctime>
#include <mutex>

#define NODES 3
class data_storage{

private:
  struct Info{

    float data;
    std::chrono::system_clock::time_point timestamp;
  };

  struct Buffer{

    boost::circular_buffer<struct Info> illum;      // for illuminance values
    boost::circular_buffer<struct Info> duty_cycle; // for duty cycle values
  };

public:
  data_storage(){ init_variables(); };

  void init_variables();
  void update_after_duty(int node);
  void update_after_illum(int node);
  void insert_duty(float pwm, int node);
  void insert_illu(float illu, int node);

  Buffer buff[NODES];
  bool occupancy[NODES];
  float lbound[NODES];
  float ext_illum[NODES];
  float control_ref[NODES];

  float inst_power[NODES];

  std::chrono::system_clock::time_point last_restart;
  float energy_coms[NODES];
  float comfort_error[NODES];
  float comfort_flicker[NODES];
  std::string k;
};

#endif