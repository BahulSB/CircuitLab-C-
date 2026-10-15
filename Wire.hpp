#ifndef WIRE_H
#define WIRE_H
#include <iostream>
#include <string>
#include <vector>

enum class LogicState {
    FLOATING = -1,
    LOW = 0,
    HIGH = 1
};

std::string logicStateToString(LogicState state);

class Wire {
private:
    std::string label;
    LogicState state;
    std::vector<std::string> destinations;

    std::string* historyLog;
    size_t historyCapacity;
    size_t historyCount;

    void resizeHistory();
    void recordHistory(const std::string& entry);

public:
    Wire();
    explicit Wire(const std::string& label);

    ~Wire();
    Wire(const Wire& other);
    Wire& operator=(const Wire& other);

    std::string getLabel() const;
    LogicState getState() const;
    const std::vector<std::string>& getDestinations() const;
    size_t getHistoryCount() const;

    Wire& setLabel(const std::string& label);
    Wire& addDestination(const std::string& destinationLabel);
    Wire& driveSignal(LogicState s);

    void printStatus() const;
    void printHistory() const;
};

#endif