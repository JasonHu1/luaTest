#ifndef __THIRD_FORMAT_H_
#define __THIRD_FORMAT_H_

#include <iostream>
#include <string>
#include <list>

class ThirdFormat
{
public:
    std::string payload;
    std::string name;
    int retVal;
    ThirdFormat(){

    }
    void setpayload(std::string s)
    {
        this->payload = s;
    }
    
    void setname(std::string s)
    {
        this->name = s;
    }

    std::string getpayload()
    {
        return this->payload;
    }

    void printpayload()
    {
        printf("Hello, my payload is %s!\n", this->payload.c_str());
    }
};

class A 
{
protected:
    std::string name;

public:
    A(){

    }
    A(std::string s)
    {
        this->name = s;
    }

    std::string getName()
    {
        return this->name;
    }

    void printName()
    {
        printf("Hello, my name is %s!\n", this->name.c_str());
    }
};


#endif//__THIRD_FORMAT_H_
