#ifndef CIRCUITS_HPP
#define CIRCUITS_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include "Component.hpp"

class PrimitiveAnd {
public:
    bool evaluate(bool a, bool b) const { return a && b; }
};

class PrimitiveOr {
public:
    bool evaluate(bool a, bool b) const { return a || b; }
};

class PrimitiveNot {
public:
    bool evaluate(bool in) const { return !in; }
};

class PrimitiveXor {
public:
    bool evaluate(bool a, bool b) const { return a != b; }
};

class Debugger;

class HalfAdder : public CircuitComponent {
private:
    PrimitiveXor xor_g;
    PrimitiveAnd and_g;

    bool in_a{false};
    bool in_b{false};
    bool wire_sum{false};
    bool wire_carry{false};

    friend class Debugger;

public:
    explicit HalfAdder(const std::string& lbl = "HalfAdder")
        : CircuitComponent(lbl) {}
    ~HalfAdder() override = default;

    void setInputs(bool a, bool b) {
        in_a = a;
        in_b = b;
    }

    void evaluate() override;

    void execute(bool a, bool b) {
        setInputs(a, b);
        evaluate();
    }

    bool getSum() const { return wire_sum; }
    bool getCarry() const { return wire_carry; }

    std::string getType() const override { return "HalfAdder"; }
    void displayInfo() const override { printState(); }
    void printState() const override;
    CircuitComponent* clone() const override { return new HalfAdder(*this); }
};

class FullAdder : public CircuitComponent {
private:
    HalfAdder sub_ha1;
    HalfAdder sub_ha2;
    PrimitiveOr or_g;

    bool in_a{false};
    bool in_b{false};
    bool in_cin{false};

    bool stage1_sum{false};
    bool stage1_carry{false};
    bool stage2_carry{false};
    bool sum_out{false};
    bool carry_out{false};

    friend class Debugger;

public:
    explicit FullAdder(const std::string& lbl = "FullAdder")
        : CircuitComponent(lbl), sub_ha1(lbl + "_HA1"), sub_ha2(lbl + "_HA2") {}
    ~FullAdder() override = default;

    void setInputs(bool a, bool b, bool c_in) {
        in_a = a;
        in_b = b;
        in_cin = c_in;
    }

    void evaluate() override;

    void execute(bool a, bool b, bool c_in) {
        setInputs(a, b, c_in);
        evaluate();
    }

    bool getSum() const { return sum_out; }
    bool getCarry() const { return carry_out; }

    std::string getType() const override { return "FullAdder"; }
    void displayInfo() const override { printState(); }
    void printState() const override;
    CircuitComponent* clone() const override { return new FullAdder(*this); }
};

class Mux2to1 : public CircuitComponent {
private:
    PrimitiveNot inv;
    PrimitiveAnd and1, and2;
    PrimitiveOr out_or;

    bool in_i0{false};
    bool in_i1{false};
    bool in_sel{false};

    bool sel_inv{false};
    bool branch0{false};
    bool branch1{false};
    bool result{false};

    friend class Debugger;

public:
    explicit Mux2to1(const std::string& lbl = "Mux2to1")
        : CircuitComponent(lbl) {}
    ~Mux2to1() override = default;

    void setInputs(bool i0, bool i1, bool sel) {
        in_i0 = i0;
        in_i1 = i1;
        in_sel = sel;
    }

    void evaluate() override;

    void execute(bool i0, bool i1, bool sel) {
        setInputs(i0, i1, sel);
        evaluate();
    }

    bool getOutput() const { return result; }

    std::string getType() const override { return "Mux2to1"; }
    void displayInfo() const override { printState(); }
    void printState() const override;
    CircuitComponent* clone() const override { return new Mux2to1(*this); }
};

class RippleCarryAdderSubtractor4Bit : public CircuitComponent {
private:
    std::array<FullAdder, 4> adders;
    std::array<PrimitiveXor, 4> b_xor_gates;

    std::array<bool, 4> A{false, false, false, false};
    std::array<bool, 4> B{false, false, false, false};
    bool mode_m{false};

