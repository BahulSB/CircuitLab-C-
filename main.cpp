#include "Utilities.hpp"
#include <iostream>
#include <vector>
#include <string>

namespace CircuitLab {

    const int OP_AND  = 1;
    const int OP_OR   = 2;
    const int OP_NOT  = 3;
    const int OP_NAND = 4;
    const int OP_NOR  = 5;
    const int OP_XOR  = 6;
    const int OP_XNOR = 7;

    class Gate {
    public:
        static int solve(int gateType, int x, int y = 0) {
            switch (gateType) {
                case OP_AND:  return (x & y);
                case OP_OR:   return (x | y);
                case OP_NOT:  return (x == 0 ? 1 : 0);
                case OP_NAND: return !(x & y);
                case OP_NOR:  return !(x | y);
                case OP_XOR:  return (x ^ y);
                case OP_XNOR: return !(x ^ y);
                default:      return 0;
            }
        }
    };

    class FullAdder {
    public:
        static void compute(int a, int b, int cin, int& sum, int& cout) {
            sum = (a ^ b) ^ cin;
            cout = ((a ^ b) & cin) | (a & b);
        }
    };

    class Multiplexer2to1 {
    public:
        static int route(int line0, int line1, int selectBit) {
            if (selectBit == 0) return line0;
            return line1;
        }
    };

}

void testSingleGate(SignalQueue<std::string>& logger) {
    std::cout << "\nSelect Gate: 1.AND 2.OR 3.NOT 4.NAND 5.NOR 6.XOR 7.XNOR\nChoice: ";
    int gateChoice;
    if (!(std::cin >> gateChoice) || gateChoice < 1 || gateChoice > 7) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Invalid gate choice.\n";
        return;
    }

    int p1 = 0, p2 = 0;
    std::cout << "Pin 1 (0 or 1): ";
    std::cin >> p1;

    if (gateChoice != CircuitLab::OP_NOT) {
        std::cout << "Pin 2 (0 or 1): ";
        std::cin >> p2;
    }

    int res = CircuitLab::Gate::solve(gateChoice, p1, p2);
    std::cout << "-> Evaluated Output: " << res << "\n";

    std::string record = "GateTest [Type=" + std::to_string(gateChoice) +
                         " In=(" + std::to_string(p1) + "," + std::to_string(p2) +
                         ") Out=" + std::to_string(res) + "]";
    logger.push(record);
}

void testFullAdder(SignalQueue<std::string>& logger) {
    int bitA, bitB, bitCin;
    std::cout << "\nInput A: "; std::cin >> bitA;
    std::cout << "Input B: "; std::cin >> bitB;
    std::cout << "Input Cin: "; std::cin >> bitCin;

    int sumRes = 0, coutRes = 0;
    CircuitLab::FullAdder::compute(bitA, bitB, bitCin, sumRes, coutRes);

    std::cout << "-> Sum = " << sumRes << ", Cout = " << coutRes << "\n";

    std::string record = "Adder [A=" + std::to_string(bitA) + " B=" + std::to_string(bitB) +
                         " Cin=" + std::to_string(bitCin) + " | S=" + std::to_string(sumRes) +
                         " C=" + std::to_string(coutRes) + "]";
    logger.push(record);
}

void testMux(SignalQueue<std::string>& logger) {
    int d0, d1, sel;
    std::cout << "\nInput D0: "; std::cin >> d0;
    std::cout << "Input D1: "; std::cin >> d1;
    std::cout << "Select line: "; std::cin >> sel;

    int res = CircuitLab::Multiplexer2to1::route(d0, d1, sel);
    std::cout << "-> Output Y = " << res << "\n";

    std::string record = "Mux21 [D0=" + std::to_string(d0) + " D1=" + std::to_string(d1) +
                         " S=" + std::to_string(sel) + " | Y=" + std::to_string(res) + "]";
    logger.push(record);
}

void runTruthTableEngine() {
    std::cout << "\nCircuits for Truth Table Generation:\n"
              << " 1. AND Gate (2 in)\n"
              << " 2. XOR Gate (2 in)\n"
              << " 3. Full Adder (3 in -> 2 out)\n"
              << " 4. 2:1 Mux (3 in -> 1 out)\n"
              << "Select: ";

    int sel;
    if (!(std::cin >> sel) || sel < 1 || sel > 4) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Invalid choice.\n";
        return;
    }

    if (sel == 1) {
        TruthTableSynthesizer::render({"A", "B"}, {"AND"}, [](const std::vector<int>& in) {
            return std::vector<int>{CircuitLab::Gate::solve(CircuitLab::OP_AND, in[0], in[1])};
        });
    } else if (sel == 2) {
        TruthTableSynthesizer::render({"A", "B"}, {"XOR"}, [](const std::vector<int>& in) {
            return std::vector<int>{CircuitLab::Gate::solve(CircuitLab::OP_XOR, in[0], in[1])};
        });
    } else if (sel == 3) {
        TruthTableSynthesizer::render({"A", "B", "Cin"}, {"Sum", "Cout"}, [](const std::vector<int>& in) {
            int s = 0, c = 0;
            CircuitLab::FullAdder::compute(in[0], in[1], in[2], s, c);
            return std::vector<int>{s, c};
        });
    } else if (sel == 4) {
        TruthTableSynthesizer::render({"D0", "D1", "S"}, {"Y"}, [](const std::vector<int>& in) {
            return std::vector<int>{CircuitLab::Multiplexer2to1::route(in[0], in[1], in[2])};
        });
    }
}

int main() {
    ComponentRegistry<std::string, std::string> deviceCatalog;
    deviceCatalog.insert("GATE_AND", "Basic 2-Input AND Gate");
    deviceCatalog.insert("GATE_XOR", "Basic 2-Input XOR Gate");
    deviceCatalog.insert("CKT_ADDER", "1-Bit Full Adder Unit");
    deviceCatalog.insert("CKT_MUX", "2-to-1 Multiplexer Unit");

    SignalQueue<std::string> sessionLogger;

    int userOption = 0;
    while (true) {
        std::cout << "  Circuit Simulator Dashboard \n"
                  << "Enter : \n1. Test Logic Gates\n"
                  << "2. Simulate 1-Bit Full Adder\n"
                  << "3. Simulate 2:1 Multiplexer\n"
                  << "4. Generate Truth Tables\n"
                  << "5. Show Log Buffer (SignalQueue)\n"
                  << "6. View Component Catalog\n"
                  << "7. Quit\n"
                  << "Option (1-7): ";

        if (!(std::cin >> userOption)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        if (userOption == 1) {
            testSingleGate(sessionLogger);
        } else if (userOption == 2) {
            testFullAdder(sessionLogger);
        } else if (userOption == 3) {
            testMux(sessionLogger);
        } else if (userOption == 4) {
            runTruthTableEngine();
        } else if (userOption == 5) {
            std::cout << "\nTotal logged operations: " << sessionLogger.count() << "\n";
            sessionLogger.printQueue();
        } else if (userOption == 6) {
            deviceCatalog.showCatalog();
        } else if (userOption == 7) {
            std::cout << "Exiting simulation.\n";
            break;
        } else {
            std::cout << "Invalid input option.\n";
        }
    }

    return 0;
}
