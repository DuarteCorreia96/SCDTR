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

std::mutex mtx;

class teste_class{
  public:
    std::shared_ptr<data_storage> p;
    teste_class(std::shared_ptr<data_storage> p_) : p(p_) {}
  private:
};

auto p = std::make_shared<data_storage>();
volatile bool flag = false;

void thread1(){

  while(true){

    mtx.lock();
    std::getline(std::cin, p->k);
    flag = true;
    mtx.unlock();

    std::stringstream msg;
    msg << "thread1: " << p->k << std::endl;
    std::cout << msg.str();
  }

  return;
}

void thread2(teste_class a){

  while (true){
    if(flag == true){

      std::stringstream msg;
      msg << "thread2: " << a.p->k << std::endl;
      std::cout << msg.str();  
      flag = false;    
    }
  }

  return;
}

int main(){
 
  teste_class a(p);

  std::thread threadwrite(thread1);
  std::thread threadread(thread2, a);

  threadwrite.join();
  threadread.join();
}

/*  std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
  typedef boost::circular_buffer<int> circular_buffer;
  circular_buffer cb{3};

  cb.push_front(10);
  cb.push_front(11);
  cb.push_front(12);

  std::cout << std::boolalpha << cb.is_linearized() << '\n';

  circular_buffer::array_range ar1, ar2;

  ar1 = cb.array_one();
  ar2 = cb.array_two();
  std::cout << ar1.second << ";" << ar2.second << '\n';

  for (int i = 0; i < cb.size(); i++)
    std::cout << cb[i] << '\n';

  cb.linearize();
  std::cout << std::boolalpha << cb.is_linearized() << '\n';

  cb.push_front(13);
  cb.push_front(14);
  cb.push_front(15);
  
  ar1 = cb.array_one();
  ar2 = cb.array_two();
  std::cout << ar1.second << ";" << ar2.second << '\n';

  for (int i = 0; i < cb.size(); i++)
    std::cout << cb[i] << '\n';

  std::cout << "Start from the front:" << std::endl;
  for (int i = cb.size()*3; i >= 0; i--)
    std::cout << cb[i] << '\n';

  int teste = 10;
  int *teste2 = &teste;

  std::cout << "teste:" << teste << std::endl;
  *teste2 = 13;
  std::cout << "teste:" << teste << std::endl;

  int temp2 = 20;
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now(); 
  while(  std::chrono::duration_cast<std::chrono::seconds>(now - timestamp).count() < 0){
    
    now = std::chrono::system_clock::now();
    int temp1 = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp).count(); 
    if(temp1 != temp2)
      //std::cout << temp2 << std::endl;
      temp2 = temp1;
  }
  */