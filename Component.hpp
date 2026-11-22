#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <iostream>
#include <string>
using namespace std;

class CircuitComponent {
protected:
    string componentName;
    bool outputState;

public:
    CircuitComponent(string name)
        : componentName(name), outputState(false) {}

    virtual ~CircuitComponent() {}

    virtual void evaluate() = 0;

    virtual void displayInfo() const = 0;
};

#endif