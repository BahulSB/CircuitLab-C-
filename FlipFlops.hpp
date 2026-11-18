#ifndef FLIPFLOPS_HPP
#define FLIPFLOPS_HPP

#include "Component.hpp"

class FlipFlop : public CircuitComponent {
protected:
    bool Q;
    bool Q_bar;

public:
    FlipFlop(string name);

    virtual void triggerClock(bool risingEdge) = 0;

    bool getQ() const;
    bool getQBar() const;

    void displayInfo() const override;
};

class DFlipFlop : public FlipFlop {
private:
    bool D;

public:
    DFlipFlop();

    void setInput(bool d);
    void triggerClock(bool risingEdge) override;
    void evaluate() override;
};

class JKFlipFlop : public FlipFlop {
private:
    bool J;
    bool K;

public:
    JKFlipFlop();

    void setInputs(bool j, bool k);
    void triggerClock(bool risingEdge) override;
    void evaluate() override;
};

class TFlipFlop : public FlipFlop {
private:
    bool T;

public:
    TFlipFlop();

    void setInput(bool t);
    void triggerClock(bool risingEdge) override;
    void evaluate() override;
};

#endif