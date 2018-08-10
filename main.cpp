/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
现象
在使用C++开发Android过程中采用了C++的Exception机制，因而导致NDK编译失败，抛出错误: exception handling disabled, use -fexceptions to enable

原因
此问题的出现是编译器的异常异常捕获被禁用了，需要在Android.mk文件中开启。

解决方案
在Android.mk文件中添加： LOCAL_CPPFLAGS += -fexceptions就可以了。
在Application.mk文件中添加APP_CPPFLAGS += -fexceptions，这种配置会自动应用到工程的所有模块当中 

*/

1、Activity中获取：
DisplayMetrics metrics = new DisplayMetrics();
getWindowManager().getDefaultDisplay().getMetrics(metrics);
int width = metrics.widthPixels;
int height = metrics.heightPixels;
此时获取到的是该Activity的实际占屏尺寸

2、上下文获取：
DisplayMetrics metrics = new DisplayMetrics();
metrics = getApplicationContext().getResources().getDisplayMetrics();
int width = metrics.widthPixels;int height = metrics.heightPixels;
此时获取到的是系统的显示尺寸




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
    int d = 10;

    return 0;
}

