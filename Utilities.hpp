#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

#include "Exceptions.hpp"
#include "Circuits.hpp"
#include "Gate.hpp"
#include "Wire.hpp"

// =========================================================================
// 1. FUNCTION TEMPLATE & STREAM MANIPULATORS
// =========================================================================
template <typename T>
void logComponentExecution(const T& comp, std::ostream& out = std::cout) {
    out << std::left << std::boolalpha << "[EXEC LOG] "
        << std::setw(28) << (comp.getType() + " :: " + comp.getLabel())
        << " | PrimaryOutput: " << std::setw(6) << comp.getPrimaryOutput() << "\n";
}

inline void logComponentExecution(const LogicGate& gate, std::ostream& out = std::cout) {
    out << std::left << std::boolalpha << "[EXEC LOG] "
        << std::setw(28) << "LogicGate"
        << " | Output: " << std::setw(6) << gate.getOutput() << "\n";
}

// =========================================================================
// 2. SIGNAL QUEUE (Rule of 3 Ring Buffer)
// =========================================================================
template <typename T>
class SignalQueue {
private:
    T* rawData;
    int curCapacity;
    int frontPos;
    int backPos;
    int activeCount;

    void expand() {
        int newCap = (curCapacity == 0) ? 4 : curCapacity * 2;
        T* temp = new T[newCap];
        for (int i = 0; i < activeCount; i++) {
            temp[i] = rawData[(frontPos + i) % curCapacity];
        }
        delete[] rawData;
        rawData = temp;
        frontPos = 0;
        backPos = activeCount;
        curCapacity = newCap;
    }

public:
    SignalQueue(int defaultCap = 4) {
        curCapacity = defaultCap;
        frontPos = 0;
        backPos = 0;
        activeCount = 0;
        rawData = (curCapacity > 0) ? new T[curCapacity] : nullptr;
    }

    ~SignalQueue() {
        delete[] rawData;
        rawData = nullptr;
    }

    SignalQueue(const SignalQueue& obj) {
        curCapacity = obj.curCapacity;
        activeCount = obj.activeCount;
        frontPos = 0;
        backPos = obj.activeCount;
        rawData = (curCapacity > 0) ? new T[curCapacity] : nullptr;
        for (int i = 0; i < activeCount; i++) {
            rawData[i] = obj.rawData[(obj.frontPos + i) % obj.curCapacity];
        }
    }

    SignalQueue& operator=(const SignalQueue& obj) {
        if (this == &obj) return *this;

        delete[] rawData;
        curCapacity = obj.curCapacity;
        activeCount = obj.activeCount;
        frontPos = 0;
        backPos = obj.activeCount;
        rawData = (curCapacity > 0) ? new T[curCapacity] : nullptr;
        for (int i = 0; i < activeCount; i++) {
            rawData[i] = obj.rawData[(obj.frontPos + i) % obj.curCapacity];
        }
        return *this;
    }

    void push(const T& val) {
        if (activeCount == curCapacity) {
            expand();
        }
        rawData[backPos] = val;
        backPos = (backPos + 1) % curCapacity;
        activeCount++;
    }

    bool pop(T& extracted) {
        if (activeCount == 0) return false;
        extracted = rawData[frontPos];
        frontPos = (frontPos + 1) % curCapacity;
        activeCount--;
        return true;
    }

    bool empty() const { return activeCount == 0; }
    int count() const { return activeCount; }

    void printQueue() const {
        if (activeCount == 0) {
            std::cout << "[Queue buffer is empty]\n";
            return;
        }
        for (int i = 0; i < activeCount; i++) {
            std::cout << " (" << (i + 1) << ") " << rawData[(frontPos + i) % curCapacity] << "\n";
        }
    }
};

// =========================================================================
// 3. COMPONENT REGISTRY + STL ALGORITHMS
// =========================================================================
template <typename K, typename V>
class ComponentRegistry {
private:
    std::vector<K> registeredKeys;
    std::vector<V> registeredValues;

public:
    ComponentRegistry() {}

