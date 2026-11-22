#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>

#include "Exceptions.hpp"
#include "Wire.hpp"
#include "Gate.hpp"
#include "FlipFlops.hpp"
#include "Circuits.hpp"
#include "Utilities.hpp"

int readInt(const std::string& prompt, int minVal, int maxVal) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= minVal && val <= maxVal) {
            return val;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  [Input Error] Enter a value between " << minVal << " and " << maxVal << ".\n";
    }
}

bool readBit(const std::string& prompt) {
    return readInt(prompt, 0, 1) == 1;
}

// =========================================================================
// 1. Basic Logic Gates & Overloaded Operators (Signal & LogicGate)
// =========================================================================
void runBasicGatesAndOperators(SignalQueue<std::string>& logger) {
    std::cout << "\n--- [1] Basic Logic Gates & Overloaded Operators ---\n";
    bool a = readBit("Enter Signal A (0/1): ");
    bool b = readBit("Enter Signal B (0/1): ");

    Signal sigA(a), sigB(b);
    std::cout << std::boolalpha
              << "\n[Overloaded Signal Operators]\n"
              << "  A & B  = " << (sigA & sigB).getValue() << "\n"
              << "  A | B  = " << (sigA | sigB).getValue() << "\n"
              << "  A ^ B  = " << (sigA ^ sigB).getValue() << "\n"
              << "  ~A     = " << (~sigA).getValue() << "\n"
              << "  A == B = " << (sigA == sigB) << "\n\n";

    LogicGate andG("AND", 2, a, b);
    LogicGate orG("OR", 2, a, b);
    LogicGate xorG("XOR", 2, a, b);
    LogicGate nandG("NAND", 2, a, b);
    LogicGate norG("NOR", 2, a, b);
    LogicGate notG("NOT", 1, a);

    std::cout << "[LogicGate Evaluations & Stream Insertion (operator<<)]\n"
              << "  " << andG << "\n"
              << "  " << orG << "\n"
              << "  " << xorG << "\n"
              << "  " << nandG << "\n"
              << "  " << norG << "\n"
              << "  " << notG << "\n";

    logComponentExecution(andG);
    logger.push("LogicGates evaluated for A=" + std::to_string(a) + ", B=" + std::to_string(b));
}

// =========================================================================
// 2. Full Adder & 2:1 MUX
// =========================================================================
void runFullAdderAndMux(SignalQueue<std::string>& logger) {
    std::cout << "\n--- [2] Full Adder & 2:1 Multiplexer ---\n";
    bool a   = readBit("Enter Input A   (0/1): ");
    bool b   = readBit("Enter Input B   (0/1): ");
    bool cin = readBit("Enter Input Cin (0/1): ");

    FullAdder fa("FA_Main");
    fa.execute(a, b, cin);
    fa.printState();
    Debugger::probeFullAdder(fa);
    logComponentExecution(fa);

    bool sel = readBit("\nEnter 2:1 MUX Select Line S (0 selects A, 1 selects B): ");
    Mux2to1 mux("MUX_Main");
    mux.execute(a, b, sel);
    mux.printState();
    logComponentExecution(mux);

    logger.push("FullAdder(Sum=" + std::to_string(fa.getSum()) + ", Cout=" + std::to_string(fa.getCarry()) +
                ") & Mux2to1(Out=" + std::to_string(mux.getOutput()) + ")");
}

