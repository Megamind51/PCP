#include <iostream>
#include <string>
#include <thread>
#include <mutex>
static std::mutex m;
void print(const std::string & s)
{
    std::lock_guard<std::mutex> lock(m);
    for (char c : s)
        std::cout.put(c);
    std::cout << std::endl;
}
int main() {
    auto f1 = std::thread (print, "Hello from a first thread");
    auto f2 = std::thread (print, "Hello from a second thread");
    print("Hello from the main thread");
    f2.join();
f1.join(); }

