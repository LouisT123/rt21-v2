#include <iostream>
#include <Windows.h>
#include <string>

int main()
{
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    wchar_t comport[] = L"COM3"; //change this com port to whatever 

    // Open the serial port
    hSerial = CreateFile(comport, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        std::cout << "Error opening serial port." << std::endl;
        return 1;
    }

    // Set serial port parameters
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        std::cout << "Error getting serial port state." << std::endl;
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_4800;  // Change baud rate if necessary, but shoudl be 4800 8N1 for rt21 dcu-1 protocol
    dcbSerialParams.ByteSize = 8;         // Number of bits/byte, 8
    dcbSerialParams.Parity = NOPARITY;    // No parity
    dcbSerialParams.StopBits = ONESTOPBIT;// One stop bit

    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        std::cout << "Error setting serial port state." << std::endl;
        CloseHandle(hSerial);
        return 1;
    }

    // Set timeouts
    timeouts.ReadIntervalTimeout = 50;           // Maximum time between read operations
    timeouts.ReadTotalTimeoutConstant = 50;      // Constant timeout for reading
    timeouts.ReadTotalTimeoutMultiplier = 10;    // Multiplier timeout for reading
    timeouts.WriteTotalTimeoutConstant = 50;     // Constant timeout for writing
    timeouts.WriteTotalTimeoutMultiplier = 10;   // Multiplier timeout for writing

    if (!SetCommTimeouts(hSerial, &timeouts))
    {
        std::cout << "Error setting timeouts." << std::endl;
        CloseHandle(hSerial);
        return 1;
    }

    // Loop until the user enters "stop"
    std::string userInput;
    while (userInput != "stop")
    {
        // Ask for user input
        std::cout << "Enter text (or 'stop' to exit): ";
        std::getline(std::cin, userInput);

        // Write data to the serial port
        DWORD bytesWritten;
        if (!WriteFile(hSerial, userInput.c_str(), userInput.length(), &bytesWritten, NULL))
        {
            std::cout << "Error writing to serial port." << std::endl;
            CloseHandle(hSerial);
            return 1;
        }

        std::cout << "Bytes written: " << bytesWritten << std::endl;

        Sleep(500);

        // Read data from the serial port
        char buffer[100];
        DWORD bytesRead;
        if (!ReadFile(hSerial, buffer, sizeof(buffer), &bytesRead, NULL))
        {
            std::cout << "Error reading from serial port." << std::endl;
            CloseHandle(hSerial);
            return 1;
        }

        std::cout << "Bytes read: " << bytesRead << std::endl;
        std::cout << "Data received: " << buffer << std::endl;
    }

    // Close the serial port
    CloseHandle(hSerial);

    return 0;
}
