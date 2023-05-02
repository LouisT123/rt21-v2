#include <windows.h>
#include <iostream>

int main()
{
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    wchar_t comP[] = L"COM3";

    // Open the serial port
    hSerial = CreateFile(comP, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error opening serial port\n";
        return 1;
    }

    // Set the serial port parameters
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        std::cerr << "Error getting serial port state\n";
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = 4800; // Set the baud rate to 9600
    dcbSerialParams.ByteSize = 8; // 8 data bits
    dcbSerialParams.StopBits = ONESTOPBIT; // 1 stop bit
    dcbSerialParams.Parity = NOPARITY; // no parity
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        std::cerr << "Error setting serial port state\n";
        CloseHandle(hSerial);
        return 1;
    }

    // Set timeouts
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hSerial, &timeouts))
    {
        std::cerr << "Error setting timeouts\n";
        CloseHandle(hSerial);
        return 1;
    }

    // Read and write data
    char data[] = "AIn;";
    DWORD bytes_written, bytes_read;
    if (!WriteFile(hSerial, data, sizeof(data), &bytes_written, NULL))
    {
        std::cerr << "Error writing to serial port\n";
        CloseHandle(hSerial);
        return 1;
    }
    Sleep(500);
    char buf[140];
    if (!ReadFile(hSerial, buf, sizeof(buf), &bytes_read, NULL))
    {
        std::cerr << "Error reading from serial port\n";
        CloseHandle(hSerial);
        return 1;
    }

    std::cout << "Received data: " << buf << std::endl;

    // Close the serial port
    CloseHandle(hSerial);

    return 0;
}
