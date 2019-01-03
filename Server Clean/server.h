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
    socket_(io_service), db(db_){}

  void handle_read(const boost::system::error_code &error, size_t bytes_transferred);
  void handle_write(const boost::system::error_code &error){
    if (!error) { start();
    } else { delete this; }
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