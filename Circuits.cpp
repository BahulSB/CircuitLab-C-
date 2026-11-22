#include "Circuits.hpp"
#include <iomanip>

void HalfAdder::evaluate() {
    wire_sum = xor_g.evaluate(in_a, in_b);
    wire_carry = and_g.evaluate(in_a, in_b);
    outputState = wire_sum;
}

void HalfAdder::printState() const {
    std::cout << "[" << getType() << " :: " << componentName << "] "
              << "A=" << in_a << ", B=" << in_b
              << " -> Sum=" << wire_sum << ", Carry=" << wire_carry << "\n";
}

void FullAdder::evaluate() {
    sub_ha1.execute(in_a, in_b);
    stage1_sum = sub_ha1.getSum();
    stage1_carry = sub_ha1.getCarry();

    sub_ha2.execute(stage1_sum, in_cin);
    sum_out = sub_ha2.getSum();
    stage2_carry = sub_ha2.getCarry();

    carry_out = or_g.evaluate(stage1_carry, stage2_carry);
    outputState = sum_out;
}

void FullAdder::printState() const {
    std::cout << "[" << getType() << " :: " << componentName << "] "
              << "A=" << in_a << ", B=" << in_b << ", Cin=" << in_cin
              << " -> Sum=" << sum_out << ", Carry=" << carry_out << "\n";
}

void Mux2to1::evaluate() {
    sel_inv = inv.evaluate(in_sel);
    branch0 = and1.evaluate(in_i0, sel_inv);
    branch1 = and2.evaluate(in_i1, in_sel);
    result = out_or.evaluate(branch0, branch1);
    outputState = result;
}

void Mux2to1::printState() const {
    std::cout << "[" << getType() << " :: " << componentName << "] "
              << "D0=" << in_i0 << ", D1=" << in_i1 << ", Sel=" << in_sel
              << " -> Out=" << result << "\n";
}

RippleCarryAdderSubtractor4Bit::RippleCarryAdderSubtractor4Bit(const std::string& lbl)
    : CircuitComponent(lbl),
      adders{FullAdder(lbl + "_FA0"), FullAdder(lbl + "_FA1"),
             FullAdder(lbl + "_FA2"), FullAdder(lbl + "_FA3")} {}

void RippleCarryAdderSubtractor4Bit::setInputs(const std::array<bool, 4>& inA,
                                               const std::array<bool, 4>& inB,
                                               bool m) {
    A = inA;
    B = inB;
    mode_m = m;
}

void RippleCarryAdderSubtractor4Bit::setInputsFromInt(uint8_t valA, uint8_t valB, bool m) {
    for (int i = 0; i < 4; ++i) {
        A[i] = ((valA >> i) & 1) != 0;
        B[i] = ((valB >> i) & 1) != 0;
    }
    mode_m = m;
}

void RippleCarryAdderSubtractor4Bit::evaluate() {
    bool c = mode_m;
    bool c3 = false;

    for (int i = 0; i < 4; ++i) {
        bool b_xor_m = b_xor_gates[i].evaluate(B[i], mode_m);
        adders[i].execute(A[i], b_xor_m, c);

        result[i] = adders[i].getSum();
        if (i == 2) {
            c3 = adders[i].getCarry();
        }
        c = adders[i].getCarry();
    }

    carry_out = c;
    overflow = (c3 != carry_out);
    outputState = result[3];
}

uint8_t RippleCarryAdderSubtractor4Bit::getResultValue() const {
    uint8_t val = 0;
    for (int i = 0; i < 4; ++i) {
        if (result[i]) {
            val |= static_cast<uint8_t>(1u << i);
        }
    }
    return val;
}

void RippleCarryAdderSubtractor4Bit::printState() const {
    std::cout << "[" << getType() << " :: " << componentName << "] "
              << "Mode=" << (mode_m ? "SUB(1)" : "ADD(0)") << " | "
              << "A=" << A[3] << A[2] << A[1] << A[0] << " "
              << "B=" << B[3] << B[2] << B[1] << B[0] << " -> "
              << "Out=" << result[3] << result[2] << result[1] << result[0]
              << " (Dec: " << static_cast<int>(getResultValue()) << "), "
              << "Cout=" << carry_out << ", Overflow=" << overflow << "\n";
}

void CounterTFlipFlop::evaluate() {
    outputState = q_state;
}

void CounterTFlipFlop::tick() {
    if (reset_line) {
        q_state = false;
    } else if (t_in) {
        q_state = !q_state;
    }
    outputState = q_state;
}

void CounterTFlipFlop::reset() {
    q_state = false;
    outputState = false;
}

void CounterTFlipFlop::printState() const {
    std::cout << "[" << getType() << " :: " << componentName << "] "
              << "T=" << t_in << " -> Q=" << q_state << "\n";
}

SynchronousCounter4Bit::SynchronousCounter4Bit(const std::string& lbl, bool en)
    : SequentialCircuit(lbl),
      ff{CounterTFlipFlop(lbl + "_FF0"), CounterTFlipFlop(lbl + "_FF1"),
         CounterTFlipFlop(lbl + "_FF2"), CounterTFlipFlop(lbl + "_FF3")},
      enable(en) {}

