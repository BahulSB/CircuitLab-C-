#include "Wire.hpp"
using namespace std;

string logicStateToString(LogicState state) {
    switch (state) {
        case LogicState::LOW:
            return "LOW (0)";
        case LogicState::HIGH:
            return "HIGH (1)";
        case LogicState::FLOATING:
            return "FLOATING (-1)";
        default:
            return "UNKNOWN";
    }
}

void Wire::resizeHistory() {
    size_t newCapacity = (historyCapacity == 0) ? 2 : historyCapacity * 2;
    string* newHistory = new string[newCapacity];

    for (size_t i = 0; i < historyCount; i++) {
        newHistory[i] = historyLog[i];
    }

    delete[] historyLog;

    historyLog = newHistory;
    historyCapacity = newCapacity;
}

void Wire::recordHistory(const string& entry) {
    if (historyCount >= historyCapacity) {
        resizeHistory();
    }

    historyLog[historyCount] = entry;
    historyCount++;
}

Wire::Wire()
    : label("UNNAMED"),
      state(LogicState::FLOATING),
      historyLog(nullptr),
      historyCapacity(2),
      historyCount(0) {

    historyLog = new string[historyCapacity];
    recordHistory("Wire created: UNNAMED, State: FLOATING");
}

Wire::Wire(const string& label)
    : label(label),
      state(LogicState::FLOATING),
      historyLog(nullptr),
      historyCapacity(2),
      historyCount(0) {

    historyLog = new string[historyCapacity];
    recordHistory("Wire created: " + this->label +
                  ", State: FLOATING");
}

Wire::~Wire() {
    cout << "[Destructor] Cleaning up wire: " << label
         << " (" << historyCount << " history entries)"
         << endl;

    delete[] historyLog;
    historyLog = nullptr;
}

Wire::Wire(const Wire& other)
    : label(other.label),
      state(other.state),
      destinations(other.destinations),
      historyLog(nullptr),
      historyCapacity(other.historyCapacity),
      historyCount(other.historyCount) {

    historyLog = new string[historyCapacity];

    for (size_t i = 0; i < historyCount; i++) {
        historyLog[i] = other.historyLog[i];
    }

    recordHistory("Copied from wire: " + other.label);
}

Wire& Wire::operator=(const Wire& other) {
    if (this != &other) {
        string* newHistory = new string[other.historyCapacity];

        for (size_t i = 0; i < other.historyCount; i++) {
            newHistory[i] = other.historyLog[i];
        }

        delete[] historyLog;

        label = other.label;
        state = other.state;
        destinations = other.destinations;
        historyLog = newHistory;
        historyCapacity = other.historyCapacity;
        historyCount = other.historyCount;

        recordHistory("Assigned from wire: " + other.label);
    }

    return *this;
}

string Wire::getLabel() const {
    return label;
}

LogicState Wire::getState() const {
    return state;
}

const vector<string>& Wire::getDestinations() const {
    return destinations;
}

size_t Wire::getHistoryCount() const {
    return historyCount;
}

Wire& Wire::setLabel(const string& label) {
    recordHistory("Label changed from '" + this->label +
                  "' to '" + label + "'");

    this->label = label;
    return *this;
}

Wire& Wire::addDestination(const string& destinationLabel) {
    destinations.push_back(destinationLabel);
    recordHistory("Connected destination: " + destinationLabel);

    return *this;
}

Wire& Wire::driveSignal(LogicState s) {
    state = s;
    recordHistory("Signal changed to: " + logicStateToString(s));

    return *this;
}

void Wire::printStatus() const {
    cout << "--- Wire Status: " << label << " ---" << endl;
    cout << "Current State : " << logicStateToString(state) << endl;
    cout << "Destinations  : ";

    if (destinations.empty()) {
        cout << "None";
    } else {
        for (size_t i = 0; i < destinations.size(); i++) {
            cout << destinations[i];

            if (i + 1 < destinations.size()) {
                cout << ", ";
            }
        }
    }

    cout << endl;
}

void Wire::printHistory() const {
    cout << "--- History for Wire: " << label << " ---" << endl;

    for (size_t i = 0; i < historyCount; i++) {
        cout << "[" << i + 1 << "] "
             << historyLog[i] << endl;
    }
}
#ifdef STANDALONE_TEST
int main() {
    cout << "\n";
    cout << "          CircuitLab - Wire Test        " << endl;
    cout << "\n";

    {
        Wire wire1("W_CLK");

        wire1.addDestination("GATE_U1:PIN_A")
             .addDestination("GATE_U2:PIN_CLK")
             .driveSignal(LogicState::HIGH);

        wire1.printStatus();
        cout << endl;

        wire1.setLabel("W_SYS_CLK")
             .driveSignal(LogicState::LOW)
             .driveSignal(LogicState::HIGH);

        wire1.printStatus();
        cout << endl;

        wire1.printHistory();
        cout << endl;

        cout << "Testing Copy Constructor:" << endl;

        Wire wire2 = wire1;

        wire2.setLabel("W_SYS_CLK_BACKUP")
             .driveSignal(LogicState::FLOATING);

        cout << "Original Wire State: "
             << logicStateToString(wire1.getState()) << endl;

        cout << "Copied Wire State  : "
             << logicStateToString(wire2.getState()) << endl;

        cout << endl;
        cout << "Leaving scope and destroying both wires..." << endl;
    }

    cout << "\n";
    cout << "           All Tests Completed          " << endl;
    cout << "\n";

    return 0;
}
#endif