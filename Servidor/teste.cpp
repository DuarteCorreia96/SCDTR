#include <boost/circular_buffer.hpp>
#include <iostream>
#include <chrono>
#include <ctime>

int main(){

  std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
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

  for (int i : cb){
    std::cout << i << '\n';
  }

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
  while(  std::chrono::duration_cast<std::chrono::seconds>(now - timestamp).count() < 20){
    
    now = std::chrono::system_clock::now();
    int temp1 = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp).count(); 
    if(temp1 != temp2)
      std::cout << temp2 << std::endl;
      temp2 = temp1;
  }

}