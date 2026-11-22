#include <iostream>
#include <string>
#include "Gate.hpp"

using namespace std;

// Signal implementation

Signal::Signal(bool initialValue) : bitValue(initialValue) {}

bool Signal::getValue() const {
    return bitValue;
}

Signal Signal::operator&(const Signal& rhs) const {
    return Signal(bitValue && rhs.bitValue);
}

Signal Signal::operator|(const Signal& rhs) const {
    return Signal(bitValue || rhs.bitValue);
}

Signal Signal::operator^(const Signal& rhs) const {
    return Signal(bitValue != rhs.bitValue);
}

Signal Signal::operator~() const {
    return Signal(!bitValue);
}

bool Signal::operator==(const Signal& rhs) const {
    return bitValue == rhs.bitValue;
}

// LogicGate implementation

LogicGate::LogicGate(string name, int count, bool first, bool second)
    : CircuitComponent(name),
      inputCount(count),
      firstInput(first),
      secondInput(second) {
    evaluate();
}

LogicGate::~LogicGate() {}

void LogicGate::evaluate() {
    if (componentName == "AND" && inputCount == 2)
        outputState = firstInput && secondInput;

    else if (componentName == "OR" && inputCount == 2)
        outputState = firstInput || secondInput;

    else if (componentName == "NOT" && inputCount == 1)
        outputState = !firstInput;

    else if (componentName == "XOR" && inputCount == 2)
        outputState = firstInput != secondInput;

    else if (componentName == "NAND" && inputCount == 2)
        outputState = !(firstInput && secondInput);

    else if (componentName == "NOR" && inputCount == 2)
        outputState = !(firstInput || secondInput);
}

bool LogicGate::getOutput() const {
    return outputState;
}

void LogicGate::displayInfo() const {
    cout << componentName << " Gate, Output = " << outputState << endl;
}

ostream& operator<<(ostream& screen, const LogicGate& item) {
    screen << item.componentName << " Gate: ";

    if (item.inputCount == 1)
        screen << "Input = " << item.firstInput;
    else
        screen << "Inputs = (" << item.firstInput
               << "," << item.secondInput << ")";

    screen << ", Output = " << item.outputState;

    return screen;
}