#include "Circuits.hpp"

int main() {
    std::cout << "=======================================\n";
    std::cout << "       CircuitLab-C++ Simulation       \n";
    std::cout << "=======================================\n\n";

    // Allocate dynamic sub-circuits on breadboard slot container
    Breadboard board;
    auto* fa = new FullAdder();
    auto* mux = new Mux2to1();

    board.addComponent(fa);
    board.addComponent(mux);

    std::cout << "--- 1-Bit Full Adder Truth Table ---\n";
    std::cout << "A | B | Cin | Sum | Carry\n";
    std::cout << "-------------------------\n";
    for (int a = 0; a <= 1; ++a) {
        for (int b = 0; b <= 1; ++b) {
            for (int cin = 0; cin <= 1; ++cin) {
                fa->execute(a, b, cin);
                std::cout << a << " | " << b << " |  " << cin << "  |  "
                          << fa->getSum() << "  |   " << fa->getCarry() << "\n";
                
                // Inspect private wire signals directly via friend class
                Debugger::probeFullAdder(*fa);
            }
        }
    }

    std::cout << "\n--- 2:1 Multiplexer Truth Table ---\n";
    std::cout << "I0 | I1 | Sel | Out\n";
    std::cout << "-------------------\n";
    for (int i0 = 0; i0 <= 1; ++i0) {
        for (int i1 = 0; i1 <= 1; ++i1) {
            for (int sel = 0; sel <= 1; ++sel) {
                mux->execute(i0, i1, sel);
                std::cout << i0 << "  | " << i1 << "  |  " << sel << "  |  " << mux->getOutput() << "\n";
            }
        }
    }

    std::cout << "\n--- Breadboard Diagnostics ---\n";
    std::cout << "Mounted Components : " << board.getCount() << "\n";
    std::cout << "Allocated Capacity : " << board.getCapacity() << "\n";

    return 0;
}