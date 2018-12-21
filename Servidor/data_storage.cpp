#include "data_storage.h"

void data_storage::init_variables(){

  last_restart = std::chrono::system_clock::now();
  for (int i = 0; i < NODES; i++){

    energy_coms[i] = 0;
    comfort_error[i] = 0;
    comfort_flicker[i] = 0;
    buff[i].illum.clear();
    buff[i].duty_cycle.clear();
    buff[i].illum.set_capacity(1028);
    buff[i].duty_cycle.set_capacity(1028);
  }
}

void data_storage::insert_duty(int pwm, int node){

  Info duty_cycle;
  duty_cycle.data = pwm/255;
  duty_cycle.timestamp = std::chrono::system_clock::now();

  buff[node].duty_cycle.push_front(duty_cycle);
  update_after_duty(node);
}

void data_storage::insert_illu(int illu, int node){

  Info illuminance;
  illuminance.data = illu;
  illuminance.timestamp = std::chrono::system_clock::now();

  buff[node].duty_cycle.push_front(illuminance);
  update_after_duty(node);
}

void data_storage::update_after_duty(int node){

  inst_power[node] = buff[node].duty_cycle[0].data;
  std::chrono::system_clock::time_point now =
      std::chrono::system_clock::now();

  float time_st_dc = std::chrono::duration_cast<std::chrono::seconds>(now - buff[node].duty_cycle[0].timestamp).count();

  energy_coms[node] += inst_power[node] * time_st_dc;
}

void  data_storage::update_after_illum(int node){

  if (float temp = control_ref[node] - buff[node].illum[0].data > 0){
    comfort_error[node] += temp;
  }

  if (buff[node].illum.size() > 3){

    float time_st_dc = std::chrono::duration_cast<std::chrono::seconds>(buff[node].illum[0].timestamp - buff[node].illum[1].timestamp).count();
    if ((buff[node].illum[0].data - buff[node].illum[1].data) * 
        (buff[node].illum[1].data - buff[node].illum[2].data) < 0){

      comfort_flicker[node] += abs(buff[node].illum[0].data - buff[node].illum[1].data) +
                               abs(buff[node].illum[1].data - buff[node].illum[2].data) /
                                   (2 * time_st_dc);
    } else {
      comfort_flicker[node] += 0;
    }
  }
}