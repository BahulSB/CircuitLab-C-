# CircuitLab-C++: Interactive EDA Workstation & Logic Simulator

CircuitLab-C++ is an interactive Electronic Design Automation (EDA) simulation workstation built in modern C++ and hardware-accelerated using Raylib. Designed under strict Object-Oriented Programming (OOP) principles, it models real-time digital logic propagation across composite combinational circuits.

---

## Key Features

- **Interactive Schematic Viewport**: Toggle input logic levels (`HIGH` / `LOW`) to observe dynamic signal propagation through logic gates and interconnects in real time.
- **Dynamic Truth Table Matrix**: Integrated verification synthesizer that renders full truth table grids and dynamically tracks active input states with live row highlighting.
- **Hierarchical Modular Architecture**: Full Adder built by composing discrete `HalfAdder` submodules (composed of `XOR` and `AND` primitives) and `OR` gates.
- **Real-Time Signal Analyzer**: Event logger capturing timestamped state changes across all primary and intermediate circuit pins.
- **Automated Clock Generator**: Test bench mode that cycles input states automatically every second across all binary combinations.

---

## Object-Oriented Architecture

The simulator backend is strictly decoupled from the visualization engine:

```
[ Gate Primitive ]  (Base Interface)
       │
       ├──> [ XORGate / ANDGate / ORGate ]
       │
[ Sub-Circuits ]
       ├──> [ HalfAdder ] (Composed of XOR + AND)
       └──> [ FullAdder ] (Composed of 2x HalfAdder + OR)
       └──> [ Mux2to1 ]   (Data routing logic)
```

- **`Gate` / `Wire`**: Base primitives managing state transitions, fan-out connections, and signal states (`LogicState::HIGH` / `LOW`).
- **`FullAdder`**: Composite multi-level circuit demonstrating component encapsulation and hierarchical interconnect routing.
- **`Mux2to1`**: Multiplexer block implementing conditional channel selection.
- **`TruthTableSynthesizer`**: Analytical module generating state spaces and exhaustive truth verification matrices.

---

## Getting Started

### Prerequisites

- **Operating System**: Windows 10 / 11
- **Compiler**: MinGW-w64 with GCC 14+ (`g++`) supporting C++17
- **Graphics Library**: Raylib 5.0+ (or Raylib 6.2 MinGW build)

---

### Project Structure

```
CircuitLab-C-/
├── Gate.hpp / Gate.cpp       # Gate abstractions (AND, OR, XOR)
├── Wire.hpp / Wire.cpp       # Node interconnects and logic level tracking
├── Circuits.hpp              # HalfAdder, FullAdder, and Mux2to1 definitions
├── Utilities.hpp             # Helper utilities and data structures
├── gui_main.cpp              # Raylib-powered interactive EDA workstation
├── README.md                 # Project documentation
└── .gitignore                # Git exclusions
```

---

### Building and Running

1. **Clone the Repository:**
   ```cmd
   git clone [https://github.com/BahulSB/CircuitLab-C-.git](https://github.com/BahulSB/CircuitLab-C-.git)
   cd CircuitLab-C-
   ```

2. **Compile the Workstation:**
   ```cmd
   g++ -std=c++17 gui_main.cpp Wire.cpp Gate.cpp -I"raylib-6.2_win32_mingw-w64\include" -L"raylib-6.2_win32_mingw-w64\lib" -lraylib -lopengl32 -lgdi32 -lwinmm -o circuitlab_workstation
   ```

3. **Launch the Simulation:**
   ```cmd
   circuitlab_workstation.exe
   ```

---

## Interactive Controls

| Control | Action |
| :--- | :--- |
| **Left Click on Input Switches** | Toggle pin logic levels between `0` (LOW) and `1` (HIGH) |
| **`Full Adder` / `2:1 MUX` Tabs** | Switch between circuit schematic inspections |
| **`Truth Table` Button** | Toggle full matrix synthesizer view with live row tracking |
| **`Clock: ON / OFF`** | Start or pause the automated binary signal generator |