void SynchronousCounter4Bit::evaluate() {
    bool t0 = enable;
    bool t1 = t0 && ff[0].getQ();
    bool t2 = t1 && ff[1].getQ();
    bool t3 = t2 && ff[2].getQ();

    ff[0].setToggle(t0);
    ff[1].setToggle(t1);
    ff[2].setToggle(t2);
    ff[3].setToggle(t3);

    for (auto& flip_flop : ff) {
        flip_flop.evaluate();
    }
    outputState = ff[3].getQ();
}

void SynchronousCounter4Bit::tick() {
    evaluate();
    for (auto& flip_flop : ff) {
        flip_flop.tick();
    }
    outputState = ff[3].getQ();
}

void SynchronousCounter4Bit::reset() {
    for (auto& flip_flop : ff) {
        flip_flop.reset();
    }
    outputState = false;
}

uint8_t SynchronousCounter4Bit::getCount() const {
    uint8_t count = 0;
    for (int i = 0; i < 4; ++i) {
        if (ff[i].getQ()) {
            count |= static_cast<uint8_t>(1u << i);
        }
    }
    return count;
}

std::array<bool, 4> SynchronousCounter4Bit::getStateBits() const {
    return {ff[0].getQ(), ff[1].getQ(), ff[2].getQ(), ff[3].getQ()};
}

void SynchronousCounter4Bit::printState() const {
    std::cout << "[" << getType() << " :: " << componentName << "] "
              << "Q3..Q0 = " << ff[3].getQ() << ff[2].getQ() << ff[1].getQ() << ff[0].getQ()
              << " (Dec: " << static_cast<int>(getCount()) << ")\n";
}

void Debugger::probeHalfAdder(const HalfAdder& ha) {
    std::cout << "  [Probe :: HalfAdder] Sum line: " << ha.wire_sum
              << " | Carry line: " << ha.wire_carry << "\n";
}

void Debugger::probeFullAdder(const FullAdder& fa) {
    std::cout << "  [Probe :: FullAdder] Stage1 Sum: " << fa.stage1_sum
              << " | Stage1 Carry: " << fa.stage1_carry
              << " | Stage2 Carry: " << fa.stage2_carry << "\n";
}

void Debugger::probeAdderSubtractor4Bit(const RippleCarryAdderSubtractor4Bit& alu) {
    std::cout << "  [Probe :: 4-Bit ALU] Mode=" << alu.mode_m
              << " | Cout=" << alu.carry_out
              << " | Overflow=" << alu.overflow << "\n";
}

void Debugger::probeCounter4Bit(const SynchronousCounter4Bit& counter) {
    std::cout << "  [Probe :: 4-Bit Counter] Count="
              << static_cast<int>(counter.getCount()) << "\n";
}

void Debugger::inspectComponent(const CircuitComponent& comp) {
    std::cout << "  [Probe :: " << comp.getType() << "] Label='" << comp.componentName
              << "' | PrimaryOutput=" << comp.outputState << "\n";
}

Breadboard::Breadboard(std::size_t reserve_cap) {
    components.reserve(reserve_cap);
}

Breadboard::~Breadboard() {
    freeComponents();
}

Breadboard::Breadboard(const Breadboard& src)
    : auto_label_counter(src.auto_label_counter) {
    components.reserve(src.components.size());
    for (const CircuitComponent* comp : src.components) {
        CircuitComponent* cloned = comp->clone();
        components.push_back(cloned);
        component_map[cloned->getLabel()] = cloned;
    }
}

Breadboard& Breadboard::operator=(const Breadboard& rhs) {
    if (this != &rhs) {
        freeComponents();
        auto_label_counter = rhs.auto_label_counter;
        components.reserve(rhs.components.size());
        for (const CircuitComponent* comp : rhs.components) {
            CircuitComponent* cloned = comp->clone();
            components.push_back(cloned);
            component_map[cloned->getLabel()] = cloned;
        }
    }
    return *this;
}

void Breadboard::addComponent(CircuitComponent* comp) {
    if (!comp) {
        throw std::invalid_argument("Cannot mount null CircuitComponent onto Breadboard.");
    }
    std::string key = comp->getLabel();

    if (component_map.find(key) != component_map.end()) {
        key = key + "_" + std::to_string(++auto_label_counter);
        comp->setLabel(key);
    }
    components.push_back(comp);
    component_map[key] = comp;
}

void Breadboard::addComponent(const std::string& label, CircuitComponent* comp) {
    if (!comp) {
        throw std::invalid_argument("Cannot mount null CircuitComponent onto Breadboard.");
    }
    if (component_map.find(label) != component_map.end()) {
        throw std::runtime_error("Duplicate component label on Breadboard: " + label);
    }
    comp->setLabel(label);
    components.push_back(comp);
    component_map[label] = comp;
}

CircuitComponent* Breadboard::getComponent(const std::string& label) const {
    auto it = component_map.find(label);
    if (it == component_map.end()) {
        return nullptr;
    }
    return it->second;
}

void Breadboard::simulateAll() {
    std::cout << "\n--- Polymorphic Breadboard Simulation (" << components.size() << " components) ---\n";
    for (CircuitComponent* comp : components) {
        comp->evaluate();
        comp->printState();
    }
}

void Breadboard::freeComponents() {
    for (CircuitComponent* comp : components) {
        delete comp;
    }
    components.clear();
    component_map.clear();
}