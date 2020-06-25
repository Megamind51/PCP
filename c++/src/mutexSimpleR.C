#include <iostream>
#include <thread>
#include<mutex>
#include <thread>

void func(std::mutex & m, int & x) {
  m.lock();
  std::cout << "Inside thread " << ++x << std::endl;
  m.unlock();
}
int main() {
  std::mutex m; int x = 0;
  std::thread th(func, std::ref(m), std::ref(x));
  th.join();
  std::cout << "Outside thread \n" << x << std::endl;
  return 0;
}
