#include "server.h"

void 
tcp_connection::handle_read(const boost::system::error_code &error, size_t bytes_transferred){

  char command;
  int node;
  char var;
  char node_c;
  int k;
  auto now = std::chrono::system_clock::now();
  float seconds;
  bool flag_dc = false;
  int time_count = 0;
  float total_value = 0;

  if (error){
    std::cout << "Client disconnected" << std::endl;
    return;
  }

  if (request_[0] == 'r'){

    db->init_variables();
    message_ = "ack\n";
  
  } else if (request_[0] == 'd'){

    flag_dc = true;
    message_ = "Disconnected user\n";

  } else if (sscanf(request_, "%c %c %c", &command, &var, &node_c) != 3 || (node = node_c - '0') < 0){

    message_ = "Wrong message\n";

  } else if (node >= NODES && node_c != 'T'){

    message_ = "Node not connected \n";
    
  } else if (command == 'g'){
  
    switch (var){
      case 'l':
        message_ = "l " + std::to_string(node) + " " + std::to_string(db->buff[node].illum[0].data) + "\n";
        break;

      case 'd':
        message_ = "d " + std::to_string(node) + " " + std::to_string(db->buff[node].duty_cycle[0].data) + "\n";
        break;

      case 's':
        message_ = "s " + std::to_string(node) + " " + std::to_string(db->occupancy[node]) + "\n";
        break;

      case 'L':
        message_ = "L " + std::to_string(node) + " " + std::to_string(db->lbound[node]) + "\n";
        break;

      case 'o':
        message_ = "o " + std::to_string(node) + " " + std::to_string(db->ext_illum[node]) + "\n";
        break;

      case 'r':
        message_ = "r " + std::to_string(node) + " " + std::to_string(db->control_ref[node]) + "\n";
        break;

      case 'p':
        if(char(node_c) == 'T'){
          for(int i = 1; i < NODES; i ++){
            total_value += db->inst_power[i];
          }
          message_ = "p T " + std::to_string(total_value) + "\n";
        } else {
          message_ = "p " + std::to_string(node) + " " + std::to_string(db->inst_power[node]) + "\n";
        }
        break;

      case 't':
        
        seconds = std::chrono::duration_cast<std::chrono::seconds>(now - db->last_restart).count();
        message_ = "t " + std::to_string(node) + " " + std::to_string(seconds) + "\n";
        break;

      case 'e':

        if(char(node_c) == 'T'){
          for(int i = 1; i < NODES; i ++){
            total_value += db->energy_coms[i];
          }
          message_ = "e T " + std::to_string(total_value) + "\n";
        } else {
          message_ = "e " + std::to_string(node) + " " + std::to_string(db->energy_coms[node]) + "\n";
        }
        break;
      
      case 'c':

        if(char(node_c) == 'T'){
          for(int i = 1; i < NODES; i ++){
            if (db->buff[node].illum_size == 0) { continue; }
            total_value += db->comfort_error[i] / db->buff[node].illum_size;
          }
          message_ = "c T " + std::to_string(total_value) + "\n";
        } else {
          message_ = "c " + std::to_string(node) + " " + std::to_string(db->comfort_error[node] / db->buff[node].illum_size) + "\n";
        }          
        break;

      case 'v':

        if(char(node_c) == 'T'){
          for (int i = 1; i < NODES; i++){
            if (db->buff[i].illum_size == 0) {continue;}
            total_value += db->comfort_flicker[i] / db->buff[node].illum_size;
          }
          message_ = "v T " + std::to_string(total_value) + "\n";
        } else {
          message_ = "v " + std::to_string(node) + " " + std::to_string(db->comfort_flicker[node] / db->buff[node].illum_size) + "\n";
        }         
        break;

      default:
        message_ = "Wrong message\n";
        break;
    }
  } else if (command == 'b'){

    k = 1;
    switch(var){
      case 'l':
        message_ = "b l " + std::to_string(node) + "\n";
        
        while ( (time_count = std::chrono::duration_cast<std::chrono::seconds>(now - db->buff[node].illum[k].timestamp).count()) < 60){
          //message_ += "sb = " +  std::to_string(time_count) + ", Value = ";
          message_ += std::to_string(db->buff[node].illum[k].data) + ",\n";
          k++;
        }
        break;

      case 'd':
        message_ = "b d " + std::to_string(node) + "\n";
        while ( (time_count = std::chrono::duration_cast<std::chrono::seconds>(now - db->buff[node].duty_cycle[k].timestamp).count()) < 60){
          message_ += std::to_string(db->buff[node].duty_cycle[k].data) + ",\n";
          k++;
        }
        break;

      default:
        message_ = "Wrong message\n";
        break;
    }
  } /* else if (command == 's'){

    k = 1;
    switch(var){
      case 'l':
        dflags[node] = !dflags[node];
        if(dflags[node]) {message_ = "\n";} else { message_ = "s l " + std::to_string(node) + "\n";}
        handle_stream(error);
        break;

      case 'd':
        dflags[node] = !dflags[node];
        if(lflags[node]) {message_ = "\n";} else { message_ = "s d " + std::to_string(node) + "\n"; }
        handle_stream(error);
        break;

      default:
        message_ = "Wrong message\n";
        break;
    }
  }*/
  
  // sends the info to the client
  //if (!(command == 's' && var == 'd') && !(command == 's' && var == 'l'))
    boost::asio::async_write(socket_, boost::asio::buffer(message_, message_.length()),
                              boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                          boost::asio::placeholders::error));
  if (flag_dc){
    try {

      socket_.shutdown(socket_.shutdown_both);
      socket_.close();
    } catch (std::exception &e){

      std::cout << "Error Closing Socket" << e.what() << std::endl;
    }
  }

  memset(request_, 0, sizeof(request_));
}


/* void 
tcp_connection::handle_stream(const boost::system::error_code &error){
  
  bool stream_on = false;
  int seconds;

  for(int i = 1; i < NODES; i++){
    if(lflags[i]){
      stream_on = true;
      if(db->new_lval[i]){

        seconds = std::chrono::duration_cast<std::chrono::milliseconds>(db->buff[i].illum[0].timestamp - db->last_restart).count();
        message_ = std::to_string(db->buff[i].illum[0].data) + " " + std::to_string(seconds) + "\n";
        boost::asio::async_write(socket_, boost::asio::buffer(message_, message_.length()),
                                boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                            boost::asio::placeholders::error));

        db->new_lval[i] = false;
        stream_on = true;
      }
    }

    if (dflags[i]){
      stream_on = true;
      if (db->new_dval[i]){

        seconds = std::chrono::duration_cast<std::chrono::milliseconds>(db->buff[i].duty_cycle[0].timestamp - db->last_restart).count();
        message_ = std::to_string(db->buff[i].duty_cycle[0].data) + " " + std::to_string(seconds) + "\n";
        boost::asio::async_write(socket_, boost::asio::buffer(message_, message_.length()),
                                boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                            boost::asio::placeholders::error));

        db->new_dval[i] = false;
      }
    }
  }

  if(!stream_on){
    message_ = "ack\n";
    boost::asio::async_write(socket_, boost::asio::buffer(message_, message_.length()),
                            boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                        boost::asio::placeholders::error));
  }
}
 */
