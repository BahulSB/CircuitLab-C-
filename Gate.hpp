#ifndef LOGICGATE_H
#define LOGICGATE_H

#include <iostream>
#include <string>
#include "Component.hpp"

using namespace std;

class Signal {
private:
    bool bitValue;

public:
    Signal(bool initialValue = false);

    bool getValue() const;

    Signal operator&(const Signal& rhs) const;
    Signal operator|(const Signal& rhs) const;
    Signal operator^(const Signal& rhs) const;
    Signal operator~() const;

    bool operator==(const Signal& rhs) const;
};

class LogicGate : public CircuitComponent {
private:
    int inputCount;
    bool firstInput;
    bool secondInput;

public:
    LogicGate(string name, int count, bool first, bool second = false);

    ~LogicGate();

    void evaluate() override;

    bool getOutput() const;

    void displayInfo() const override;

    friend ostream& operator<<(ostream& screen, const LogicGate& item);
};

#endif