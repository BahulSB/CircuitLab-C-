Paste this entire clean `README.md` (without any citation tags inside it) into your `README.md` file:

```markdown
# CircuitLab-C++ v2.0: Interactive EDA Workstation & Digital Logic Simulator

CircuitLab-C++ is a modular Electronic Design Automation (EDA) simulation suite built in modern C++. Designed around strict Object-Oriented Programming (OOP) principles, v2.0 unifies combinational circuits, sequential clocked flip-flops, arithmetic logic units (ALUs), polymorphic breadboard simulation, custom exception handling, and persistent File I/O across both an interactive CLI console and a Raylib-powered graphical workstation.

---

## Key Features (v1.0 & v2.0)

- **Unified Polymorphic Architecture (`CircuitComponent`)**: Runtime dynamic binding (`virtual evaluate()`, `displayInfo()`, `printState()`, `clone()`) across basic logic gates, combinational modules, arithmetic units, and sequential circuits.
- **Combinational & Arithmetic Circuits**: `HalfAdder`, `FullAdder`, `Mux2to1`, and a 4-Bit `RippleCarryAdderSubtractor4Bit` supporting 2's complement subtraction, carry-out, and signed overflow detection.
- **Sequential Logic & Clocked Counters**: Edge-triggered `DFlipFlop`, `JKFlipFlop`, and `TFlipFlop` modules alongside a 4-Bit `SynchronousCounter4Bit` with clock pulse stepping and reset lines.
- **Polymorphic Breadboard & Friend Debugger**: Deep-copying `Breadboard` container (`Rule of 3`) managing heterogeneous `CircuitComponent*` instances, paired with a `Debugger` friend class for internal wire probing.
- **Automated Truth Table Synthesizer & CSV Export**: Exhaustive $2^N$ truth table generation (`TruthTableSynthesizer`) with console rendering and direct `.csv` export (`CircuitFileManager`).
- **Persistent Circuit Snapshots & Stream Manipulators**: Save and reload circuit configurations/states to disk using `<fstream>` (`std::ofstream`, `std::ifstream`) formatted with `std::setw`, `std::left`, and `std::boolalpha`.
- **Hardware Fault Exception Hierarchy**: Custom `CircuitException` hierarchy (`FloatingWireException`, `ShortCircuitException`, `FileIOException`) for safe recovery from floating high-impedance wires (`LogicState::FLOATING`), bus contention, and file stream errors.
- **Interactive Raylib EDA Workstation (`gui_main.cpp`)**: Real-time schematic viewport, dynamic truth table matrix with live row highlighting, signal event queue, and automated clock generator.

---

## Object-Oriented Architecture

The simulator backend is decoupled from both the CLI and Raylib visualization frontends:


```

```
                    [ CircuitComponent ]  (Abstract Base Class)
                             │
    ┌────────────────────────┼────────────────────────┬───────────────────────┐
    ▼                        ▼                        ▼                       ▼

```

[ LogicGate ]           [ FlipFlop ]            [ Combinational ]       [ SequentialCircuit ]
(AND/OR/XOR/NOT)       (D / JK / T FF)        (HalfAdder, FullAdder,     (SynchronousCounter4Bit,
Mux2to1, 4-Bit ALU)        CounterTFlipFlop)

```

### Core OOP Concepts Demonstrated

| OOP Concept | Implementation in CircuitLab-C++ v2.0 |
| :--- | :--- |
| **Abstraction & Inheritance** | `CircuitComponent` abstract base class (`Component.hpp`) extended by `LogicGate`, `FlipFlop`, `HalfAdder`, `FullAdder`, `Mux2to1`, `RippleCarryAdderSubtractor4Bit`, and `SequentialCircuit`. |
| **Runtime Polymorphism** | `Breadboard::simulateAll()` iterates over `std::vector<CircuitComponent*>` calling overridden `evaluate()` and `printState()` methods. |
| **Operator Overloading** | `Signal` class (`&`, `|`, `^`, `~`, `==`), `LogicGate` stream insertion (`operator<<`), and `Rule of 3` copy assignment (`Wire`, `SignalQueue`, `Breadboard`). |
| **Templates & STL Algorithms** | Class templates `SignalQueue<T>` and `ComponentRegistry<K, V>`, function template `logComponentExecution<T>()`, and `std::find` / `std::distance`. |
| **Friend Classes & Functions** | `Debugger` granted friend access to inspect internal wires and stage states of `CircuitComponent`, `HalfAdder`, `FullAdder`, `RippleCarryAdderSubtractor4Bit`, and `SynchronousCounter4Bit`. |
| **Exception Handling** | `CircuitException` base class with specialized `FloatingWireException`, `ShortCircuitException`, and `FileIOException` caught cleanly in `main.cpp`. |
| **File I/O & Manipulators** | `CircuitFileManager` utilizing `std::ofstream`, `std::ifstream`, `std::setw`, `std::left`, and `std::boolalpha`. |

---

## Project Structure


```

