#include "FlipFlops.hpp"

FlipFlop::FlipFlop(string name)
    : CircuitComponent(name), Q(false), Q_bar(true) {}

bool FlipFlop::getQ() const {
    return Q;
}

bool FlipFlop::getQBar() const {
    return Q_bar;
}

void FlipFlop::displayInfo() const {
    cout << componentName
         << " | Q = " << Q
         << " | Q_bar = " << Q_bar << endl;
}

// D Flip-Flop

DFlipFlop::DFlipFlop()
    : FlipFlop("D Flip-Flop"), D(false) {}

void DFlipFlop::setInput(bool d) {
    D = d;
}

void DFlipFlop::triggerClock(bool risingEdge) {
    if (risingEdge) {
        Q = D;
        Q_bar = !Q;
    }
}

void DFlipFlop::evaluate() {
    // State changes are handled by triggerClock().
}

// JK Flip-Flop

JKFlipFlop::JKFlipFlop()
    : FlipFlop("JK Flip-Flop"), J(false), K(false) {}

void JKFlipFlop::setInputs(bool j, bool k) {
    J = j;
    K = k;
}

void JKFlipFlop::triggerClock(bool risingEdge) {
    if (risingEdge) {
        if (J == false && K == false) {
            // Hold state
        }
        else if (J == false && K == true) {
            Q = false;
        }
        else if (J == true && K == false) {
            Q = true;
        }
        else {
            Q = !Q;
        }

        Q_bar = !Q;
    }
}

void JKFlipFlop::evaluate() {
    // State changes are handled by triggerClock().
}

// T Flip-Flop

TFlipFlop::TFlipFlop()
    : FlipFlop("T Flip-Flop"), T(false) {}

void TFlipFlop::setInput(bool t) {
    T = t;
}

void TFlipFlop::triggerClock(bool risingEdge) {
    if (risingEdge) {
        if (T) {
            Q = !Q;
        }

        Q_bar = !Q;
    }
}

void TFlipFlop::evaluate() {
    // State changes are handled by triggerClock().
}