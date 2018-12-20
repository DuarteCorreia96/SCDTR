
//Compile as:  // g++ -g -O -Wall -pedantic -std=c++11 tcpServer.cpp
// serialComm.cpp raspServer.cpp -lpthread -lboost_system -o server
//Run in a separate terminal, before starting client : ./server 17000

#include "tcpServer.h"


using namespace std;
using namespace boost::asio;

// creats the object where we handle the data coming from arduino
// shared_ptr <Database> db (new Database(6000));

// creates I2C object
//shared_ptr <I2Comm> i2c_slave (new I2Comm(db));

// creating io services for tcp and arduino
io_service io;

//shared_ptr <SerialComm> arduino (new SerialComm(io, "/dev/ttyACM0", db, i2c_slave));


/* void i2c_thread() {

  // creates object
  i2c_slave->sniffer();

  return;
} */

int main(int argc, char* argv[])
{
  try{

    boost::asio::io_service io_service;
    tcp_server server(io_service);
    io_service.run();
  }
  catch (std::exception &e){
    
    std::cerr << e.what() << std::endl;
  }
}