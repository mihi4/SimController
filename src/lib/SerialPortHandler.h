#ifndef SERIALPORTHANDLER_H  
#define SERIALPORTHANDLER_H  

#include <string>  
#include <functional>  
#include <atomic>  
#include <thread>  
#include <windows.h>  
#include <iostream>  
#include <condition_variable>


// Hilfsfunktion zur Konvertierung von std::string zu std::wstring  
inline std::wstring stringToWString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

class SerialPortHandler {
private:
    HANDLE hSerial;                // Handle für den seriellen Port  
    std::string portName;          // Name des Ports (z. B. "COM3")  
    std::function<void(const std::string&)> onDataReceived; // Callback-Funktion  
    std::atomic<bool> running;     // Flag, um den Lese-Thread zu steuern  
    std::condition_variable_any cv;
    std::mutex mtx;
    OVERLAPPED overlapped;

    // Interner Lese-Thread  
    void readLoop() {
        std::cout << "Read loop started." << std::endl;
        char buffer[256];
        DWORD bytesRead;

        while (running) { // Schleife läuft, solange "running" true ist  
            DWORD eventMask = 0;

            // Warten auf ein Ereignis  
            if (!WaitCommEvent(hSerial, &eventMask, &overlapped)) {
                DWORD error = GetLastError();
                if (error == ERROR_IO_PENDING) {
                    // Warten, bis die Operation abgeschlossen ist  
                    WaitForSingleObject(overlapped.hEvent, INFINITE);
                }
                else if (!running && error == ERROR_OPERATION_ABORTED) {
                    // Operation wurde durch CancelIoEx abgebrochen  
                    break;
                }
                else {
                    std::cerr << "WaitCommEvent failed. Error code: " << error << std::endl;
                    continue;
                }
            }

            // Wenn ein Zeichen empfangen wurde  
            if (eventMask & EV_RXCHAR) {
                if (!ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, &overlapped)) {
                    DWORD error = GetLastError();
                    if (error == ERROR_IO_PENDING) {
                        // Warten, bis die Leseoperation abgeschlossen ist  
                        if (!GetOverlappedResult(hSerial, &overlapped, &bytesRead, TRUE)) {
                            DWORD overlappedError = GetLastError();
                            if (!running && overlappedError == ERROR_OPERATION_ABORTED) {
                                // Operation wurde durch CancelIoEx abgebrochen  
                                break;
                            }
                            std::cerr << "GetOverlappedResult failed. Error code: " << overlappedError << std::endl;
                            continue;
                        }
                    }
                    else if (!running && error == ERROR_OPERATION_ABORTED) {
                        // Operation wurde durch CancelIoEx abgebrochen  
                        break;
                    }
                    else {
                        std::cerr << "ReadFile failed. Error code: " << error << std::endl;
                        continue;
                    }
                }

                // Null-terminieren und Callback aufrufen  
                buffer[bytesRead] = '\0';
                if (onDataReceived) {
                    onDataReceived(std::string(buffer));
                }
            }
        }

        // Benachrichtige, dass der Lese-Thread beendet wurde  
        std::lock_guard<std::mutex> lock(mtx);
        running = false;
        cv.notify_all();

        std::cout << "Read loop stopped." << std::endl;
    }

public:
    // Konstruktor  
    SerialPortHandler(const std::string& portName, std::function<void(const std::string&)> callback)
        : portName(portName), hSerial(INVALID_HANDLE_VALUE), onDataReceived(callback), running(false) {
        ZeroMemory(&overlapped, sizeof(overlapped));
        overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    }

    // Destruktor  
    ~SerialPortHandler() {
        close(); // Stelle sicher, dass der Port geschlossen wird  
        if (overlapped.hEvent) {
            CloseHandle(overlapped.hEvent);
        }
    }

    std::string getPortname() { return portName; }

    // Öffnen des seriellen Ports  
    bool open(int baudRate) {
        std::cout << "calling CreateFile\n";
        hSerial = CreateFile(
            stringToWString(portName).c_str(), // Konvertiere std::string zu LPCWSTR  
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED, // Kein FILE_FLAG_OVERLAPPED  
            NULL
        );

        if (hSerial == INVALID_HANDLE_VALUE) {
            std::cerr << "Error: Unable to open serial port " << portName << std::endl;
            return false;
        }

        // Konfiguration des seriellen Ports  
        DCB dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        
        std::cout << "calling getCommState\n";
        if (!GetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error: Unable to get serial port state" << std::endl;
            close();
            return false;
        }

        dcbSerialParams.BaudRate = baudRate;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;

        std::cout << "calling SetCommstate\n";
        if (!SetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error: Unable to set serial port state" << std::endl;
            close();
            return false;
        }

        // Ereignismaske setzen  
        std::cout << "calling setComMask\n";
        if (!SetCommMask(hSerial, EV_RXCHAR)) {
            std::cerr << "Error: Unable to set comm mask" << std::endl;
            close();
            return false;
        }

        // Timeouts konfigurieren  
        COMMTIMEOUTS timeouts = { 0 };
        timeouts.ReadIntervalTimeout = 50;          // Maximale Zeit zwischen zwei Zeichen (in ms)  
        timeouts.ReadTotalTimeoutConstant = 50;     // Gesamte Lesezeit (in ms)  
        timeouts.ReadTotalTimeoutMultiplier = 10;   // Multiplikator für die Anzahl der zu lesenden Zeichen  
        timeouts.WriteTotalTimeoutConstant = 50;    // Gesamte Schreibzeit (in ms)  
        timeouts.WriteTotalTimeoutMultiplier = 10;  // Multiplikator für die Anzahl der zu schreibenden Zeichen  
        std::cout << "calling timeouts";
        if (!SetCommTimeouts(hSerial, &timeouts)) {
            std::cerr << "Error: Unable to set timeouts" << std::endl;
            close();
            return false;
        }

        std::cout << "starting thread\n";
        // Lese-Thread starten  
        running = true; // Setze das Flag auf "true"  
        std::thread(&SerialPortHandler::readLoop, this).detach(); // Thread im Hintergrund starten  
        std::cout << "Read thread started successfully." << std::endl;

        return true;
    }

    // Schließen des seriellen Ports  
    void close() {
        if (running) {
            running = false; // Setze das Flag auf "false", um den Lese-Thread zu stoppen  


            if (hSerial != INVALID_HANDLE_VALUE) {
                CancelIoEx(hSerial, &overlapped);
            }

            cv.notify_all(); // Benachrichtige den Lese-Thread, damit er die Schleife verlässt  

              // Warte, bis der Lese-Thread vollständig beendet ist  
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]() { return !running; });

        }

        // Warte, bis der Lese-Thread vollständig beendet ist  
/*        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return !running; }); */

        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial);
            hSerial = INVALID_HANDLE_VALUE;
            std::cout << "Serial port closed." << std::endl;
        }
    }

    // Daten senden  
    bool write(const std::string& data) {
        if (hSerial == INVALID_HANDLE_VALUE) {
            std::cerr << "Error: Serial port is not open." << std::endl;
            return false;
        }

        DWORD bytesWritten;
        if (!WriteFile(hSerial, data.c_str(), data.size(), &bytesWritten, &overlapped)) {
            DWORD error = GetLastError();
            std::cerr << "Error writing to serial port. Error code: " << error << std::endl;
            return false;
        }

        WaitForSingleObject(overlapped.hEvent, INFINITE);
        /*if (bytesWritten != data.size()) {
            std::cerr << "Warning: Not all data was written to the serial port." << std::endl;
        }*/

        return true;
    }
};

#endif // SERIALPORTHANDLER_H  