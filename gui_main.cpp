#include "raylib.h"
#include "Circuits.hpp"
#include "Wire.hpp"
#include "Gate.hpp"
#include "Utilities.hpp"
#include <sys/stat.h>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

// MinGW UCRT compatibility stub
extern "C" int stat64i32(const char* path, struct _stat64i32* buffer) {
    return _stat64i32(path, buffer);
}

// UI Color Palette
const Color BG_COLOR       = { 18, 21, 26, 255 };
const Color PANEL_COLOR    = { 28, 33, 41, 255 };
const Color PANEL_ALT      = { 22, 26, 33, 255 };
const Color BORDER_COLOR   = { 50, 60, 75, 255 };
const Color WIRE_HIGH      = { 46, 204, 113, 255 };  // Bright Neon Green
const Color WIRE_LOW       = { 85, 95, 110, 255 };   // Slate Inactive Gray
const Color GATE_COLOR     = { 41, 128, 185, 255 };  // EDA Blue
const Color ACCENT_COLOR   = { 52, 152, 219, 255 };
const Color HIGHLIGHT_ROW  = { 39, 174, 96, 60 };

// Log entry structure for Signal Queue
struct LogEvent {
    double timestamp;
    std::string message;
};

// Global log list
std::vector<LogEvent> eventLogs;
void AddLog(const std::string& msg) {
    if (eventLogs.size() > 5) {
        eventLogs.erase(eventLogs.begin());
    }
    eventLogs.push_back({ GetTime(), msg });
}

// Helper: Clickable Switch
bool DrawSwitch(Rectangle bounds, const char* label, bool state) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, bounds);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color btnColor = state ? WIRE_HIGH : PANEL_ALT;
    DrawRectangleRec(bounds, btnColor);
    DrawRectangleLinesEx(bounds, 2.0f, hovered ? RAYWHITE : BORDER_COLOR);

    std::string text = std::string(label) + ": " + (state ? "1" : "0");
    DrawText(text.c_str(), bounds.x + 10, bounds.y + 10, 15, state ? BLACK : RAYWHITE);

    if (clicked) {
        AddLog(std::string(label) + " toggled -> " + (!state ? "HIGH" : "LOW"));
        return !state;
    }
    return state;
}

// Helper: Wire router
void DrawCircuitWire(Vector2 start, Vector2 end, bool state) {
    Color wireColor = state ? WIRE_HIGH : WIRE_LOW;
    float thickness = state ? 3.0f : 2.0f;

    Vector2 mid1 = { (start.x + end.x) / 2.0f, start.y };
    Vector2 mid2 = { (start.x + end.x) / 2.0f, end.y };

    DrawLineEx(start, mid1, thickness, wireColor);
    DrawLineEx(mid1, mid2, thickness, wireColor);
    DrawLineEx(mid2, end, thickness, wireColor);

    DrawCircleV(start, 3.5f, wireColor);
    DrawCircleV(end, 3.5f, wireColor);
}

