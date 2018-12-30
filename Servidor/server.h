//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef __SERVER_RASP__
#define __SERVER_RASP__

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "data_storage.h"

using boost::asio::ip::tcp;

class tcp_connection: public boost::enable_shared_from_this <tcp_connection> {
public:

  typedef boost::shared_ptr<tcp_connection> pointer;
  static pointer create(boost::asio::io_service &io_service, std::shared_ptr<data_storage> db_){
    return pointer(new tcp_connection(io_service, db_));
  }

  tcp::socket &socket(){
    return socket_;
  }

  void start(){

    socket_.async_read_some(boost::asio::buffer(request_, max_length),
                            boost::bind(&tcp_connection::handle_read, shared_from_this(),
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
  }

private:
  tcp_connection(boost::asio::io_service &io_service, std::shared_ptr<data_storage> db_): 
    socket_(io_service),
    db(db_){}

  void handle_read(const boost::system::error_code &error, size_t bytes_transferred){

    if (!error){

      char command;
      int node;
      char var;
      int k;
      if(sscanf(request_,"%c %c %d", &command, &var, &node) != 3){
            message_ = "Wrong message\n";

      } else if(command == 'g' && var == 'l'){
          message_ = "l " + std::to_string(node) + " " + std::to_string(db->buff[node].illum[0].data) + "\n";

      } else if(command == 'g' && var == 'd'){
          message_ = "d " + std::to_string(node) + " " + std::to_string(db->buff[node].duty_cycle[0].data) + "\n";

      } else if(command == 'g' && var == 's'){
          message_ = "s " + std::to_string(node) + " " + std::to_string(db->occupancy[node]) + "\n";

      } else if(command == 'g' && var == 'L'){
          message_ = "L " + std::to_string(node) + " " + std::to_string(db->lbound[node]) + "\n";

      } else if(command == 'g' && var == 'o'){
          message_ = "o " + std::to_string(node) + " " + std::to_string(db->ext_illum[node]) + "\n";

      } else if(command == 'g' && var == 'r'){
          message_ = "o " + std::to_string(node) + " " + std::to_string(db->control_ref[node]) + "\n"; 

      } else if(command == 'g' && var == 'p'){
          message_ = "p " + std::to_string(node) + " " + std::to_string(db->inst_power[node]) + "\n";
      
      } else if(command == 'g' && var == 'p' && char(node) == 'T'){
          message_ = "p T " + std::to_string(db->inst_power[1]+ db->inst_power[2]) + "\n";

      } else if(command == 'g' && var == 't'){
        auto now = std::chrono::system_clock::now();
        float seconds = std::chrono::duration_cast<std::chrono::seconds>(now - db->last_restart).count();
        message_ = "t " + std::to_string(node) + " " + std::to_string(seconds) + "\n";
      } else if(command == 'g' && var == 'e'){
          message_ = "e " + std::to_string(node) + " " + std::to_string(db->energy_coms[node]) + "\n";

      } else if(command == 'g' && var == 'e' && char(node) == 'T'){
          message_ = "e T " + std::to_string(db->energy_coms[1]+ db->energy_coms[2]) + "\n";

      } else if(command == 'g' && var == 'c'){
          message_ = "c " + std::to_string(node) + " " + std::to_string(db->comfort_error[node]/db->buff[node].illum.size()) + "\n";

      } else if(command == 'g' && var == 'c' && char(node) == 'T'){
          message_ = "c T " +  std::to_string(db->comfort_error[1]/db->buff[1].illum.size() +  
                                db->comfort_error[2]/db->buff[2].illum.size())  + "\n"; 
      } else if(command == 'g' && var == 'v'){
          message_ = "v " + std::to_string(node) + " " + std::to_string(db->comfort_flicker[node]/db->buff[node].illum.size()) + "\n";

      } else if(command == 'g' && var == 'c' && char(node) == 'T'){
          message_ = "v T " +  std::to_string(db->comfort_flicker[1]/db->buff[1].illum.size() +  
                                db->comfort_flicker[2]/db->buff[2].illum.size()) + "\n"; 
      } else if(command == 'b' && var == 'l') {
        message_ = "b l " + std::to_string(node) + "\n";
        auto now = std::chrono::system_clock::now();
        k = 1;
        while(std::chrono::duration_cast<std::chrono::seconds>(now - db->buff[node].illum[k].timestamp).count() < 60){
          message_ += std::to_string(db->buff[node].illum[k].data) + ",\n";
          k++;
        }
        message_ = "error";
      } else if(command == 'b' && var == 'd') {
        message_ = "d l " + std::to_string(node) + "\n";
        auto now = std::chrono::system_clock::now();
        k = 1;
        while(std::chrono::duration_cast<std::chrono::seconds>(now - db->buff[node].duty_cycle[k].timestamp).count() < 60){
          message_ += std::to_string(db->buff[node].duty_cycle[k].data) + ",\n";
          k++;
        }
        message_ = "error";
      }


      if(command == 'r'){
        db->init_variables();
        message_ = "ack";
      }

      // sends the info to the client
      boost::asio::async_write( socket_, boost::asio::buffer(message_, message_.length()),
                                boost::bind(&tcp_connection::handle_write, shared_from_this(), 
                                            boost::asio::placeholders::error)); 

    } else {
      delete this;
    }
    memset(request_, 0, sizeof(request_));
  }

  void handle_write(const boost::system::error_code &error){
    if (!error) {

      start();

    } else {
      delete this;
    }
  }

  std::shared_ptr<data_storage> db;
  tcp::socket socket_;
  std::string message_;
  enum {max_length = 1024};
  char request_[max_length];
};

class tcp_server {
public:
  tcp_server(boost::asio::io_service &io_service, std::shared_ptr<data_storage> p_)
      : acceptor_(io_service, tcp::endpoint(tcp::v4(), 17000)), io_service_(io_service), p(p_){
    start_accept();
  }

private:
  void start_accept(){
    tcp_connection::pointer new_connection = tcp_connection::create(io_service_, p);
    acceptor_.async_accept(new_connection->socket(),
                           boost::bind(&tcp_server::handle_accept, this,
                                       new_connection, boost::asio::placeholders::error));
  }

  void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code &error){
    if (!error){

      new_connection->start();
      start_accept();
    }
  }

  std::shared_ptr<data_storage> p;
  tcp::acceptor acceptor_;
  boost::asio::io_service &io_service_;
};

#endif