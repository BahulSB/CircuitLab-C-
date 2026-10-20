# CircuitLab-C++: Interactive EDA Workstation & Logic Simulator

CircuitLab-C++ is an interactive Electronic Design Automation (EDA) simulation workstation built in modern C++ and hardware-accelerated with Raylib. Designed with strict Object-Oriented Programming (OOP) principles, it models real-time digital logic propagation across composite combinational circuits.

---

## Key Features

- **Interactive Schematic Viewport**: Toggle input logic levels (`HIGH` / `LOW`) to observe dynamic signal propagation through logic gates and interconnects in real time.
- **Dynamic Truth Table Matrix**: Integrated verification synthesizer that renders full truth table grids and dynamically tracks active input states.
- **Hierarchical Modular Architecture**: Full Adder built by composing discrete `HalfAdder` (composed of `XOR` and `AND` primitives) and `OR` submodules.
- **Real-Time Signal Analyzer**: Event logger capturing timestamped state changes across all primary and intermediate circuit pins.
- **Automated Clock Generator**: Test bench mode that cycles input states automatically every second.

---

## Object-Oriented Architecture

The simulator backend is strictly decoupled from the visualization engine:

```text
[ Gate Primitive ]  (Base Interface)
       │
       ├──> [ XORGate / ANDGate / ORGate ]
       │
[ Sub-Circuits ]
       ├──> [ HalfAdder ] (Composed of XOR + AND)
       └──> [ FullAdder ] (Composed of 2x HalfAdder + OR)
       └──> [ Mux2to1 ]   (Data routing logic)
```text
Getting Started
Prerequisites
Windows 10/11

MinGW-w64 (GCC 14+)

Raylib 5.0+

Building and Running
Clone the repository:

DOS
git clone [https://github.com/Jagruthi-S/CircuitLab-C-.git](https://github.com/Jagruthi-S/CircuitLab-C-.git)
cd CircuitLab-C-
Compile the Workstation:

DOS
g++ -std=c++17 gui_main.cpp Wire.cpp Gate.cpp -I"raylib-6.2_win32_mingw-w64\include" -L"raylib-6.2_win32_mingw-w64\lib" -lraylib -lopengl32 -lgdi32 -lwinmm -o circuitlab_workstation
Run the Application:

DOS
circuitlab_workstation.exe
Interactive Controls
Left Click on Input Pins (A, B, Cin / D0, D1, SEL): Toggle logic states between 0 and 1.

Full Adder / 2:1 MUX Tabs: Switch active schematic inspection.

Truth Table Button: Toggle matrix synthesizer overlay with active row tracking.

Clock: ON/OFF: Start/stop automated signal cycling.