int main() {
    const int screenWidth = 980;
    const int screenHeight = 670;

    InitWindow(screenWidth, screenHeight, "CircuitLab-C++ :: Enterprise EDA Workstation");
    SetTargetFPS(60);

    // Backend OOP Instances
    FullAdder fa;
    Mux2to1 mux;

    // Simulation Input States
    bool inA = false, inB = false, inCin = false;
    bool inD0 = false, inD1 = false, inSel = false;
    
    // UI Navigation State
    int currentView = 0;   // 0 = Schematic, 1 = Truth Table
    int currentCircuit = 0; // 0 = Full Adder, 1 = 2:1 MUX
    
    // Auto-Clock Generator State
    bool autoClock = false;
    double lastClockTick = 0;
    int clockCounter = 0;

    AddLog("Engine initialized. Systems nominal.");

    while (!WindowShouldClose()) {
        // --- 1. Auto-Clock Pulse Engine ---
        if (autoClock && (GetTime() - lastClockTick >= 1.0)) {
            lastClockTick = GetTime();
            clockCounter = (clockCounter + 1) % 8;
            if (currentCircuit == 0) {
                inA   = (clockCounter >> 2) & 1;
                inB   = (clockCounter >> 1) & 1;
                inCin = (clockCounter >> 0) & 1;
            } else {
                inD0  = (clockCounter >> 2) & 1;
                inD1  = (clockCounter >> 1) & 1;
                inSel = (clockCounter >> 0) & 1;
            }
            AddLog("Clock Pulse: Cycle " + std::to_string(clockCounter));
        }

        // --- 2. Execute Backend Circuit Modules ---
        if (currentCircuit == 0) {
            fa.execute(inA, inB, inCin);
        } else {
            mux.execute(inD0, inD1, inSel);
        }

        // --- 3. Render GUI ---
        BeginDrawing();
        ClearBackground(BG_COLOR);

        // Header Navigation Bar
        DrawRectangle(0, 0, screenWidth, 55, PANEL_COLOR);
        DrawLine(0, 55, screenWidth, 55, BORDER_COLOR);
        DrawText("CircuitLab-C++", 25, 14, 24, RAYWHITE);
        DrawText("OOPS Architecture Edition", 215, 24, 13, GRAY);

        // Header Tabs: Circuit Selection
        Rectangle tabFA  = { 470, 12, 115, 32 };
        Rectangle tabMUX = { 595, 12, 105, 32 };
        if (CheckCollisionPointRec(GetMousePosition(), tabFA) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) currentCircuit = 0;
        if (CheckCollisionPointRec(GetMousePosition(), tabMUX) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) currentCircuit = 1;
        DrawRectangleRec(tabFA, currentCircuit == 0 ? ACCENT_COLOR : PANEL_ALT);
        DrawRectangleLinesEx(tabFA, 1.5f, BORDER_COLOR);
        DrawText("Full Adder", tabFA.x + 18, tabFA.y + 8, 15, RAYWHITE);
        DrawRectangleRec(tabMUX, currentCircuit == 1 ? ACCENT_COLOR : PANEL_ALT);
        DrawRectangleLinesEx(tabMUX, 1.5f, BORDER_COLOR);
        DrawText("2:1 MUX", tabMUX.x + 22, tabMUX.y + 8, 15, RAYWHITE);

        // View Mode: Schematic vs Truth Table
        Rectangle btnView = { 720, 12, 125, 32 };
        if (CheckCollisionPointRec(GetMousePosition(), btnView) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            currentView = (currentView == 0) ? 1 : 0;
        }
        DrawRectangleRec(btnView, currentView == 1 ? PURPLE : PANEL_ALT);
        DrawRectangleLinesEx(btnView, 1.5f, BORDER_COLOR);
        DrawText(currentView == 0 ? "Truth Table" : "Schematic", btnView.x + 16, btnView.y + 8, 15, RAYWHITE);

        // Auto Clock Toggle Button
        Rectangle btnClock = { 855, 12, 105, 32 };
        if (CheckCollisionPointRec(GetMousePosition(), btnClock) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            autoClock = !autoClock;
            AddLog(autoClock ? "Auto-Clock generator ENABLED" : "Auto-Clock generator STOPPED");
        }
        DrawRectangleRec(btnClock, autoClock ? ORANGE : PANEL_ALT);
        DrawRectangleLinesEx(btnClock, 1.5f, BORDER_COLOR);
        DrawText(autoClock ? "Clock: ON" : "Clock: OFF", btnClock.x + 14, btnClock.y + 8, 14, RAYWHITE);

        // ================= MAIN VIEWPORT =================
        if (currentView == 0) {
            // SCHEMATIC VIEW
            DrawRectangle(25, 70, 630, 420, PANEL_COLOR);
            DrawRectangleLinesEx((Rectangle){ 25, 70, 630, 420 }, 1.5f, BORDER_COLOR);

            if (currentCircuit == 0) {
                DrawText("Full Adder (2x HalfAdder + OR Submodules)", 45, 88, 16, RAYWHITE);

                // Input switches
                inA   = DrawSwitch({ 45, 145, 85, 34 }, "A", inA);
                inB   = DrawSwitch({ 45, 215, 85, 34 }, "B", inB);
                inCin = DrawSwitch({ 45, 335, 85, 34 }, "Cin", inCin);

                // Logic Gate Blocks
                Rectangle ha1 = { 190, 160, 110, 75 };
                Rectangle ha2 = { 355, 220, 110, 75 };
                Rectangle orG = { 355, 350, 95, 55 };

                DrawRectangleRec(ha1, GATE_COLOR);
                DrawRectangleLinesEx(ha1, 1.5f, RAYWHITE);
                DrawText("HalfAdder 1", ha1.x + 12, ha1.y + 12, 15, RAYWHITE);
                DrawText("[XOR + AND]", ha1.x + 12, ha1.y + 35, 11, LIGHTGRAY);

                DrawRectangleRec(ha2, GATE_COLOR);
                DrawRectangleLinesEx(ha2, 1.5f, RAYWHITE);
                DrawText("HalfAdder 2", ha2.x + 12, ha2.y + 12, 15, RAYWHITE);
                DrawText("[XOR + AND]", ha2.x + 12, ha2.y + 35, 11, LIGHTGRAY);

                DrawRectangleRec(orG, { 142, 68, 173, 255 });
                DrawRectangleLinesEx(orG, 1.5f, RAYWHITE);
                DrawText("OR Gate", orG.x + 18, orG.y + 18, 15, RAYWHITE);

                // Internal wire propagation
                bool ha1_sum = inA ^ inB;
                bool ha1_carry = inA && inB;
                bool ha2_carry = ha1_sum && inCin;

                DrawCircuitWire({ 130, 162 }, { 190, 180 }, inA);
                DrawCircuitWire({ 130, 232 }, { 190, 215 }, inB);
                DrawCircuitWire({ 300, 185 }, { 355, 240 }, ha1_sum);
                DrawCircuitWire({ 130, 352 }, { 355, 275 }, inCin);
                DrawCircuitWire({ 300, 215 }, { 355, 365 }, ha1_carry);
                DrawCircuitWire({ 465, 275 }, { 355, 385 }, ha2_carry);

                // Output lines
                DrawCircuitWire({ 465, 240 }, { 590, 240 }, fa.getSum());
                DrawCircuitWire({ 450, 375 }, { 590, 375 }, fa.getCarry());

                // Output LED Nodes
                DrawCircle(605, 240, 14, fa.getSum() ? WIRE_HIGH : WIRE_LOW);
                DrawText("SUM", 588, 215, 14, RAYWHITE);

                DrawCircle(605, 375, 14, fa.getCarry() ? WIRE_HIGH : WIRE_LOW);
                DrawText("COUT", 585, 350, 14, RAYWHITE);

            } else {
                DrawText("2-to-1 Multiplexer Schematic", 45, 88, 16, RAYWHITE);

                inD0  = DrawSwitch({ 45, 155, 85, 34 }, "D0", inD0);
                inD1  = DrawSwitch({ 45, 235, 85, 34 }, "D1", inD1);
                inSel = DrawSwitch({ 45, 340, 85, 34 }, "SEL", inSel);

                Rectangle muxBox = { 245, 165, 160, 175 };
                DrawRectangleRec(muxBox, GATE_COLOR);
                DrawRectangleLinesEx(muxBox, 1.5f, RAYWHITE);
                DrawText("2:1 MUX", muxBox.x + 45, muxBox.y + 20, 18, RAYWHITE);
                DrawText("Y = (D0 & ~S) |", muxBox.x + 20, muxBox.y + 70, 14, LIGHTGRAY);
                DrawText("    (D1 & S)", muxBox.x + 20, muxBox.y + 92, 14, LIGHTGRAY);

                DrawCircuitWire({ 130, 172 }, { 245, 205 }, inD0);
                DrawCircuitWire({ 130, 252 }, { 245, 280 }, inD1);
                DrawCircuitWire({ 130, 357 }, { 325, 340 }, inSel);

                DrawCircuitWire({ 405, 250 }, { 580, 250 }, mux.getOutput());
                DrawCircle(595, 250, 15, mux.getOutput() ? WIRE_HIGH : WIRE_LOW);
                DrawText("OUTPUT (Y)", 555, 215, 14, RAYWHITE);
            }

        } else {
            // ================= TRUTH TABLE MATRIX TAB =================
            DrawRectangle(25, 70, 630, 420, PANEL_COLOR);
            DrawRectangleLinesEx((Rectangle){ 25, 70, 630, 420 }, 1.5f, BORDER_COLOR);

            if (currentCircuit == 0) {
                DrawText("Full Adder Truth Table (Synthesizer View)", 45, 88, 16, RAYWHITE);
                DrawText("Current state dynamically highlighted below:", 45, 110, 13, GRAY);

                // Table Header
                DrawRectangle(45, 135, 590, 30, PANEL_ALT);
                DrawText("A", 80, 142, 15, RAYWHITE);
                DrawText("B", 160, 142, 15, RAYWHITE);
                DrawText("Cin", 240, 142, 15, RAYWHITE);
                DrawText("|", 320, 142, 15, GRAY);
                DrawText("SUM (Expected)", 360, 142, 15, WIRE_HIGH);
                DrawText("COUT (Expected)", 490, 142, 15, WIRE_HIGH);

                for (int i = 0; i < 8; ++i) {
                    int a = (i >> 2) & 1;
                    int b = (i >> 1) & 1;
                    int c = (i >> 0) & 1;
                    int expectedSum = a ^ b ^ c;
                    int expectedCout = (a & b) | (b & c) | (a & c);

                    int rowY = 170 + (i * 35);
                    bool isCurrent = (a == inA && b == inB && c == inCin);

                    if (isCurrent) {
                        DrawRectangle(45, rowY - 2, 590, 30, HIGHLIGHT_ROW);
                        DrawRectangleLinesEx((Rectangle){ 45, (float)rowY - 2, 590, 30 }, 1.5f, WIRE_HIGH);
                    }

                    DrawText(std::to_string(a).c_str(), 82, rowY + 5, 14, isCurrent ? WIRE_HIGH : LIGHTGRAY);
                    DrawText(std::to_string(b).c_str(), 162, rowY + 5, 14, isCurrent ? WIRE_HIGH : LIGHTGRAY);
                    DrawText(std::to_string(c).c_str(), 245, rowY + 5, 14, isCurrent ? WIRE_HIGH : LIGHTGRAY);
                    DrawText("|", 320, rowY + 5, 14, GRAY);
                    DrawText(std::to_string(expectedSum).c_str(), 410, rowY + 5, 14, isCurrent ? WIRE_HIGH : RAYWHITE);
                    DrawText(std::to_string(expectedCout).c_str(), 540, rowY + 5, 14, isCurrent ? WIRE_HIGH : RAYWHITE);
                }
            } else {
                DrawText("2:1 Multiplexer Truth Table", 45, 88, 16, RAYWHITE);
                DrawText("Current state dynamically highlighted below:", 45, 110, 13, GRAY);

                DrawRectangle(45, 135, 590, 30, PANEL_ALT);
                DrawText("D0", 80, 142, 15, RAYWHITE);
                DrawText("D1", 160, 142, 15, RAYWHITE);
                DrawText("SEL", 240, 142, 15, RAYWHITE);
                DrawText("|", 320, 142, 15, GRAY);
                DrawText("OUTPUT (Y)", 430, 142, 15, WIRE_HIGH);

                for (int i = 0; i < 8; ++i) {
                    int d0 = (i >> 2) & 1;
                    int d1 = (i >> 1) & 1;
                    int sel = (i >> 0) & 1;
                    int expectedY = sel ? d1 : d0;

                    int rowY = 170 + (i * 35);
                    bool isCurrent = (d0 == inD0 && d1 == inD1 && sel == inSel);

                    if (isCurrent) {
                        DrawRectangle(45, rowY - 2, 590, 30, HIGHLIGHT_ROW);
                        DrawRectangleLinesEx((Rectangle){ 45, (float)rowY - 2, 590, 30 }, 1.5f, WIRE_HIGH);
                    }

                    DrawText(std::to_string(d0).c_str(), 85, rowY + 5, 14, isCurrent ? WIRE_HIGH : LIGHTGRAY);
                    DrawText(std::to_string(d1).c_str(), 165, rowY + 5, 14, isCurrent ? WIRE_HIGH : LIGHTGRAY);
                    DrawText(std::to_string(sel).c_str(), 248, rowY + 5, 14, isCurrent ? WIRE_HIGH : LIGHTGRAY);
                    DrawText("|", 320, rowY + 5, 14, GRAY);
                    DrawText(std::to_string(expectedY).c_str(), 465, rowY + 5, 14, isCurrent ? WIRE_HIGH : RAYWHITE);
                }
            }
        }

        // ================= RIGHT TELEMETRY INSPECTOR =================
        DrawRectangle(675, 70, 280, 420, PANEL_COLOR);
        DrawRectangleLinesEx((Rectangle){ 675, 70, 280, 420 }, 1.5f, BORDER_COLOR);
        DrawText("Signal Inspector", 695, 88, 17, RAYWHITE);
        DrawLine(695, 115, 935, 115, BORDER_COLOR);

        if (currentCircuit == 0) {
            DrawText("Circuit: 1-Bit Full Adder", 695, 130, 14, ACCENT_COLOR);
            DrawText(("Pin A:    " + std::to_string(inA)).c_str(), 695, 160, 15, inA ? WIRE_HIGH : GRAY);
            DrawText(("Pin B:    " + std::to_string(inB)).c_str(), 695, 185, 15, inB ? WIRE_HIGH : GRAY);
            DrawText(("Pin Cin:  " + std::to_string(inCin)).c_str(), 695, 210, 15, inCin ? WIRE_HIGH : GRAY);

            DrawLine(695, 240, 935, 240, BORDER_COLOR);
            DrawText("Submodule Status:", 695, 255, 14, LIGHTGRAY);
            DrawText(("HA-1 Sum:   " + std::to_string(inA ^ inB)).c_str(), 695, 280, 14, GRAY);
            DrawText(("HA-1 Carry: " + std::to_string(inA && inB)).c_str(), 695, 305, 14, GRAY);

            DrawLine(695, 335, 935, 335, BORDER_COLOR);
            DrawText("Simulation Output:", 695, 350, 14, RAYWHITE);
            DrawText(("SUM:   " + std::to_string(fa.getSum())).c_str(), 695, 380, 20, fa.getSum() ? WIRE_HIGH : GRAY);
            DrawText(("COUT:  " + std::to_string(fa.getCarry())).c_str(), 695, 415, 20, fa.getCarry() ? WIRE_HIGH : GRAY);
        } else {
            DrawText("Circuit: 2:1 Multiplexer", 695, 130, 14, ACCENT_COLOR);
            DrawText(("Line D0:  " + std::to_string(inD0)).c_str(), 695, 160, 15, inD0 ? WIRE_HIGH : GRAY);
            DrawText(("Line D1:  " + std::to_string(inD1)).c_str(), 695, 185, 15, inD1 ? WIRE_HIGH : GRAY);
            DrawText(("Select S: " + std::to_string(inSel)).c_str(), 695, 210, 15, inSel ? ACCENT_COLOR : GRAY);

            DrawLine(695, 240, 935, 240, BORDER_COLOR);
            DrawText("Active Route:", 695, 265, 14, LIGHTGRAY);
            DrawText(inSel ? "D1 -> Y [Channel 1]" : "D0 -> Y [Channel 0]", 695, 295, 15, RAYWHITE);

            DrawLine(695, 335, 935, 335, BORDER_COLOR);
            DrawText("Output:", 695, 355, 14, RAYWHITE);
            DrawText(("Y: " + std::to_string(mux.getOutput())).c_str(), 695, 395, 24, mux.getOutput() ? WIRE_HIGH : GRAY);
        }

        // ================= BOTTOM PANEL: EVENT LOGS =================
        DrawRectangle(25, 505, 930, 145, PANEL_COLOR);
        DrawRectangleLinesEx((Rectangle){ 25, 505, 930, 145 }, 1.5f, BORDER_COLOR);
        DrawText("Signal History & Propagation Event Queue", 45, 515, 14, LIGHTGRAY);
        DrawLine(45, 535, 935, 535, BORDER_COLOR);

        for (size_t i = 0; i < eventLogs.size(); ++i) {
            std::stringstream ss;
            ss << "[" << std::fixed << std::setprecision(2) << eventLogs[i].timestamp << "s] ";
            ss << eventLogs[i].message;
            DrawText(ss.str().c_str(), 45, 545 + (i * 18), 13, (i == eventLogs.size() - 1) ? WIRE_HIGH : GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}