#include "Utilities.hpp"
#include "Wire.hpp"
#include "Gate.hpp"
#include "Circuits.hpp"

#include <iostream>
#include <vector>
#include <string>

void runGateSimulation(SignalQueue<std::string>& logger) {
    std::cout << "\n--- Logic Gate Evaluation (Gate Module) ---\n"
              << "Available Gates: AND, OR, NOT, NAND, NOR, XOR, XNOR\n"
              << "Enter Gate Name (exact uppercase): ";
    std::string gateName;
    std::cin >> gateName;

    int in1 = 0, in2 = 0;
    std::cout << "Input 1 (0 or 1): ";
    std::cin >> in1;

    bool hasSecond = (gateName != "NOT");
    if (hasSecond) {
        std::cout << "Input 2 (0 or 1): ";
        std::cin >> in2;
    }

    LogicGate gate(gateName, hasSecond ? 2 : 1, in1 != 0, in2 != 0);
    bool out = gate.evaluate();

    std::cout << ">> " << gate << "\n";
    std::cout << ">> Output Result: " << (out ? 1 : 0) << "\n";

    std::string rec = "Gate [" + gateName + "] (" + std::to_string(in1) + 
                      (hasSecond ? ", " + std::to_string(in2) : "") + 
                      ") -> " + std::to_string(out ? 1 : 0);
    logger.push(rec);
}

void runFullAdderSimulation(SignalQueue<std::string>& logger) {
    int a = 0, b = 0, cin = 0;
    std::cout << "\n--- Full Adder Simulation (Circuits Module) ---\n";
    std::cout << "Input A   (0/1): "; std::cin >> a;
    std::cout << "Input B   (0/1): "; std::cin >> b;
    std::cout << "Input Cin (0/1): "; std::cin >> cin;

    FullAdder fa;
    fa.execute(a != 0, b != 0, cin != 0);

    bool sumOut = fa.getSum();
    bool carryOut = fa.getCarry();

    std::cout << ">> Sum: " << sumOut << " | Carry: " << carryOut << "\n";
    std::cout << "\n[Probing Internal Sub-Circuits]:\n";
    Debugger::probeFullAdder(fa);

    std::string rec = "FullAdder (A=" + std::to_string(a) + ", B=" + std::to_string(b) + 
                      ", Cin=" + std::to_string(cin) + ") -> S=" + std::to_string(sumOut) + 
                      ", Cout=" + std::to_string(carryOut);
    logger.push(rec);
}

void runMuxSimulation(SignalQueue<std::string>& logger) {
    int d0 = 0, d1 = 0, sel = 0;
    std::cout << "\n--- 2-to-1 Multiplexer Simulation (Circuits Module) ---\n";
    std::cout << "Input Line D0 (0/1): "; std::cin >> d0;
    std::cout << "Input Line D1 (0/1): "; std::cin >> d1;
    std::cout << "Select Line S (0/1): ";  std::cin >> sel;

    Mux2to1 mux;
    mux.execute(d0 != 0, d1 != 0, sel != 0);
    bool out = mux.getOutput();

    std::cout << ">> Selected Output (Y): " << out << "\n";

    std::string rec = "Mux21 (D0=" + std::to_string(d0) + ", D1=" + std::to_string(d1) + 
                      ", S=" + std::to_string(sel) + ") -> Y=" + std::to_string(out);
    logger.push(rec);
}

void runWireDiagnostics() {
    std::cout << "\n--- Wire Signal Propagation Test (Wire Module) ---\n";
    Wire busA("DATA_BUS_0");
    busA.addDestination("FullAdder.PinA");
    busA.addDestination("Mux2to1.Line0");

    std::cout << "Initial state:\n";
    busA.printStatus();

    std::cout << "\nDriving HIGH signal...\n";
    busA.driveSignal(LogicState::HIGH);
    busA.printStatus();

    std::cout << "\nDriving LOW signal...\n";
    busA.driveSignal(LogicState::LOW);
    busA.printStatus();

    std::cout << "\nInspection History Log:\n";
    busA.printHistory();
}

void runBreadboardSimulation() {
    std::cout << "\n--- Breadboard Dynamic Storage Test ---\n";
    Breadboard bb(2);

    FullAdder* fa1 = new FullAdder();
    fa1->execute(true, true, true);
    bb.addComponent(fa1);

    Mux2to1* mux1 = new Mux2to1();
    mux1->execute(true, false, true);
    bb.addComponent(mux1);

    std::cout << "Breadboard Components Count: " << bb.getCount() << "\n";
    std::cout << "Breadboard Capacity: " << bb.getCapacity() << "\n";
    std::cout << "Components safely mounted and verified in dynamic memory.\n";
}

