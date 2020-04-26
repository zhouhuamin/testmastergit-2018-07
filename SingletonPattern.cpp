设计模式之Singleton模式（对象创建型模式）
1. 意图
   单件模式确保一个类仅有一个实例，并提供一个访问它的全局访问点
2. 动机
    对一些类来说，只有一个实例是很重要的。我们怎样才能保证一个类只有一个实例并且这个实例容易被访问呢？方法是
    让类自身负责保存它的唯一实例。这个类可以保证没有其他实例可以被创建，并且它可以提供一个访问该实例的方法。
    这就是Singleton模式。
3. 适用性
    当类只能有一个实例而且客户可以从一个众所周知的访问点访问它时。
    当这个唯一实例应该是通过子类化可扩展的，并且客户应该无需更改代码就能使用一个扩展的实例时。
4. 效果
    Singleton模式有许多优点：
    1）对唯一实例的受控访问
    2）缩小名称空间
    3）允许对操作和表示的精化

5. 代码示例一：

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

    相信大多数的同仁都喜欢使用上边这种单件模式的实现方法，如果在单线程的情况下，是没有问题的，但如果是多线程，那么就极有可能会返回两个不同的对象，在调用

    CSingleton::getInstance的时候，两个线程如果都同时运行完if判断，而又还没有调用到构造函数的话，想象下后果吧。那该怎么办呢？看下边这个实现吧。


6. 代码示例二：
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


    就是在进程运行的时候就对这个单件进行实例化，可是这样似乎又不能达到延迟实例化的目的啦。如果我们的对象对资源占用非常大，而我们的进行在整个过程中其实并没有用到这个单件，那岂不是白白浪费资源了嘛。还有更好的办法。
7. 代码示例三：
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

    是不是跟预想的一样呢？把单件声明为成员函数中的静态成员，这样既可以达到延迟实例化的目的，又能达到线程安全的目的，
而且看结果的最后，是不是在程序退出的时候，还自动的调用了析构函数呢？这样我们就可以把资源的释放放到析构函数里边，等到程序退出的时候，进程会自动释放这些静态成员的。

 
8. 参考图书：《Head First 设计模式》