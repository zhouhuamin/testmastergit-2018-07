/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
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

    int b = 10;
    int a = 10;
    int c = 10;

    return 0;
}