CircuitLab-C-/
├── Component.hpp             # Abstract base class CircuitComponent
├── Exceptions.hpp            # CircuitException, FloatingWireException, ShortCircuitException, FileIOException
├── Wire.hpp / Wire.cpp       # Signal interconnects, LogicState tracking, Rule of 3 history buffer
├── Gate.hpp / Gate.cpp       # Overloaded Signal class & LogicGate primitives
├── FlipFlops.hpp / .cpp      # Edge-triggered D, JK, and T Flip-Flops
├── Circuits.hpp / .cpp       # Adders, 2:1 MUX, 4-Bit ALU, 4-Bit Synchronous Counter, Breadboard, Debugger
├── Utilities.hpp             # SignalQueue, ComponentRegistry, TruthTableSynthesizer, CircuitFileManager
├── main.cpp                  # Interactive v2.0 CLI Simulation Console
├── gui_main.cpp              # Raylib-powered interactive EDA workstation
├── README.md                 # Project documentation
└── .gitignore                # Git exclusions

```

---

## Building and Running

### Prerequisites

- **Operating System**: Windows 10 / 11 (or Linux/macOS with GCC/Clang)
- **Compiler**: MinGW-w64 `g++` supporting C++14 / C++17
- **Graphics Library (Optional, for GUI target only)**: Raylib 5.0+ / 6.2 MinGW build

### 1. Clone the Repository

```cmd
git clone [https://github.com/BahulSB/CircuitLab-C-.git](https://github.com/BahulSB/CircuitLab-C-.git)
cd CircuitLab-C-

```

### 2. Build & Run the v2.0 Interactive CLI Console

```cmd
g++ -std=c++14 -Wall -Wextra main.cpp Wire.cpp Gate.cpp FlipFlops.cpp Circuits.cpp -o circuitlab_cli.exe
circuitlab_cli.exe

```

### 3. Build & Run the Raylib GUI Workstation (Optional)

```cmd
g++ -std=c++17 gui_main.cpp Wire.cpp Gate.cpp Circuits.cpp -I"raylib-6.2_win32_mingw-w64\include" -L"raylib-6.2_win32_mingw-w64\lib" -lraylib -lopengl32 -lgdi32 -lwinmm -o circuitlab_workstation.exe
circuitlab_workstation.exe

```

---

## Interactive Modes

### CLI Menu Options (`main.cpp`)

1. **Basic Logic Gates & Overloaded Operators**: Test `Signal` bitwise operators (`&`, `|`, `^`, `~`, `==`) and `LogicGate` evaluations.
2. **Full Adder & 2:1 MUX**: Evaluate 1-bit addition and multiplexer channel routing with `Debugger` sub-circuit probing.
3. **Truth Table Synthesizer + CSV Export**: Generate full truth tables and export formatted results directly to `.csv` files.
4. **4-Bit Ripple Carry Adder / Subtractor**: Perform 4-bit binary addition (`M=0`) and 2's complement subtraction (`M=1`) with carry-out and overflow diagnostics.
5. **Sequential Flip-Flops & 4-Bit Clocked Counter**: Trigger rising clock edges on `D`, `JK`, and `T` flip-flops and step through multi-cycle synchronous counter pulses.
6. **Polymorphic Breadboard & Fault Exception Demo**: Run dynamic polymorphic evaluation across mounted components and test `FloatingWireException` / `ShortCircuitException` recovery.
7. **Save / Load Circuit Snapshot**: Persist and reload component states to disk using `std::setw`, `std::left`, and `std::boolalpha`.
8. **Exit**: Display the `SignalQueue` session execution log and terminate cleanly.

### Raylib GUI Controls (`gui_main.cpp`)

| Control | Action |
| --- | --- |
| **Left Click on Input Switches** | Toggle pin logic levels between `0` (LOW) and `1` (HIGH) |
| **`Full Adder` / `2:1 MUX` Tabs** | Switch between circuit schematic inspections |
| **`Truth Table` Button** | Toggle full matrix synthesizer view with live row tracking |
| **`Clock: ON / OFF`** | Start or pause the automated binary signal generator |

```

