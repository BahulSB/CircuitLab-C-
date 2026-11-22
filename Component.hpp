#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <iostream>
#include <string>
using namespace std;

class Debugger;

class CircuitComponent {
protected:
    string componentName;
    bool outputState;

    friend class Debugger;

public:
    CircuitComponent(string name = "Component")
        : componentName(name), outputState(false) {}

    virtual ~CircuitComponent() {}

    virtual void evaluate() = 0;
    virtual void displayInfo() const = 0;

    virtual string getType() const { return componentName; }
    string getLabel() const { return componentName; }
    void setLabel(const string& lbl) { componentName = lbl; }
    bool getPrimaryOutput() const { return outputState; }
    virtual void printState() const { displayInfo(); }
    virtual CircuitComponent* clone() const { return nullptr; }
};

#endif