    void insert(const K& key, const V& val) {
        for (size_t i = 0; i < registeredKeys.size(); i++) {
            if (registeredKeys[i] == key) {
                registeredValues[i] = val;
                return;
            }
        }
        registeredKeys.push_back(key);
        registeredValues.push_back(val);
    }

    bool find(const K& key, V& outVal) const {
        typename std::vector<K>::const_iterator it = std::find(registeredKeys.begin(), registeredKeys.end(), key);
        if (it != registeredKeys.end()) {
            size_t idx = static_cast<size_t>(std::distance(registeredKeys.begin(), it));
            outVal = registeredValues[idx];
            return true;
        }
        return false;
    }

    void showCatalog() const {
        std::cout << "\n----------------------------------------------------\n";
        std::cout << std::left << std::setw(18) << "ID Tag" << " | " << "Component Description\n";
        std::cout << "----------------------------------------------------\n";
        for (size_t i = 0; i < registeredKeys.size(); i++) {
            std::cout << std::left << std::setw(18) << registeredKeys[i]
                      << " | " << registeredValues[i] << "\n";
        }
        std::cout << "----------------------------------------------------\n";
    }
};

// =========================================================================
// 4. TRUTH TABLE SYNTHESIZER (Console Render + Data Capture for CSV)
// =========================================================================
struct TruthTableData {
    std::string title;
    std::vector<std::string> headers;
    std::vector<std::vector<int> > rows;
};

class TruthTableSynthesizer {
private:
    static void advanceBinary(std::vector<int>& state) {
        for (int i = static_cast<int>(state.size()) - 1; i >= 0; i--) {
            if (state[i] == 0) {
                state[i] = 1;
                return;
            }
            state[i] = 0;
        }
    }

public:
    template <typename CircuitFunc>
    static TruthTableData synthesize(const std::string& title,
                                     const std::vector<std::string>& inCols,
                                     const std::vector<std::string>& outCols,
                                     CircuitFunc evalFunc) {
        TruthTableData data;
        data.title = title;
        data.headers = inCols;
        data.headers.insert(data.headers.end(), outCols.begin(), outCols.end());

        int nIn = static_cast<int>(inCols.size());
        int totalRows = 1 << nIn;
        std::vector<int> currentPattern(nIn, 0);

        for (int step = 0; step < totalRows; step++) {
            std::vector<int> outValues = evalFunc(currentPattern);
            std::vector<int> row = currentPattern;
            row.insert(row.end(), outValues.begin(), outValues.end());
            data.rows.push_back(row);
            advanceBinary(currentPattern);
        }
        return data;
    }

    template <typename CircuitFunc>
    static void render(const std::vector<std::string>& inCols,
                       const std::vector<std::string>& outCols,
                       CircuitFunc evalFunc) {
        int nIn = static_cast<int>(inCols.size());
        int totalRows = 1 << nIn;

        std::cout << "\n";
        for (size_t i = 0; i < inCols.size(); ++i) {
            std::cout << std::setw(6) << inCols[i] << " ";
        }
        std::cout << "| ";
        for (size_t i = 0; i < outCols.size(); ++i) {
            std::cout << std::setw(6) << outCols[i] << " ";
        }
        std::cout << "\n";

        for (int i = 0; i < (nIn * 7 + static_cast<int>(outCols.size()) * 7 + 2); i++) {
            std::cout << "-";
        }
        std::cout << "\n";

        std::vector<int> currentPattern(nIn, 0);

        for (int step = 0; step < totalRows; step++) {
            std::vector<int> outValues = evalFunc(currentPattern);

            for (size_t j = 0; j < currentPattern.size(); ++j) {
                std::cout << std::setw(6) << currentPattern[j] << " ";
            }
            std::cout << "| ";
            for (size_t j = 0; j < outValues.size(); ++j) {
                std::cout << std::setw(6) << outValues[j] << " ";
            }
            std::cout << "\n";

            advanceBinary(currentPattern);
        }
        std::cout << "\n";
    }
};

