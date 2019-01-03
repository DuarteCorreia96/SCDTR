#include <boost/circular_buffer.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <ctime>
#include <thread>
#include <mutex>
#include "data_storage.h"

#include <pigpio.h>
#include <memory.h>

#include "server.h"
using namespace std;


#include <random>

using boost::asio::ip::tcp;

std::mutex mtx;
auto p = std::make_shared<data_storage>();
volatile bool flag = false;
volatile bool flag_print = false;

void thread1(){

  bsc_xfer_t xfer;
  gpioInitialise();

  gpioSetMode(18, PI_ALT3);
  gpioSetMode(19, PI_ALT3);

  xfer.control = (0x04 << 16) | 0x305; // Set I2C slave Address to 0x00

  while(true){

      int status = bscXfer(&xfer);
      //pause();
      if (xfer.rxCnt > 0){

          char var; 
          int node;
          float val;

          if(sscanf(xfer.rxBuf,"%c %d %f", &var, &node, &val) != 3){
            memset(xfer.rxBuf, '\0', sizeof(char) * BSC_FIFO_SIZE);
            continue;
          }
          mtx.lock();
          switch(var){
            case 'l':
              p->insert_illu(val, node);
              break;
            case 'p':
              p->insert_duty(val, node);
              break;
            case 'e':
              p->ext_illum[node] = val;
              break; 
            case 'o':
              p->occupancy[node] = (bool) val;
              break;  
            case 'b':
              p->lbound[node] = val;
              break;           
            case 'r':
              p->control_ref[node] = val;
              break;                            
            default:
              p->k = "Wrong data received";
              flag = true; 
              break; 
          }
          mtx.unlock();

          // processes data
          memset(xfer.rxBuf, '\0', sizeof(char) * BSC_FIFO_SIZE);
      }
  }
}

void thread_teste(){

  int node = 1;
  float r;
  srand(time(NULL));

  while(true){

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    r = (((double)rand() / (RAND_MAX)) - 0.5) * 2;

    mtx.lock();    

    p->insert_illu(120 + r, node);
    p->insert_duty(100 + r, node);
    p->ext_illum[node] = 10 + r;
    p->occupancy[node] = true;
    p->lbound[node] = 50;

    r = (((double)rand() / (RAND_MAX)) - 0.5) * 2;
    p->control_ref[node] = 120 + r;

    mtx.unlock();

    if(node == 2){
      node = 1;
    } else {
      node = 2;
    }
  }
}

void thread2(){

  try{
    boost::asio::io_service io_service;
    tcp_server server(io_service, p);
    io_service.run();
  }
  catch (std::exception &e){

    std::cerr << e.what() << std::endl;
  }
}

void thread3(){
  
  int node = 2;
  while (true){
    
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    if(flag_print){

      std::cout << "\nNode: "  << node << std::endl;
      std::cout << "Illuminance = " << p->buff[node].illum[0].data << std::endl;
      std::cout << "Duty Cycle  = " << p->buff[node].duty_cycle[0].data << std::endl;
      std::cout << "Lower Bound = " << p->lbound[node] << std::endl;
      std::cout << "External Ilu= " << p->ext_illum[node] << std::endl;
      std::cout << "Control Ref = " << p->control_ref[node] << std::endl;
      std::cout << "Inst Power  = " << p->inst_power[node] << std::endl;
      std::cout << "Energy Coms = " << p->energy_coms[node] << std::endl;
      std::cout << "Confort Err = " << p->comfort_error[node]/p->buff[node].illum.size() << std::endl;
      std::cout << "Confort Flic= " << p->comfort_flicker[node]/p->buff[node].illum.size() << std::endl;

      auto now = std::chrono::system_clock::now();
      float seconds = std::chrono::duration_cast<std::chrono::seconds>(now - p->last_restart).count();
      std::cout << "Last Restart= "  << seconds << std::endl;

      if(node == 2){
        node = 1;
      } else {
        node = 2;
      }

      if(flag == true){

        std::stringstream msg;
        msg << "thread2: " << p->k << std::endl;
        std::cout << msg.str();  
        flag = false;    
      }
    }
  }
}

void command(){

  char c;
  while (true){

    c = getchar();
    switch (c){
    case 'p':
      std::cout << "\nSwitching Printing Values Received" << std::endl;
      flag_print = !flag_print;
      break;

    case 'q':
      std::cout << "\nClosing Server" << std::endl;
      exit(0);
      break;

    default:
      std::cout << "\nWrong command" << std::endl;
      break;
    }
    getchar();
  }
}

int main(){

  //std::thread threadwrite(thread1);
  std::thread threadread(thread2);
  std::thread print_thread(thread3);
  std::thread command_thread(command);
  std::thread generate(thread_teste);

  //threadwrite.join();
  threadread.join();
  print_thread.join();
  command_thread.join();
  generate.join();
}