#pragma once

#include "DataReader.h"
#include "../lib/F16Data.h"
#include "../lib/miUtils.h"
#include "../lib/f16common.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <cstdint>
#include <vector>
#include <string>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

class DCSReader : public DataReader
{
public:
    DCSReader();
    ~DCSReader() override;

    bool connectToSim() override;
    void readF16Data(F16Data* data) override;

private:
    // Winsock
    SOCKET              udpSocket = INVALID_SOCKET;
    std::thread         rxThread;
    std::atomic<bool>   running{ false };

    // DCS-BIOS state buffer (16-bit address space, 0..65535)
    uint8_t             state[65536];
    std::mutex          stateMutex;

    miUtility           util;

    // Receiver-Thread: liest UDP-Stream und decodiert DCS-BIOS-Frames
    void receiverLoop();

    // Decoder für das Export-Protokoll:
    //  - findet 0x55 0x55 0x55 0x55 Sync
    //  - liest <addr><len><data...>, wendet sie auf state[] an.
    void processBytes(const uint8_t* buf, size_t len);

    // Hilfsfunktionen (analog zu DCSDataReceiver)
    uint16_t readWord(uint16_t addr) const;                // little endian
    uint16_t readGauge(uint16_t addr) const;               // 16-bit 0..65535 Gauge
    uint16_t readInt16(uint16_t addr, uint16_t mask = 0xFFFF) const;
    bool     readBool(uint16_t addr, uint16_t mask) const;
    std::string readString(uint16_t addr, uint16_t length) const;

    // Bit-Helfer (wie im BMSReader)
    inline void setDatabit(unsigned int& var, unsigned int bit)   { var |= bit; }
    inline void clearDatabit(unsigned int& var, unsigned int bit) { var &= ~bit; }
    inline void setDatabit(unsigned short& var, unsigned short bit)   { var |= bit; }
    inline void clearDatabit(unsigned short& var, unsigned short bit) { var &= ~bit; }
    inline void setDatabit(unsigned char& var, unsigned char bit)   { var |= bit; }
    inline void clearDatabit(unsigned char& var, unsigned char bit) { var &= ~bit; }

    // Caution Panel Bit Helfer (wie im BMSReader, aber ohne FlightData)
    inline void setCPBit(F16Data* data, unsigned long bit)   { data->cautionPanelLights |= bit; }
    inline void clearCPBit(F16Data* data, unsigned long bit) { data->cautionPanelLights &= ~bit; }

    // Mapping-Funktionen: state[] -> F16Data
    void mapSimAndPowerAndRelays(F16Data* data);
    void mapRightAux(F16Data* data);
    void mapCautionPanel(F16Data* data);
    void mapPFD(F16Data* data);
    void mapEngineCluster(F16Data* data);
    void mapMainInstruments(F16Data* data);
    void mapHSI(F16Data* data);
    void mapInstrumentBits(F16Data* data);
    void mapInstPanelLights(F16Data* data);
    void mapLeftConsole(F16Data* data);
    void mapUHF(F16Data* data);
    void mapIFFAux(F16Data* data);
    void mapTrim(F16Data* data);
    void mapCMDS(F16Data* data);
    void mapADI(F16Data* data);
    void mapECM(F16Data* data);
};