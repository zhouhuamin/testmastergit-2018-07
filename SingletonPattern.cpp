定义：单件模式确保一个类只有一个实例，并提供一个全局访问点

实现一：

复制代码
#include <iostream>
using namespace std;

class CSingleton
{
public:
    static CSingleton* getInstance();
    static void cleanInstance();
    int getValue();
    void setValue(int iValue);
private:
    int m_iValue;
    static CSingleton* m_pSingleton;
    CSingleton();
    ~CSingleton();
};

CSingleton* CSingleton::m_pSingleton = NULL;

CSingleton::CSingleton()
{
    cout << "Constructor" << endl;
}

CSingleton::~CSingleton()
{
    cout << "Destructor" << endl;
}

CSingleton* CSingleton::getInstance()
{
    if (NULL == m_pSingleton)
    {
        m_pSingleton = new CSingleton();
    }
    return m_pSingleton;
}

void CSingleton::cleanInstance()
{
    delete m_pSingleton;
}

int CSingleton::getValue()
{
    return m_iValue;
}

void CSingleton::setValue(int iValue)
{
    m_iValue = iValue;
}

int main()
{
    CSingleton* pSingleton1 = CSingleton::getInstance();
    CSingleton* pSingleton2 = CSingleton::getInstance();
    pSingleton1->setValue(123);
    if (pSingleton1->getValue() == pSingleton2->getValue())
    {
        cout << "Two objects is the same instance" << endl;
    }
    else
    {
        cout << "Two objects isn't the same instance" << endl;
    }

    CSingleton::cleanInstance();
    return 0;
}
复制代码
相信大多数的同仁都喜欢使用上边这种单件模式的实现方法，如果在单线程的情况下，是没有问题的，但如果是多线程，那么就极有可能会返回两个不同的对象，在调用

CSingleton::getInstance的时候，两个线程如果都同时运行完if判断，而又还没有调用到构造函数的话，想象下后果吧。那该怎么办呢？看下边这个实现吧。
实现二：
复制代码
#include <iostream>
using namespace std;

class CSingleton
{
public:
    static CSingleton* getInstance();
    static void cleanInstance();
    int getValue();
    void setValue(int iValue);
private:
    int m_iValue;
    static CSingleton* m_pSingleton;
    CSingleton();
    ~CSingleton();
};

// 在进程运行开始就实例化该单件，又称“急切”创建实例
CSingleton* CSingleton::m_pSingleton = new CSingleton();

CSingleton::CSingleton()
{
    cout << "Constructor" << endl;
}

CSingleton::~CSingleton()
{
    cout << "Destructor" << endl;
}

CSingleton* CSingleton::getInstance()
{
    return m_pSingleton;
}

void CSingleton::cleanInstance()
{
    delete m_pSingleton;
}

int CSingleton::getValue()
{
    return m_iValue;
}

void CSingleton::setValue(int iValue)
{
    m_iValue = iValue;
}

int main()
{
    CSingleton* pSingleton1 = CSingleton::getInstance();
    CSingleton* pSingleton2 = CSingleton::getInstance();
    pSingleton1->setValue(123);
    if (pSingleton1->getValue() == pSingleton2->getValue())
    {
        cout << "Two objects is the same instance" << endl;
    }
    else
    {
        cout << "Two objects isn't the same instance" << endl;
    }

    CSingleton::cleanInstance();
    return 0;
}
复制代码
哈哈，看清楚了吗？就是在进程运行的时候就对这个单件进行实例化，可是这样似乎又不能达到延迟实例化的目的啦。如果我们的对象对资源占用非常大，而我们的进行在整个过程中其实并没有用到这个单件，那岂不是白白浪费资源了嘛。还有更好的办法。
实现三：
复制代码
#include <iostream>
using namespace std;

class CSingleton
{
public:
    static CSingleton* getInstance();
    int getValue();
    void setValue(int iValue);
private:
    int m_iValue;
    CSingleton();
    ~CSingleton();
};

CSingleton::CSingleton()
{
    cout << "Constructor" << endl;
}

CSingleton::~CSingleton()
{
    cout << "Destructor" << endl;
}

CSingleton* CSingleton::getInstance()
{
    static CSingleton single;
    return &single;
}

int CSingleton::getValue()
{
    return m_iValue;
}

void CSingleton::setValue(int iValue)
{
    m_iValue = iValue;
}

int main()
{
    cout << "Process begin" << endl;
    CSingleton* pSingleton1 = CSingleton::getInstance();
    CSingleton* pSingleton2 = CSingleton::getInstance();
    pSingleton1->setValue(123);
    if (pSingleton1->getValue() == pSingleton2->getValue())
    {
        cout << "Two objects is the same instance" << endl;
    }
    else
    {
        cout << "Two objects isn't the same instance" << endl;
    }
    return 0;
}
复制代码
看下运行结果吧：
Process begin
Constructor
Two objects is the same instance
Destructor

是不是跟预想的一样呢？把单件声明为成员函数中的静态成员，这样既可以达到延迟实例化的目的，又能达到线程安全的目的，而且看结果的最后，是不是在程序退出的时候，还自动的调用了析构函数呢？这样我们就可以把资源的释放放到析构函数里边，等到程序退出的时候，进程会自动释放这些静态成员的。

 
参考图书：《Head First 设计模式》