// =========================================================================
// 3. Truth Table Synthesizer + Export to CSV File
// =========================================================================
void runTruthTableAndCSV(SignalQueue<std::string>& logger) {
    std::cout << "\n--- [3] Truth Table Synthesizer + CSV File Export ---\n"
              << "  1. 1-Bit Full Adder (A, B, Cin -> Sum, Cout)\n"
              << "  2. 2-to-1 Multiplexer (D0, D1, Sel -> Y)\n"
              << "  3. 2-Input XOR Gate (A, B -> XOR)\n";
    int choice = readInt("Select Circuit (1-3): ", 1, 3);

    TruthTableData tableData;

    if (choice == 1) {
        auto evalFA = [](const std::vector<int>& in) {
            FullAdder fa;
            fa.execute(in[0] != 0, in[1] != 0, in[2] != 0);
            return std::vector<int>{fa.getSum() ? 1 : 0, fa.getCarry() ? 1 : 0};
        };
        TruthTableSynthesizer::render({"A", "B", "Cin"}, {"Sum", "Cout"}, evalFA);
        tableData = TruthTableSynthesizer::synthesize("FullAdder_1Bit", {"A", "B", "Cin"}, {"Sum", "Cout"}, evalFA);
    } else if (choice == 2) {
        auto evalMux = [](const std::vector<int>& in) {
            Mux2to1 mux;
            mux.execute(in[0] != 0, in[1] != 0, in[2] != 0);
            return std::vector<int>{mux.getOutput() ? 1 : 0};
        };
        TruthTableSynthesizer::render({"D0", "D1", "Sel"}, {"Y"}, evalMux);
        tableData = TruthTableSynthesizer::synthesize("Mux_2to1", {"D0", "D1", "Sel"}, {"Y"}, evalMux);
    } else {
        auto evalXor = [](const std::vector<int>& in) {
            LogicGate g("XOR", 2, in[0] != 0, in[1] != 0);
            return std::vector<int>{g.getOutput() ? 1 : 0};
        };
        TruthTableSynthesizer::render({"A", "B"}, {"XOR"}, evalXor);
        tableData = TruthTableSynthesizer::synthesize("XOR_Gate", {"A", "B"}, {"XOR"}, evalXor);
    }

    std::string filename;
    std::cout << "Enter CSV filename to export (e.g., truth_table.csv): ";
    std::cin >> filename;

    CircuitFileManager::exportTruthTableCSV(filename, tableData);
    std::cout << "  [Success] Truth table exported to '" << filename << "'.\n";
    logger.push("Exported truth table '" + tableData.title + "' to " + filename);
}

// =========================================================================
// 4. 4-Bit Ripple Carry Adder / Subtractor
// =========================================================================
void runRippleCarryAdderSubtractor(SignalQueue<std::string>& logger) {
    std::cout << "\n--- [4] 4-Bit Ripple Carry Adder / Subtractor ---\n";
    int valA = readInt("Enter 4-bit unsigned Operand A (0..15): ", 0, 15);
    int valB = readInt("Enter 4-bit unsigned Operand B (0..15): ", 0, 15);
    bool modeSub = readBit("Operation Mode (0 = ADD [A + B], 1 = SUBTRACT [A - B]): ");

    RippleCarryAdderSubtractor4Bit alu("ALU_4Bit");
    alu.execute(static_cast<uint8_t>(valA), static_cast<uint8_t>(valB), modeSub);

    alu.printState();
    Debugger::probeAdderSubtractor4Bit(alu);
    logComponentExecution(alu);

    logger.push("4-Bit ALU (" + std::to_string(valA) + (modeSub ? " - " : " + ") +
                std::to_string(valB) + ") = " + std::to_string(alu.getResultValue()));
}

// =========================================================================
// 5. Sequential Flip-Flops (D, JK, T) & 4-Bit Synchronous Clocked Counter
// =========================================================================
void runFlipFlopsAndCounter(SignalQueue<std::string>& logger) {
    std::cout << "\n--- [5] Sequential Flip-Flops (D, JK, T) & 4-Bit Clocked Counter ---\n";

    DFlipFlop  dff;
    JKFlipFlop jkff;
    TFlipFlop  tff;

    bool dIn = readBit("Enter D input for D Flip-Flop   (0/1): ");
    bool jIn = readBit("Enter J input for JK Flip-Flop  (0/1): ");
    bool kIn = readBit("Enter K input for JK Flip-Flop  (0/1): ");
    bool tIn = readBit("Enter T input for T Flip-Flop   (0/1): ");

    dff.setInput(dIn);
    jkff.setInputs(jIn, kIn);
    tff.setInput(tIn);

    std::cout << "\n[Triggering Rising Clock Edge (risingEdge = true)]\n";
    dff.triggerClock(true);
    jkff.triggerClock(true);
    tff.triggerClock(true);

    dff.displayInfo();
    jkff.displayInfo();
    tff.displayInfo();

    int pulses = readInt("\nEnter number of clock pulses for 4-Bit Synchronous Counter (1..16): ", 1, 16);
    SynchronousCounter4Bit counter("SyncCounter_4Bit", true);
    counter.reset();

    std::cout << "Initial State: ";
    counter.printState();

    for (int clk = 1; clk <= pulses; ++clk) {
        counter.tick();
        std::cout << "  Clock Pulse " << std::setw(2) << clk << " -> ";
        counter.printState();
    }

    Debugger::probeCounter4Bit(counter);
    logComponentExecution(counter);
    logger.push("Flip-Flops & 4-Bit Counter simulated for " + std::to_string(pulses) +
                " ticks -> Count=" + std::to_string(counter.getCount()));
}

