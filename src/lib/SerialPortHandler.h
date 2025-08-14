#pragma once
#include <windows.h>  
#include <iostream>  
#include <string>  
#include <thread>  
#include <atomic>  
#include <functional>  

class SerialPortHandler {
private:
    HANDLE hSerial;                // Handle für den seriellen Port  
    std::string portName;          // Name des Ports (z. B. "COM3")  
    DCB dcbSerialParams;           // Konfigurationsstruktur für den seriellen Port  
    COMMTIMEOUTS timeouts;         // Timeout-Einstellungen  
    std::atomic<bool> running;     // Flag, um den Lese-Thread zu steuern  
    std::thread readThread;        // Thread für das Lesen von Daten  

    // Callback-Funktion für empfangene Daten  
    std::function<void(const std::string&)> onDataReceived;

    // Interner Lese-Thread  
    void readLoop() {
        char buffer[256];
        DWORD bytesRead;

        while (running) {
            // Warten auf ein Ereignis am seriellen Port  
            DWORD eventMask;
            if (WaitCommEvent(hSerial, &eventMask, NULL)) {
                if (eventMask & EV_RXCHAR) { // Daten empfangen  
                    // Daten lesen  
                    if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
                        buffer[bytesRead] = '\0'; // Null-terminieren  
                        if (onDataReceived) {
                            onDataReceived(std::string(buffer));
                        }
                    }
                }
            }
        }
    }

public:
    // Konstruktor  
    SerialPortHandler(const std::string& portName, std::function<void(const std::string&)> callback)
        : portName(portName), hSerial(INVALID_HANDLE_VALUE), running(false), onDataReceived(callback) {}

    // Initialisierung des seriellen Ports  
    bool open(int baudRate = 115200) {
        hSerial = CreateFileA(portName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED, // Für asynchrone Operationen  
            NULL);

        if (hSerial == INVALID_HANDLE_VALUE) {
            std::cerr << "Error: Unable to open serial port " << portName << std::endl;
            return false;
        }

        // Konfiguration des seriellen Ports  
        dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

        if (!GetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error: Unable to get serial port state" << std::endl;
            close();
            return false;
        }

        dcbSerialParams.BaudRate = baudRate;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;

        if (!SetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error: Unable to set serial port state" << std::endl;
            close();
            return false;
        }

        // Timeout-Einstellungen  
        timeouts = { 0 };
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;

        if (!SetCommTimeouts(hSerial, &timeouts)) {
            std::cerr << "Error: Unable to set timeouts" << std::endl;
            close();
            return false;
        }

        // Ereignismaske setzen  
        if (!SetCommMask(hSerial, EV_RXCHAR)) {
            std::cerr << "Error: Unable to set comm mask" << std::endl;
            close();
            return false;
        }

        // Lese-Thread starten  
        running = true;
        readThread = std::thread(&SerialPortHandler::readLoop, this);

        return true;
    }

    // Schließen des seriellen Ports  
    void close() {
        if (running) {
            running = false;
            if (readThread.joinable()) {
                readThread.join();
            }
        }

        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial);
            hSerial = INVALID_HANDLE_VALUE;
        }
    }

    // Daten senden  
    bool write(const std::string& data) {
        DWORD bytesWritten;
        if (!WriteFile(hSerial, data.c_str(), data.size(), &bytesWritten, NULL)) {
            std::cerr << "Error: Unable to write to serial port" << std::endl;
            return false;
        }
        return true;
    }

    // Destruktor  
    ~SerialPortHandler() {
        close();
    }
};

/*
class MySerialClass {
private:
    SerialPortHandler serialPort;

    // Memberfunktion für empfangene Daten  
    void onSerialDataReceived(const std::string& data) {
        std::cout << "Data received in MySerialClass: " << data << std::endl;
    }

public:
    // Konstruktor  
    MySerialClass(const std::string& portName)
        : serialPort(portName, [this](const std::string& data) { this->onSerialDataReceived(data); }) {}

    // Initialisierung des seriellen Ports  
    bool initialize(int baudRate = 9600) {
        return serialPort.open(baudRate);
    }

    // Daten senden  
    void sendData(const std::string& data) {
        serialPort.write(data);
    }

    // Schließen des seriellen Ports  
    void shutdown() {
        serialPort.close();
    }
};

int main() {
    // Instanz der Klasse erstellen  
    MySerialClass mySerial("COM3");

    // Seriellen Port initialisieren  
    if (!mySerial.initialize(9600)) {
        return 1;
    }

    // Daten senden  
    mySerial.sendData("Hello, Serial Port!\n");

    // Hauptprogramm läuft weiter  
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    // Seriellen Port schließen  
    mySerial.shutdown();

    return 0;
}
*/