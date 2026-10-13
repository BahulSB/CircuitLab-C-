#ifndef CIRCUITS_HPP
#define CIRCUITS_HPP

#include <iostream>
#include <string>
#include <cstddef>

// Primitive logic gate blocks
class AndGate {
public:
    bool evaluate(bool a, bool b) const { return a && b; }
};

class OrGate {
public:
    bool evaluate(bool a, bool b) const { return a || b; }
};

class NotGate {
public:
    bool evaluate(bool in) const { return !in; }
};

class XorGate {
public:
    bool evaluate(bool a, bool b) const { return a != b; }
};

class Debugger; // Forward decl for direct wire probing

// -------------------------------------------------------------
// Composite Sub-Circuits
// -------------------------------------------------------------

// 1-Bit Half Adder composed of XOR and AND gates
class HalfAdder {
private:
    XorGate xor_g;
    AndGate and_g;

    // Internal probing lines
    bool wire_sum{false};
    bool wire_carry{false};

    friend class Debugger;

public:
    HalfAdder() = default;

    void execute(bool in_a, bool in_b) {
        wire_sum = xor_g.evaluate(in_a, in_b);
        wire_carry = and_g.evaluate(in_a, in_b);
    }

    bool getSum() const { return wire_sum; }
    bool getCarry() const { return wire_carry; }
};

// 1-Bit Full Adder composed of 2 HalfAdders + 1 OR Gate
class FullAdder {
private:
    HalfAdder sub_ha1;
    HalfAdder sub_ha2;
    OrGate or_g;

    // Internal wire states across stages
    bool stage1_sum{false};
    bool stage1_carry{false};
    bool stage2_carry{false};
    bool sum_out{false};
    bool carry_out{false};

    friend class Debugger;

public:
    FullAdder() = default;

    void execute(bool a, bool b, bool c_in) {
        // Stage 1: Add input bits A & B
        sub_ha1.execute(a, b);
        stage1_sum = sub_ha1.getSum();
        stage1_carry = sub_ha1.getCarry();

        // Stage 2: Add stage 1 sum with carry-in
        sub_ha2.execute(stage1_sum, c_in);
        sum_out = sub_ha2.getSum();
        stage2_carry = sub_ha2.getCarry();

        // Stage 3: OR gate for final carry line
        carry_out = or_g.evaluate(stage1_carry, stage2_carry);
    }

    bool getSum() const { return sum_out; }
    bool getCarry() const { return carry_out; }
};

// 2-to-1 Multiplexer composed of NOT, AND, and OR gates
class Mux2to1 {
private:
    NotGate inv;
    AndGate and1, and2;
    OrGate out_or;

    bool sel_inv{false};
    bool branch0{false};
    bool branch1{false};
    bool result{false};

public:
    Mux2to1() = default;

    // Output line: Y = (I0 AND NOT S) OR (I1 AND S)
    void execute(bool i0, bool i1, bool sel) {
        sel_inv = inv.evaluate(sel);
        branch0 = and1.evaluate(i0, sel_inv);
        branch1 = and2.evaluate(i1, sel);
        result = out_or.evaluate(branch0, branch1);
    }

    bool getOutput() const { return result; }
};

// -------------------------------------------------------------
// Debugger Friend Class
// -------------------------------------------------------------
class Debugger {
public:
    static void probeHalfAdder(const HalfAdder& ha) {
        std::cout << "  [Probe :: HalfAdder] Sum line: " << ha.wire_sum 
                  << " | Carry line: " << ha.wire_carry << "\n";
    }

    static void probeFullAdder(const FullAdder& fa) {
        std::cout << "  [Probe :: FullAdder] Stage1 Sum: " << fa.stage1_sum 
                  << " | Stage1 Carry: " << fa.stage1_carry 
                  << " | Stage2 Carry: " << fa.stage2_carry << "\n";
    }
};

// -------------------------------------------------------------
// Breadboard Dynamic Memory Container (No Polymorphism/Inheritance)
// -------------------------------------------------------------
enum class SlotKind { FULL_ADDER, MUX21 };

struct BoardSlot {
    SlotKind kind;
    union {
        FullAdder* fa;
        Mux2to1* mux;
    };

    BoardSlot() : kind(SlotKind::FULL_ADDER), fa(nullptr) {}
    BoardSlot(FullAdder* ptr) : kind(SlotKind::FULL_ADDER), fa(ptr) {}
    BoardSlot(Mux2to1* ptr) : kind(SlotKind::MUX21), mux(ptr) {}
};

class Breadboard {
private:
    BoardSlot* slots{nullptr};
    std::size_t max_capacity{0};
    std::size_t active_count{0};

    void reallocateStorage() {
        std::size_t new_cap = (max_capacity == 0) ? 2 : max_capacity * 2;
        auto* new_slots = new BoardSlot[new_cap];
        for (std::size_t i = 0; i < active_count; ++i) {
            new_slots[i] = slots[i];
        }
        delete[] slots;
        slots = new_slots;
        max_capacity = new_cap;
    }

public:
    explicit Breadboard(std::size_t init_cap = 2) 
        : max_capacity(init_cap), active_count(0) {
        slots = new BoardSlot[max_capacity];
    }

    // Rule of Three: Destructor
    ~Breadboard() {
        freeComponents();
        delete[] slots;
    }

    // Rule of Three: Copy Constructor
    Breadboard(const Breadboard& src) 
        : max_capacity(src.max_capacity), active_count(src.active_count) {
        slots = new BoardSlot[max_capacity];
        for (std::size_t i = 0; i < active_count; ++i) {
            if (src.slots[i].kind == SlotKind::FULL_ADDER) {
                slots[i] = BoardSlot(new FullAdder(*src.slots[i].fa));
            } else {
                slots[i] = BoardSlot(new Mux2to1(*src.slots[i].mux));
            }
        }
    }

    // Rule of Three: Copy Assignment Operator
    Breadboard& operator=(const Breadboard& rhs) {
        if (this != &rhs) {
            freeComponents();
            delete[] slots;

            max_capacity = rhs.max_capacity;
            active_count = rhs.active_count;
            slots = new BoardSlot[max_capacity];
            for (std::size_t i = 0; i < active_count; ++i) {
                if (rhs.slots[i].kind == SlotKind::FULL_ADDER) {
                    slots[i] = BoardSlot(new FullAdder(*rhs.slots[i].fa));
                } else {
                    slots[i] = BoardSlot(new Mux2to1(*rhs.slots[i].mux));
                }
            }
        }
        return *this;
    }

    void addComponent(FullAdder* fa) {
        if (active_count >= max_capacity) reallocateStorage();
        slots[active_count++] = BoardSlot(fa);
    }

    void addComponent(Mux2to1* mux) {
        if (active_count >= max_capacity) reallocateStorage();
        slots[active_count++] = BoardSlot(mux);
    }

    void freeComponents() {
        for (std::size_t i = 0; i < active_count; ++i) {
            if (slots[i].kind == SlotKind::FULL_ADDER) {
                delete slots[i].fa;
            } else if (slots[i].kind == SlotKind::MUX21) {
                delete slots[i].mux;
            }
        }
        active_count = 0;
    }

    std::size_t getCount() const { return active_count; }
    std::size_t getCapacity() const { return max_capacity; }
};

#endif // CIRCUITS_HPP