// =========================================================================
// 6. Polymorphic Breadboard Simulation & Fault Exception Demo
// =========================================================================
void runPolymorphicBreadboardAndFaults(SignalQueue<std::string>& logger) {
    std::cout << "\n--- [6] Polymorphic Breadboard Simulation & Fault Exception Demo ---\n";
    Breadboard board(4);

    auto* fa = new FullAdder("BB_FullAdder");
    fa->execute(true, false, true);

    auto* mux = new Mux2to1("BB_Mux2to1");
    mux->execute(false, true, true);

    auto* alu = new RippleCarryAdderSubtractor4Bit("BB_ALU4");
    alu->setInputsFromInt(7, 3, false);

    auto* cnt = new SynchronousCounter4Bit("BB_Counter4");
    cnt->tick();
    cnt->tick();

    board.addComponent(fa);
    board.addComponent(mux);
    board.addComponent("BB_ALU4", alu);
    board.addComponent("BB_Counter4", cnt);

    board.simulateAll();

    std::cout << "\n--- Hardware Fault Exception Simulation ---\n"
              << "  1. Floating Wire Fault (Uninitialized Wire read)\n"
              << "  2. Short-Circuit Fault (HIGH driven directly into LOW bus)\n"
              << "  3. Duplicate Component Label Fault on Breadboard\n";
    int faultChoice = readInt("Select Fault Scenario (1-3): ", 1, 3);

    if (faultChoice == 1) {
        Wire wFloat("NET_UNCONNECTED");
        wFloat.printStatus();
        if (wFloat.getState() == LogicState::FLOATING) {
            throw FloatingWireException(wFloat.getLabel());
        }
    } else if (faultChoice == 2) {
        Wire wVcc("NET_VCC"), wGnd("NET_GND");
        wVcc.driveSignal(LogicState::HIGH);
        wGnd.driveSignal(LogicState::LOW);
        if (wVcc.getState() != LogicState::FLOATING &&
            wGnd.getState() != LogicState::FLOATING &&
            wVcc.getState() != wGnd.getState()) {
            throw ShortCircuitException("NET_VCC <-> NET_GND");
        }
    } else {
        board.addComponent("BB_ALU4", new FullAdder("Duplicate"));
    }

    logger.push("Polymorphic Breadboard simulation completed.");
}

