#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

class CircuitException : public std::runtime_error {
public:
    explicit CircuitException(const std::string& msg)
        : std::runtime_error(msg) {}
};

class FloatingWireException : public CircuitException {
public:
    explicit FloatingWireException(const std::string& wireLabel)
        : CircuitException("FloatingWireException: Wire '" + wireLabel + "' is in FLOATING (-1) high-impedance state!") {}
};

class ShortCircuitException : public CircuitException {
public:
    explicit ShortCircuitException(const std::string& netLabel)
        : CircuitException("ShortCircuitException: Bus contention detected on '" + netLabel + "' (HIGH shorted directly to LOW)!") {}
};

class FileIOException : public CircuitException {
public:
    explicit FileIOException(const std::string& details)
        : CircuitException("FileIOException: " + details) {}
};

#endif // EXCEPTIONS_HPP