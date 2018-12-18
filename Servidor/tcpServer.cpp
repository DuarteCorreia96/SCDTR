//Compile as:  g++ -std=c++11 async_tcp_echo_server.cpp -lpthread -lboost_system -o server
//Run in a separate terminal, before starting client : ./server 17000

//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "tcpServer.h"

session::session(boost::asio::io_service& io_service)
  : socket_(io_service), response_(""), tim(io_service) {
}

tcp::socket& session::socket() {

  return socket_;
}

void session::start() {
  socket_.async_read_some(boost::asio::buffer(request_, max_length),
    boost::bind(&session::handle_read, this, _1, _2 )); 
}

void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
  if (!error) {

    if (request_[0] == 'g') {
      // takes care of get requests
      response_ = db->getRequest(request_);       

    } else if (request_[0] == 's') {

      response_ = ->stream(_request); // starts or stops the stream

    } else if (request_[0] == 'b') {

      response_ = db->lastMinCommand(request_);

      // stream Mode ON

    } else if (request_[0] == '\n') {

      response_ = "Ping received.\n";

    } else {

      response_ = "Request not found.\n";
    }

    // sends the info to the client
    boost::asio::async_write(socket_, boost::asio::buffer(response_, response_.length()),
            boost::bind(&session::handle_write, this, boost::asio::placeholders::error));

    memset(request_, 0, sizeof(request_));
  }
  else{
    
    delete this;
  }
}

void session::handle_write(const boost::system::error_code& error) {
  if (!error) {

    socket_.async_read_some(boost::asio::buffer(request_, max_length),
      boost::bind(&session::handle_read, this,
        boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));

  } else {
    delete this;
  }
}

// asynchronous timer
void session::deadline_handler(const boost::system::error_code & ec)  { 

  std::string sample;

  // if requested to stop the stream
  if (stop == 0) {

    sample = arduino->streaming(address, type);

    if (sample == "Address not valid!\n"){

      // stops streaming
      stop = 1;
      // sends the info to the client
      boost::asio::async_write(socket_, boost::asio::buffer(sample, sample.length()),
          boost::bind(&session::deadline_handler, this, boost::asio::placeholders::error));

    } else if (sample != "no new data") {
      // sends the info to the client
      boost::asio::async_write(socket_, boost::asio::buffer(sample, sample.length()),
          boost::bind(&session::deadline_handler, this, boost::asio::placeholders::error));
    } else {
        tim.expires_from_now(std::chrono::milliseconds(100));
        tim.async_wait(boost::bind(&session::deadline_handler, this, boost::asio::placeholders::error)); 
    }
  } 
}

/** SERVER FUNCTIONS **/

Tcp_server::Tcp_server(boost::asio::io_service& io_service, short port): io_service_(io_service),
  acceptor_(io_service, tcp::endpoint(tcp::v4(), port)){

  start_accept();
}

void Tcp_server::start_accept() {

  session* new_session = new session();
  acceptor_.async_accept(new_session->socket(),
    boost::bind(&Tcp_server::handle_accept, this, new_session, _1));
}


void Tcp_server::handle_accept(session* new_session,
  const boost::system::error_code& error) {

  if (!error){

    new_session->start();

  } else {

    delete new_session;
  }

  start_accept();
}
