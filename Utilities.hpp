#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

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

    bool empty() const {
        return activeCount == 0;
    }

    int count() const {
        return activeCount;
    }

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
        for (size_t i = 0; i < registeredKeys.size(); i++) {
            if (registeredKeys[i] == key) {
                outVal = registeredValues[i];
                return true;
            }
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
    static void render(const std::vector<std::string>& inCols,
                       const std::vector<std::string>& outCols,
                       CircuitFunc evalFunc) {
        int nIn = static_cast<int>(inCols.size());
        int totalRows = 1;
        for (int i = 0; i < nIn; i++) {
            totalRows *= 2;
        }

        std::cout << "\n";
        for (const auto& col : inCols) {
            std::cout << std::setw(6) << col << " ";
        }
        std::cout << "| ";
        for (const auto& col : outCols) {
            std::cout << std::setw(6) << col << " ";
        }
        std::cout << "\n";

        for (int i = 0; i < (nIn * 7 + static_cast<int>(outCols.size()) * 7 + 2); i++) {
            std::cout << "-";
        }
        std::cout << "\n";

        std::vector<int> currentPattern(nIn, 0);

        for (int step = 0; step < totalRows; step++) {
            std::vector<int> outValues = evalFunc(currentPattern);

            for (int bit : currentPattern) {
                std::cout << std::setw(6) << bit << " ";
            }
            std::cout << "| ";
            for (int bit : outValues) {
                std::cout << std::setw(6) << bit << " ";
            }
            std::cout << "\n";

            advanceBinary(currentPattern);
        }
        std::cout << "\n";
    }
};

#endif