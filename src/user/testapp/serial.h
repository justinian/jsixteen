#pragma once
/// \file serial.h
/// Declarations related to serial ports.
#include <stdint.h>

#define serial_port nulldrv_serial_port

class serial_port
{
public:
    /// Constructor.
    /// \arg port   The IO address of the serial port
    serial_port(uint16_t port);

    serial_port();

    void write(char c);
    char read();

private:
    uint16_t m_port;

    bool read_ready();
    bool write_ready();
};

