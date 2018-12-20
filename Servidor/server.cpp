//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class tcp_connection: public boost::enable_shared_from_this <tcp_connection> {
public:

  typedef boost::shared_ptr<tcp_connection> pointer;
  static pointer create(boost::asio::io_service &io_service){
    return pointer(new tcp_connection(io_service));
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
  tcp_connection(boost::asio::io_service &io_service) : socket_(io_service){}

  void handle_read(const boost::system::error_code &error, size_t bytes_transferred){
    
    std::cout << request_ << std::endl;
    if (!error){
      switch(request_[0]){
        case 'g':
          message_ = "Got a get request:\n";
          break;
        case 's':
          message_ = "Got a set request:\n";
          break;
        default:
          message_ = "Wrong message\n";
      }

      std::cout << "Message to send: " << message_ << std::endl;
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

  tcp::socket socket_;
  std::string message_;
  enum {max_length = 1024};
  char request_[max_length];
};

class tcp_server {
public:
  tcp_server(boost::asio::io_service &io_service)
      : acceptor_(io_service, tcp::endpoint(tcp::v4(), 17000)), io_service_(io_service){
    start_accept();
  }

private:
  void start_accept(){
    tcp_connection::pointer new_connection = tcp_connection::create(io_service_);
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

  tcp::acceptor acceptor_;
  boost::asio::io_service &io_service_;
};

int main(){
  
  try{
    boost::asio::io_service io_service;
    tcp_server server(io_service);
    io_service.run();
  }
  catch (std::exception &e){

    std::cerr << e.what() << std::endl;
  }

  return 0;
}