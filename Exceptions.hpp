#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

using namespace std;

class CircuitException : public exception {
protected:
    string message;

public:
    explicit CircuitException(const string& msg) {
        message = "[CircuitException] " + msg;
    }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class ShortCircuitException : public CircuitException {
public:
    ShortCircuitException(const string& wireName,
                          const string& currentLevel,
                          const string& newLevel)
        : CircuitException("Short-Circuit on '" + wireName +
                           "': already driven " + currentLevel +
                           ", cannot drive " + newLevel +
                           " without resetting first.") {}
};

class FloatingWireException : public CircuitException {
public:
    explicit FloatingWireException(const string& wireName)
        : CircuitException("Floating Wire Error on '" + wireName +
                           "': cannot read or propagate a FLOATING signal.") {}
};

#endif