// =========================================================================
// 7. Save / Load Circuit Snapshot to File (Stream Manipulators)
// =========================================================================
void runSaveLoadSnapshot(SignalQueue<std::string>& logger) {
    std::cout << "\n--- [7] Save / Load Circuit Snapshot to File ---\n"
              << "  1. Save current testbench circuit snapshot to file\n"
              << "  2. Load and inspect circuit snapshot from file\n";
    int action = readInt("Choice (1-2): ", 1, 2);

    std::string filename;
    std::cout << "Enter snapshot filename (e.g., circuit_snapshot.txt): ";
    std::cin >> filename;

    if (action == 1) {
        HalfAdder ha("HA_Snap");
        ha.execute(true, true);

        FullAdder fa("FA_Snap");
        fa.execute(true, true, false);

        Mux2to1 mux("MUX_Snap");
        mux.execute(false, true, true);

        TFlipFlop tff;
        tff.setInput(true);
        tff.triggerClock(true);

        std::vector<CircuitSnapshotRecord> records = {
            {ha.getLabel(),  ha.getType(),  true,  true,  ha.getSum()},
            {fa.getLabel(),  fa.getType(),  true,  true,  fa.getSum()},
            {mux.getLabel(), mux.getType(), false, true,  mux.getOutput()},
            {"TFF_Snap",     "TFlipFlop",   true,  false, tff.getQ()}
        };

        CircuitFileManager::saveCircuitSnapshot(filename, "CircuitLab_v2_Eval_Snapshot", records);
        std::cout << "  [Success] Saved " << records.size() << " component snapshots to '" << filename << "'.\n";
        logger.push("Saved circuit snapshot to " + filename);
    } else {
        std::string loadedTitle;
        auto records = CircuitFileManager::loadCircuitSnapshot(filename, loadedTitle);

        std::cout << "\nLoaded Circuit Snapshot: " << loadedTitle << "\n"
                  << std::left << std::boolalpha
                  << std::setw(20) << "Label"
                  << std::setw(24) << "Type"
                  << std::setw(10) << "In_A"
                  << std::setw(10) << "In_B"
                  << std::setw(10) << "Output" << "\n"
                  << std::string(74, '-') << "\n";

        for (const auto& rec : records) {
            std::cout << std::setw(20) << rec.label
                      << std::setw(24) << rec.type
                      << std::setw(10) << rec.inputA
                      << std::setw(10) << rec.inputB
                      << std::setw(10) << rec.outputState << "\n";
        }
        logger.push("Loaded circuit snapshot from " + filename);
    }
}

// =========================================================================
// MAIN MENU LOOP
// =========================================================================
int main() {
    SignalQueue<std::string> sessionLog;

    std::cout << "=================================================================\n"
              << "   CircuitLab-C++ v2.0 :: Digital Logic & OOPS Simulator CLI\n"
              << "=================================================================\n";

    while (true) {
        std::cout << "\n========================= MAIN MENU =========================\n"
                  << "  1. Basic Logic Gates & Overloaded Operators\n"
                  << "  2. Full Adder & 2:1 MUX\n"
                  << "  3. Truth Table Synthesizer + Export to CSV file\n"
                  << "  4. 4-Bit Ripple Carry Adder / Subtractor\n"
                  << "  5. Sequential Flip-Flops (D, JK, T) & 4-Bit Clocked Counter\n"
                  << "  6. Polymorphic Breadboard Simulation & Fault Exception Demo\n"
                  << "  7. Save / Load Circuit Snapshot to File\n"
                  << "  8. Exit\n"
                  << "=============================================================\n";

        int choice = readInt("Select an option (1-8): ", 1, 8);

        if (choice == 8) {
            std::cout << "\nSession Summary (" << sessionLog.count() << " operations logged):\n";
            sessionLog.printQueue();
            std::cout << "Exiting CircuitLab-C++ v2.0. Goodbye!\n";
            break;
        }

        try {
            switch (choice) {
                case 1: runBasicGatesAndOperators(sessionLog); break;
                case 2: runFullAdderAndMux(sessionLog); break;
                case 3: runTruthTableAndCSV(sessionLog); break;
                case 4: runRippleCarryAdderSubtractor(sessionLog); break;
                case 5: runFlipFlopsAndCounter(sessionLog); break;
                case 6: runPolymorphicBreadboardAndFaults(sessionLog); break;
                case 7: runSaveLoadSnapshot(sessionLog); break;
            }
        } catch (const CircuitException& e) {
            std::cerr << "\n[CircuitException Caught] " << e.what() << "\n"
                      << "  -> Simulation safely recovered and returned to Main Menu.\n";
        } catch (const std::exception& e) {
            std::cerr << "\n[Runtime Exception Caught] " << e.what() << "\n"
                      << "  -> Simulation safely recovered and returned to Main Menu.\n";
        }
    }

    return 0;
}