// =========================================================================
// 5. FILE I/O & STREAM MANIPULATORS: CircuitFileManager
// =========================================================================
struct CircuitSnapshotRecord {
    std::string label;
    std::string type;
    bool inputA;
    bool inputB;
    bool outputState;
};

class CircuitFileManager {
public:
    static void exportTruthTableCSV(const std::string& filename, const TruthTableData& table) {
        std::ofstream outFile(filename.c_str(), std::ios::out | std::ios::trunc);
        if (!outFile.is_open()) {
            throw FileIOException("Failed to open CSV file '" + filename + "' for writing.");
        }

        outFile << "# CircuitLab-C++ v2.0 Truth Table: " << table.title << "\n";
        for (size_t i = 0; i < table.headers.size(); ++i) {
            outFile << table.headers[i] << (i + 1 < table.headers.size() ? "," : "\n");
        }

        for (size_t r = 0; r < table.rows.size(); ++r) {
            for (size_t c = 0; c < table.rows[r].size(); ++c) {
                outFile << table.rows[r][c] << (c + 1 < table.rows[r].size() ? "," : "\n");
            }
        }

        if (outFile.fail()) {
            throw FileIOException("Stream write failure while exporting '" + filename + "'.");
        }
        outFile.close();
    }

    static void saveCircuitSnapshot(const std::string& filename,
                                    const std::string& circuitTitle,
                                    const std::vector<CircuitSnapshotRecord>& records) {
        std::ofstream outFile(filename.c_str(), std::ios::out | std::ios::trunc);
        if (!outFile.is_open()) {
            throw FileIOException("Failed to open snapshot file '" + filename + "' for saving.");
        }

        outFile << std::left << std::boolalpha;
        outFile << "CIRCUIT_TITLE " << circuitTitle << "\n";
        outFile << "RECORD_COUNT  " << records.size() << "\n";
        outFile << std::setw(22) << "#LABEL"
                << std::setw(26) << "TYPE"
                << std::setw(10) << "IN_A"
                << std::setw(10) << "IN_B"
                << std::setw(10) << "OUT" << "\n";

        for (size_t i = 0; i < records.size(); ++i) {
            outFile << std::setw(22) << records[i].label
                    << std::setw(26) << records[i].type
                    << std::setw(10) << records[i].inputA
                    << std::setw(10) << records[i].inputB
                    << std::setw(10) << records[i].outputState << "\n";
        }

        if (outFile.fail()) {
            throw FileIOException("Stream write failure while saving snapshot '" + filename + "'.");
        }
        outFile.close();
    }

    static std::vector<CircuitSnapshotRecord> loadCircuitSnapshot(const std::string& filename,
                                                                  std::string& outCircuitTitle) {
        std::ifstream inFile(filename.c_str(), std::ios::in);
        if (!inFile.is_open()) {
            throw FileIOException("Failed to open snapshot file '" + filename + "' for reading.");
        }

        std::string tag;
        size_t count = 0;

        if (!(inFile >> tag >> std::ws) || tag != "CIRCUIT_TITLE") {
            throw FileIOException("Invalid snapshot format in '" + filename + "': missing CIRCUIT_TITLE.");
        }
        std::getline(inFile, outCircuitTitle);

        if (!(inFile >> tag >> count) || tag != "RECORD_COUNT") {
            throw FileIOException("Invalid snapshot format in '" + filename + "': missing RECORD_COUNT.");
        }

        std::string headerLine;
        std::getline(inFile >> std::ws, headerLine);

        std::vector<CircuitSnapshotRecord> loaded;
        inFile >> std::boolalpha;

        for (size_t i = 0; i < count; ++i) {
            CircuitSnapshotRecord rec;
            if (!(inFile >> rec.label >> rec.type >> rec.inputA >> rec.inputB >> rec.outputState)) {
                throw FileIOException("Corrupted or truncated record #" + std::to_string(i + 1) + " in '" + filename + "'.");
            }
            loaded.push_back(rec);
        }

        return loaded;
    }
};

#endif // UTILITIES_HPP