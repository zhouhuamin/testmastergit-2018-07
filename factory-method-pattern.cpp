1. 工厂方法模式简述
    工厂方法模式(Factory Method Pattern)是一种常用的对象创建型设计模式, 此模式的核心思想是封装类中不变的部分，
    提取其中个性化善变的部分为独立类，通过依赖注入以达到解耦、复用以及方便后期维护拓展的目的.
    编码实现时定义一个创建产品对象的工厂接口，将实际创建工作推迟到子类当中。它修正了简单工厂模式中不遵守开放—封闭原则。
    核心工厂类不再负责产品的创建，这样核心类成为一个抽象工厂角色，仅负责具体工厂子类必须实现的接口，
    这样进一步抽象化的好处是使得工厂方法模式可以使系统在不修改具体工厂角色的情况下引进新的产品。

2. 适用场合
    在设计的初期，就考虑到产品在后期会进行扩展的情况下，可以使用工厂方法模式；
    产品结构较复杂的情况下，可以使用工厂方法模式；
 
    由于使用设计模式是在详细设计时，就需要进行定夺的，所以，需要权衡多方面的因素，而不能为了使用设计模式而使用设计模式。

3. 代码实现
#include <cstdlib>
#include <iostream>

using namespace std;

class Product
{
public:
    virtual void Show() = 0;
};

class ProductA : public Product
{
public:

    void Show() override
    {
        cout << "It is ProductA" << endl;
    }
};

class ProductB : public Product
{
public:

    void Show() override
    {
        cout << "It is ProductB" << endl;
    }
};

class Factory
{
public:
    virtual Product *CreateProduct() = 0;
};

class FactoryA : public Factory
{
public:

    Product *CreateProduct() override
    {
        return new ProductA();
    }
};

class FactoryB : public Factory
{
public:

    Product *CreateProduct() override
    {
        return new ProductB();
    }
};

int main(int argc, char *argv [])
{
    Factory *factoryA = new FactoryA();
    Product *productA = factoryA->CreateProduct();
    productA->Show();

    Factory *factoryB = new FactoryB();
    Product *productB = factoryB->CreateProduct();
    productB->Show();

    if (factoryA != nullptr)
    {
        delete factoryA;
        factoryA = nullptr;
    }

    if (productA != nullptr)
    {
        delete productA;
        productA = nullptr;
    }

    if (factoryB != nullptr)
    {
        delete factoryB;
        factoryB = nullptr;
    }

    if (productB != nullptr)
    {
        delete productB;
        productB = nullptr;
    }
    return 0;
}

4. 优缺点
    优点: 克服了简单工厂模式违背开放-封闭原则的缺点，又保留了封装对象创建过程的优点，降低客户端和工厂的耦合性。
    缺点: 每增加一个产品，相应的也要增加一个子工厂，加大了额外的开发量。

5. 参考文献：　　

《大话设计模式 C++》

《C++设计模式》


