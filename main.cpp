#include <cstdlib>
#include <iostream>
#include <thread>

using namespace std;

void hello()
{
    std::cout << "Hello Concurrent World\n";
}

int main(int argc, char** argv)
{
    std::thread t(hello);
    t.join();

    return 0;
}

