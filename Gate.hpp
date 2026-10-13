#ifndef LOGICGATE_H
#define LOGICGATE_H

#include<iostream>
#include<string>
using namespace std;

class Signal{
private:
    bool bitValue;

public:
    Signal(bool initialValue=false);

    bool getValue() const;

    Signal operator&(const Signal& rhs) const;
    Signal operator|(const Signal& rhs) const;
    Signal operator^(const Signal& rhs) const;
    Signal operator~() const;

    bool operator==(const Signal& rhs) const;
};

class LogicGate{
private:
    string gateName;
    int inputCount;
    bool outputValue;
    bool firstInput;
    bool secondInput;

public:
    LogicGate(string name,int count,bool first,bool second=false);

    ~LogicGate();

    bool evaluate();
    bool getOutput() const;

    friend ostream& operator<<(ostream& screen,const LogicGate& item);
};

#endif