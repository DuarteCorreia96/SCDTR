#ifndef _CLIENT_CUST_
#define _CLIENT_CUST_

#include <unistd.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class client{
  
  public:

    client(boost::asio::io_service &io_service):socket_(io_service), input_(io_service, ::dup(STDIN_FILENO)),console_buffer_(100000){}
    void start(tcp::resolver::iterator endpoint_iter);

  private:
    
    void start_connect(tcp::resolver::iterator endpoint_iter);
    void handle_connect(const boost::system::error_code &ec, tcp::resolver::iterator endpoint_iter);
    void start_read();
    void handle_read(const boost::system::error_code &ec, std::size_t length);
    void start_read_console();
    void handle_read_console(const boost::system::error_code &ec, std::size_t length);
    void handle_send(const boost::system::error_code &ec, std::size_t length);

    tcp::socket socket_;
    boost::asio::streambuf input_buffer_;
    boost::asio::posix::stream_descriptor input_;
    boost::asio::streambuf console_buffer_;
    enum{ max_length = 1024};
    char send_buffer_[max_length];
};

#endif