    std::array<bool, 4> result{false, false, false, false};
    bool carry_out{false};
    bool overflow{false};

    friend class Debugger;

public:
    explicit RippleCarryAdderSubtractor4Bit(const std::string& lbl = "RCA_Sub_4Bit");
    ~RippleCarryAdderSubtractor4Bit() override = default;

    void setInputs(const std::array<bool, 4>& inA, const std::array<bool, 4>& inB, bool m);
    void setInputsFromInt(uint8_t valA, uint8_t valB, bool m);

    void evaluate() override;

    void execute(uint8_t valA, uint8_t valB, bool m) {
        setInputsFromInt(valA, valB, m);
        evaluate();
    }

    std::array<bool, 4> getResultBits() const { return result; }
    uint8_t getResultValue() const;
    bool getCarryOut() const { return carry_out; }
    bool getOverflow() const { return overflow; }

    std::string getType() const override { return "RippleCarryAdderSubtractor4Bit"; }
    void displayInfo() const override { printState(); }
    void printState() const override;
    CircuitComponent* clone() const override { return new RippleCarryAdderSubtractor4Bit(*this); }
};

class SequentialCircuit : public CircuitComponent {
protected:
    bool reset_line{false};

public:
    explicit SequentialCircuit(const std::string& lbl = "SequentialCircuit")
        : CircuitComponent(lbl) {}
    ~SequentialCircuit() override = default;

    virtual void tick() = 0;
    virtual void reset() = 0;
};

class CounterTFlipFlop : public SequentialCircuit {
private:
    bool t_in{false};
    bool q_state{false};

    friend class Debugger;

public:
    explicit CounterTFlipFlop(const std::string& lbl = "CounterTFlipFlop", bool toggle = false)
        : SequentialCircuit(lbl), t_in(toggle), q_state(false) {}
    ~CounterTFlipFlop() override = default;

    void setToggle(bool t) { t_in = t; }
    bool getQ() const { return q_state; }

    void evaluate() override;
    void tick() override;
    void reset() override;

    std::string getType() const override { return "CounterTFlipFlop"; }
    void displayInfo() const override { printState(); }
    void printState() const override;
    CircuitComponent* clone() const override { return new CounterTFlipFlop(*this); }
};

class SynchronousCounter4Bit : public SequentialCircuit {
private:
    std::array<CounterTFlipFlop, 4> ff;
    bool enable{true};

    friend class Debugger;

public:
    explicit SynchronousCounter4Bit(const std::string& lbl = "SyncCounter4Bit", bool en = true);
    ~SynchronousCounter4Bit() override = default;

    void setEnable(bool en) { enable = en; }
    void evaluate() override;
    void tick() override;
    void reset() override;

    uint8_t getCount() const;
    std::array<bool, 4> getStateBits() const;

    std::string getType() const override { return "SynchronousCounter4Bit"; }
    void displayInfo() const override { printState(); }
    void printState() const override;
    CircuitComponent* clone() const override { return new SynchronousCounter4Bit(*this); }
};

class Debugger {
public:
    static void probeHalfAdder(const HalfAdder& ha);
    static void probeFullAdder(const FullAdder& fa);
    static void probeAdderSubtractor4Bit(const RippleCarryAdderSubtractor4Bit& alu);
    static void probeCounter4Bit(const SynchronousCounter4Bit& counter);
    static void inspectComponent(const CircuitComponent& comp);
};

class Breadboard {
private:
    std::vector<CircuitComponent*> components;
    std::map<std::string, CircuitComponent*> component_map;
    std::size_t auto_label_counter{0};

    friend class Debugger;

public:
    explicit Breadboard(std::size_t reserve_cap = 4);

    ~Breadboard();
    Breadboard(const Breadboard& src);
    Breadboard& operator=(const Breadboard& rhs);

    void addComponent(CircuitComponent* comp);
    void addComponent(const std::string& label, CircuitComponent* comp);

    CircuitComponent* getComponent(const std::string& label) const;

    void simulateAll();
    void freeComponents();

    std::size_t getCount() const { return components.size(); }
    std::size_t getCapacity() const { return components.capacity(); }
};

#endif