void runTruthTableEngine() {
    std::cout << "\n--- Automated Truth Table Synthesizer ---\n"
              << " 1. 2-Input AND Gate (Gate Module)\n"
              << " 2. 2-Input XOR Gate (Gate Module)\n"
              << " 3. 1-Bit Full Adder (Circuits Module)\n"
              << " 4. 2-to-1 Multiplexer (Circuits Module)\n"
              << "Select Circuit (1-4): ";

    int choice;
    if (!(std::cin >> choice) || choice < 1 || choice > 4) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Invalid selection.\n";
        return;
    }

    if (choice == 1) {
        std::cout << "\nTruth Table: 2-Input AND Gate\n";
        TruthTableSynthesizer::render({"A", "B"}, {"AND"}, [](const std::vector<int>& in) {
            LogicGate g("AND", 2, in[0] != 0, in[1] != 0);
            return std::vector<int>{g.evaluate() ? 1 : 0};
        });
    } else if (choice == 2) {
        std::cout << "\nTruth Table: 2-Input XOR Gate\n";
        TruthTableSynthesizer::render({"A", "B"}, {"XOR"}, [](const std::vector<int>& in) {
            LogicGate g("XOR", 2, in[0] != 0, in[1] != 0);
            return std::vector<int>{g.evaluate() ? 1 : 0};
        });
    } else if (choice == 3) {
        std::cout << "\nTruth Table: 1-Bit Full Adder (3 Inputs -> 2 Outputs)\n";
        TruthTableSynthesizer::render({"A", "B", "Cin"}, {"Sum", "Cout"}, [](const std::vector<int>& in) {
            FullAdder fa;
            fa.execute(in[0] != 0, in[1] != 0, in[2] != 0);
            return std::vector<int>{fa.getSum() ? 1 : 0, fa.getCarry() ? 1 : 0};
        });
    } else if (choice == 4) {
        std::cout << "\nTruth Table: 2-to-1 Multiplexer\n";
        TruthTableSynthesizer::render({"D0", "D1", "S"}, {"Y"}, [](const std::vector<int>& in) {
            Mux2to1 mux;
            mux.execute(in[0] != 0, in[1] != 0, in[2] != 0);
            return std::vector<int>{mux.getOutput() ? 1 : 0};
        });
    }
}

int main() {
    ComponentRegistry<std::string, std::string> catalog;
    catalog.insert("WIRE_CORE", "Signal line with state tracking & history buffer");
    catalog.insert("GATE_LOGIC", "Primitive and compound logic gates with Signal class");
    catalog.insert("CIRCUIT_FA", "Full Adder composite block with sub-half adders");
    catalog.insert("CIRCUIT_MUX", "2:1 Multiplexer routing block");
    catalog.insert("CONTAINER_BB", "Dynamic Breadboard memory storage (Rule of 3)");

    SignalQueue<std::string> sessionLog;

    int opt = 0;
    while (true) {
        std::cout << "  CircuitLab-C++: Master Simulation Console           \n"
                  << " 1. Evaluate Logic Gate (Gate Module)\n"
                  << " 2. Simulate 1-Bit Full Adder (Circuits Module)\n"
                  << " 3. Simulate 2-to-1 Multiplexer (Circuits Module)\n"
                  << " 4. Run Wire Signal Diagnostics (Wire Module)\n"
                  << " 5. Test Dynamic Breadboard Container\n"
                  << " 6. Synthesize Full Truth Tables\n"
                  << " 7. Display Session Log (SignalQueue Buffer)\n"
                  << " 8. View Component Catalog (ComponentRegistry)\n"
                  << " 9. Exit\n"
                  << "Enter selection (1-9): ";

        if (!(std::cin >> opt)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        switch (opt) {
            case 1: runGateSimulation(sessionLog); break;
            case 2: runFullAdderSimulation(sessionLog); break;
            case 3: runMuxSimulation(sessionLog); break;
            case 4: runWireDiagnostics(); break;
            case 5: runBreadboardSimulation(); break;
            case 6: runTruthTableEngine(); break;
            case 7:
                std::cout << "\n--- SignalQueue Log (" << sessionLog.count() << " events recorded) ---\n";
                sessionLog.printQueue();
                break;
            case 8:
                catalog.showCatalog();
                break;
            case 9:
                std::cout << "Terminating CircuitLab-C++ simulation. Goodbye!\n";
                return 0;
            default:
                std::cout << "Invalid choice. Select between 1 and 9.\n";
                break;
        }
    }

    return 0;
}