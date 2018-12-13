#include "client.h"

using boost::asio::ip::tcp;

void client::start(tcp::resolver::iterator endpoint_iter){

  start_connect(endpoint_iter);
  start_read_console();
}


void 
client::start_connect(tcp::resolver::iterator endpoint_iter){

  socket_.async_connect(endpoint_iter->endpoint(), boost::bind(&client::handle_connect, this, _1, endpoint_iter));
}

void
client::handle_connect(const boost::system::error_code &ec, tcp::resolver::iterator endpoint_iter){

  start_read();
}

void 
client::start_read(){
  
  boost::asio::async_read_until(socket_, input_buffer_, '\n', boost::bind(&client::handle_read, this, _1, _2));
}

void 
client::handle_read(const boost::system::error_code &ec, std::size_t length){

  if (!ec){

    std::string line;
    std::istream is(&input_buffer_);
    std::getline(is, line);
    
    if (!line.empty()){
      
      std::cout << "Received: " << line << "\n";
    }

    start_read();
  } else {

    std::cout << "Error on receive: " << ec.message() << "\n";
  }
}

void 
client::start_read_console(){
  
  boost::asio::async_read_until(input_, console_buffer_, '\n', boost::bind(&client::handle_read_console, this, _1, _2));
}
  
void 
client::handle_read_console(const boost::system::error_code &ec, std::size_t length){

  if (!ec){

    std::string line, terminated_line;
    std::istream is(&console_buffer_);
    std::getline(is, line);

    if (!line.empty()){ // Empty messages are ignored.

      std::cout << "Sending: " << line << "\n";
      terminated_line = line + std::string("\n");
      std::size_t n = terminated_line.size();
      terminated_line.copy(send_buffer_, n);
      boost::asio::async_write(socket_, boost::asio::buffer(send_buffer_, n), boost::bind(&client::handle_send, this, _1, _2));
    }

    start_read_console();

  } else {
    std::cout << "Error on handle_read_console: " << ec.message() << "\n";
  }
}

void 
client::handle_send(const boost::system::error_code &ec, std::size_t length){

  std::cout << "Sent " << length << " bytes" << std::endl;
}
