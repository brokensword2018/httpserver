#pragma once

class noncopyable{
//让派生类可以访问构造函数和析构函数，不至于编译不过
protected:
    noncopyable() {}
    ~noncopyable() {}

//派生类如果不显式定义复制构造函数和赋值构造函数， 则不支持这两种构造
private:
    noncopyable(const noncopyable&) {}
    const noncopyable operator=(const noncopyable&) { }
};