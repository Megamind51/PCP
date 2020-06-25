#include <iostream>
#include <thread>
#include <unordered_set>
class IntSet {
public:
  bool contains(int i) const {return s_.find(i) != s_.end();}
  void add(int i) {s_.insert(i);}
private: std::unordered_set<int> s_;
};
IntSet s;
int main() {
  std::thread t1([](){
      for (int i = 0; i < 10'000; ++i) {s.add(2 * i);} });
  std::thread t2([](){
      for (int i = 0; i < 10'000; ++i) {s.add(2 * i + 1);}
    });
  t1.join();
  t2.join();
  std::cout << s.contains(1000) << '\n';
}
 
