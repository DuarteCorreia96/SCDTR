#include "server.h"

void 
tcp_connection::handle_read(const boost::system::error_code &error, size_t bytes_transferred){

  if (!error){

    char command;
    int node;
    char var;
    int k;
    auto now = std::chrono::system_clock::now();
    float seconds;

    if (sscanf(request_, "%c %*s", &command) == 1 && command == 'r'){

      db->init_variables();
      message_ = "ack";

    } else if (sscanf(request_, "%c %c %d", &command, &var, &node) != 3){

      message_ = "Wrong message\n";

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
          if(char(node) == 'T'){
            message_ = "p T " + std::to_string(db->inst_power[1] + db->inst_power[2]) + "\n";
          } else {
            message_ = "p " + std::to_string(node) + " " + std::to_string(db->inst_power[node]) + "\n";
          }
          break;

        case 't':
          
          seconds = std::chrono::duration_cast<std::chrono::seconds>(now - db->last_restart).count();
          message_ = "t " + std::to_string(node) + " " + std::to_string(seconds) + "\n";
          break;

        case 'e':
          if(char(node) == 'T'){
            message_ = "e T " + std::to_string(db->energy_coms[1] + db->energy_coms[2]) + "\n";
          } else {
            message_ = "e " + std::to_string(node) + " " + std::to_string(db->energy_coms[node]) + "\n";
          }
          break;
        
        case 'c':
          if(char(node) == 'T'){
            message_ = "c T " + std::to_string(db->comfort_error[1] / db->buff[1].illum.size() + db->comfort_error[2] / db->buff[2].illum.size()) + "\n";
          } else {
            message_ = "c " + std::to_string(node) + " " + std::to_string(db->comfort_error[node] / db->buff[node].illum.size()) + "\n";
          }          
          break;

        case 'v':
          if(char(node) == 'T'){
            message_ = "v T " + std::to_string(db->comfort_flicker[1] / db->buff[1].illum.size() + db->comfort_flicker[2] / db->buff[2].illum.size()) + "\n";
          } else {
            message_ = "v " + std::to_string(node) + " " + std::to_string(db->comfort_flicker[node] / db->buff[node].illum.size()) + "\n";
          }     
          
          break;

        default:
          message_ = "Wrong message";
          break;
      }
    } else if (command == 'b'){

      k = 1;
      switch(var){
        case 'l':
          message_ = "b l " + std::to_string(node) + "\n";
          
          while (std::chrono::duration_cast<std::chrono::seconds>(now - db->buff[node].illum[k].timestamp).count() < 60){
            message_ += std::to_string(db->buff[node].illum[k].data) + ",\n";
            k++;
          }
          break;

        case 'd':
          message_ = "b d " + std::to_string(node) + "\n";
          while (std::chrono::duration_cast<std::chrono::seconds>(now - db->buff[node].duty_cycle[k].timestamp).count() < 60){
            message_ += std::to_string(db->buff[node].duty_cycle[k].data) + ",\n";
            k++;
          }
          break;

        default:
          message_ = "Wrong message";
          break;
      }
    }

    // sends the info to the client
    boost::asio::async_write(socket_, boost::asio::buffer(message_, message_.length()),
                             boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                         boost::asio::placeholders::error));
  } else {
    delete this;
  }

  memset(request_, 0, sizeof(request_));
}