#include "DCSReader.h"
#include <iostream>
#include <cstring>
#include <cmath>

DCSReader::DCSReader()
{
    memset(state, 0, sizeof(state));
    util.createGaugeMappings();
}

DCSReader::~DCSReader()
{
    running = false;
    if (rxThread.joinable())
        rxThread.join();

    if (udpSocket != INVALID_SOCKET) {
        closesocket(udpSocket);
        WSACleanup();
        udpSocket = INVALID_SOCKET;
    }
}

bool DCSReader::connectToSim()
{
    if (udpSocket != INVALID_SOCKET) return true;

    WSADATA wsaData{};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        std::cerr << "socket() failed\n";
        WSACleanup();
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(5010); // derselbe Standardport wie in DCSDataReceiver

    if (bind(udpSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "bind() failed\n";
        closesocket(udpSocket);
        WSACleanup();
        udpSocket = INVALID_SOCKET;
        return false;
    }

    running = true;
    rxThread = std::thread(&DCSReader::receiverLoop, this);

    return true;
}

void DCSReader::receiverLoop()
{
    uint8_t buf[2048];

    while (running) {
        sockaddr_in src{};
        int srclen = sizeof(src);
        int len = recvfrom(udpSocket,
                           reinterpret_cast<char*>(buf),
                           sizeof(buf), 0,
                           reinterpret_cast<sockaddr*>(&src), &srclen);
        if (len <= 0) {
            // kleine Pause
            Sleep(1);
            continue;
        }
        processBytes(buf, static_cast<size_t>(len));
    }
}

// -------------------------
// DCS-BIOS Export Decoder
// -------------------------

void DCSReader::processBytes(const uint8_t* buf, size_t len)
{
    static std::vector<uint8_t> rxBuf;
    rxBuf.insert(rxBuf.end(), buf, buf + len);

    while (true) {
        // Sync suchen
        size_t pos = 0;
        while (pos + 3 < rxBuf.size()) {
            if (rxBuf[pos] == 0x55 && rxBuf[pos + 1] == 0x55 &&
                rxBuf[pos + 2] == 0x55 && rxBuf[pos + 3] == 0x55) {
                break;
            }
            ++pos;
        }

        if (pos + 3 >= rxBuf.size()) {
            // kein vollständiger Sync gefunden -> alles vor pos verwerfen
            if (pos > 0) rxBuf.erase(rxBuf.begin(), rxBuf.begin() + pos);
            return;
        }

        // Sync an den Anfang schieben
        if (pos > 0) {
            rxBuf.erase(rxBuf.begin(), rxBuf.begin() + pos);
        }

        // Wir haben 0x55 0x55 0x55 0x55 am Anfang
        if (rxBuf.size() < 4 + 2 + 2) {
            // noch keine addr/len
            return;
        }

        size_t idx = 4;
        uint16_t addr = rxBuf[idx] | (uint16_t(rxBuf[idx + 1]) << 8); idx += 2;
        uint16_t length = rxBuf[idx] | (uint16_t(rxBuf[idx + 1]) << 8); idx += 2;

        if (rxBuf.size() < idx + length) {
            // noch nicht alle Daten für dieses Frame vorhanden
            return;
        }

        {
            std::lock_guard<std::mutex> lock(stateMutex);
            for (uint16_t i = 0; i < length; ++i) {
                uint16_t a = addr + i;
                if (a < 65536)
                    state[a] = rxBuf[idx + i];
            }
        }

        rxBuf.erase(rxBuf.begin(), rxBuf.begin() + idx + length);

        if (rxBuf.size() < 4) return;
    }
}

// -------------------------
// Hilfsfunktionen (Memory)
// -------------------------

uint16_t DCSReader::readWord(uint16_t addr) const
{
    if (addr >= 65535) return 0;
    uint16_t lo = state[addr];
    uint16_t hi = state[addr + 1];
    return static_cast<uint16_t>((hi << 8) | lo);
}

uint16_t DCSReader::readGauge(uint16_t addr) const
{
    // identisch zur C#-Funktion ReadMemoryGauge
    int tmp1 = 256 * state[addr + 1];
    int tmp2 = state[addr];
    return static_cast<uint16_t>(tmp1 + tmp2);
}

uint16_t DCSReader::readInt16(uint16_t addr, uint16_t mask) const
{
    uint16_t extract = readWord(addr);
    return static_cast<uint16_t>(extract & mask);
}

bool DCSReader::readBool(uint16_t addr, uint16_t mask) const
{
    uint16_t extract = readWord(addr);
    return (extract & mask) != 0;
}

std::string DCSReader::readString(uint16_t addr, uint16_t length) const
{
    if (addr + length > 65535) {
        if (addr >= 65535) return "";
        length = static_cast<uint16_t>(65535 - addr);
    }
    std::string s;
    s.reserve(length);
    for (uint16_t i = 0; i < length; ++i) {
        char c = static_cast<char>(state[addr + i]);
        if (c == '\0') break;
        s.push_back(c);
    }
    return s;
}

// -------------------------
// Mapping in F16Data
// -------------------------

void DCSReader::readF16Data(F16Data* data)
{
    if (!data) return;

    std::lock_guard<std::mutex> lock(stateMutex);

    // Wichtig: Keine Schreibzugriffe auf state[] hier

    mapSimAndPowerAndRelays(data);
    mapRightAux(data);
    mapCautionPanel(data);
    mapPFD(data);
    mapEngineCluster(data);
    mapMainInstruments(data);
    mapHSI(data);
    mapInstrumentBits(data);
    mapInstPanelLights(data);
    mapLeftConsole(data);
    mapUHF(data);
    mapIFFAux(data);
    mapTrim(data);
    mapCMDS(data);
    mapADI(data);
    mapECM(data);

    data->simVersion = "DCS F-16C (DCS-BIOS)";
}

// ------------------------------------------------------
// 1) Sim / Power / Relay Bits
//    => hier gibt es in DCSDataReceiver kein 1:1 Mapping
//    auf BMS-PowerBits etc. Daher: erstmal leer lassen.
// ------------------------------------------------------

void DCSReader::mapSimAndPowerAndRelays(F16Data* data)
{
    // Sicher ableitbare Dinge (OnGround) könnten hier später ergänzt werden.
    // Aus DCSDataReceiver.Enviroment.OnGround():
    //   F16C: ReadMemoryBool(0x4562, 2, 0x4000/0x2000) -> WOW
    // Da diese Bits aber in F16Data nicht weiter ausgewertet werden,
    // lassen wir simStates/powerStates/relayStates vorerst 0.
    data->simStates   = 0;
    data->powerStates = 0;
    data->relayStates = 0;
}

// ------------------------------------------------------
// 2) Right AUX (Fuel, HYD, Cabin, EPU)
//    => aus DCSDataReceiver.Fuel, Engines, Gauges
// ------------------------------------------------------

void DCSReader::mapRightAux(F16Data* data)
{
    // Fuel FWD/AFT: DCSDataReceiver.Fuel.Fuel_Qty_Left/Right()
    // Fuel_Qty_Left_g:  addr 0x44EA Gauge -> 0..65535, Left
    // Fuel_Qty_Right_g: addr 0x44EC Gauge -> 0..65535, Right
    // In C#: Fuel_Qty_Left() = floor(Left_g / 6553.0)  => 0..10 (x100 lbs)
    // F16Data: fuelFWD/fuelAFT erwartet 0..4200 lbs (BMS-Einheit).
    // Wir skalieren wie in BMSReader: BMS nutzt FlightData.fwd/aft (lbs),
    // dort wird durch 10 geteilt für Anzeige.
    // Hier: wir nehmen DCS-Logik 0..10, interpretieren als 0..10000 lbs
    // und schneiden auf 4200, damit Skala passt.

    uint16_t fuelLeft_g  = readGauge(0x44EA);
    uint16_t fuelRight_g = readGauge(0x44EC);

    int fuelLeftSteps  = static_cast<int>(std::floor(fuelLeft_g / 6553.0));   // 0..10
    int fuelRightSteps = static_cast<int>(std::floor(fuelRight_g / 6553.0));  // 0..10

    // 1 Schritt ~ 1000 lbs. BMS nimmt max 4200/4200; wir kappen bei 4200.
    int fuelLeftLbs  = fuelLeftSteps * 1000;
    int fuelRightLbs = fuelRightSteps * 1000;

    if (fuelLeftLbs > 4200)  fuelLeftLbs  = 4200;
    if (fuelRightLbs > 4200) fuelRightLbs = 4200;

    data->fuelFWD = static_cast<unsigned short>(fuelLeftLbs);
    data->fuelAFT = static_cast<unsigned short>(fuelRightLbs);

    // Fuel Totalizer:
    // DCSDataReceiver.Fuel.Fuel_Qty_Total():
    //  tt = ReadInt16(0x44EE), t = ReadInt16(0x44F0), h = ReadInt16(0x44F2)
    //  total = tt*10000 + t*1000 + h*100
    uint16_t tt = readInt16(0x44EE);
    uint16_t t  = readInt16(0x44F0);
    uint16_t h  = readInt16(0x44F2);
    int total = tt * 10000 + t * 1000 + h * 100;
    data->fuelTotal = static_cast<unsigned char>(total / 100); // wie in F16Data-Kommentar

    // HYD Pressure:
    // Engines.HydraulicPressure_Left_g():  0x44D6 Gauge
    // Engines.HydraulicPressure_Right_g(): 0x44D8 Gauge
    // Diese Gauges sind 0..65535, skalieren wir 0..4000 PSI wie in BMSReader,
    // BMSReader nimmt flightData2->hydPressureA/B direkt (0..4000).
    uint16_t hydA_g = readGauge(0x44D6);
    uint16_t hydB_g = readGauge(0x44D8);
    data->hydA = static_cast<unsigned short>(std::round(hydA_g / 65535.0 * 4000.0));
    data->hydB = static_cast<unsigned short>(std::round(hydB_g / 65535.0 * 4000.0));

    // EPU Fuel / Hydrazin Volume:
    // Engines.JFS_Hydrazine_Qty_g(): Gauge(0x44E2), F-16C_50.lua: HYDRAZIN_VOLUME
    uint16_t hydrazin_g = readGauge(0x44E2);
    // F16Data.epuFuel: 0..65535, raw pointer 0..100% * 100 => übernehmen direkt
    data->epuFuel = hydrazin_g;

    // Cabin Pressure Alt:
    // Gauges.CabinPressureGauge_g(): Gauge(0x44F4)  (F-16C_50.lua: COCKPIT_ALITITUDE)
    uint16_t cabin_g = readGauge(0x44F4);
    // DCSDataReceiver.CabinPressure(): floor(g/6553) 0..10 (0..50000 ft / 5000)
    int cabinIndex = static_cast<int>(std::floor(cabin_g / 6553.0));
    int cabinFt = cabinIndex * 5000;
    // F16Data.cabinPress: 0..65535, raw 0..50000 -> lineare Skalierung
    data->cabinPress = static_cast<unsigned short>(
        util.map(cabinFt, 0, 50000, 0, 65535)
    );
}

// ------------------------------------------------------
// 3) Caution Panel
//    Mapping aus CautionIndexerLights.* (DCSDataReceiver)
//    und deinem Hinweis CPOXYLOW ⇔ CAUTION_OBOGS,
//    CPLINE1..6 ⇔ LIGHT_CAUTION_1..6.
// ------------------------------------------------------

void DCSReader::mapCautionPanel(F16Data* data)
{
    data->cautionPanelLights = 0;

    // Alle CAUTION_xxx aus DCSDataReceiver.CautionIndexerLights

    // Grundcautions (gelbe Einzellampen)
    if (readBool(0x4476, 0x0001)) setCPBit(data, CPFLCS);      // CAUTION_FLCS_FAULT
    if (readBool(0x4476, 0x0002)) setCPBit(data, CPENGINE);    // CAUTION_EngineFault
    if (readBool(0x4476, 0x0010)) setCPBit(data, CPELECSYS);   // CAUTION_ElecSys
    if (readBool(0x4476, 0x0020)) setCPBit(data, CPSEC);       // CAUTION_SEC
    if (readBool(0x4476, 0x0040)) setCPBit(data, CPSTBYGEN);   // CAUTION_STBY_GEN – kein eigenes Symbol, BMS nutzt CPNUCLEAR/CPEEC als Extra; hier STBY auf CPLINE?
    if (readBool(0x4476, 0x0080)) setCPBit(data, CPNWS);       // CAUTION_NWSFail
    if (readBool(0x4476, 0x0100)) setCPBit(data, CPPROBEHEAT); // CAUTION_ProbeHeat
    if (readBool(0x4476, 0x0200)) setCPBit(data, CPFUELOIL);   // CAUTION_Fuel_Oil_Hot
    if (readBool(0x4476, 0x0400)) setCPBit(data, CPRADARALT);  // CAUTION_RadarAlt
    if (readBool(0x4476, 0x0800)) setCPBit(data, CPINLET);     // CAUTION_Inlet_Icing
    if (readBool(0x4476, 0x1000)) setCPBit(data, CPCADC);      // CAUTION_CADC
    if (readBool(0x4476, 0x2000)) setCPBit(data, CPINLET);     // Inlet_Icing (bereits auf CPINLET, doppelt)
    if (readBool(0x4476, 0x4000)) setCPBit(data, CPIFF);       // CAUTION_IFF
    // Seat Not Armed
    if (readBool(0x4476, 0x0008)) setCPBit(data, CPSEAT);      // CAUTION_SeatNotArmed

    // Weitere Cautions aus 0x4478
    if (readBool(0x4478, 0x0001)) setCPBit(data, CPSTORESCFG); // CAUTION_StoresConfig
    if (readBool(0x4478, 0x0002)) setCPBit(data, CPOVERHEAT);  // CAUTION_Overheat
    if (readBool(0x4478, 0x0004)) setCPBit(data, CPNUCLEAR);   // CAUTION_Nuclear
    if (readBool(0x4478, 0x0008)) setCPBit(data, CPOXYLOW);    // CAUTION_OBOGS -> CPOXYLOW gemäß deinem Hinweis
    if (readBool(0x4478, 0x0010)) setCPBit(data, CPATF);       // CAUTION_ATFMode_NOT_ENGAGED
    if (readBool(0x4478, 0x0020)) setCPBit(data, CPEEC);       // CAUTION_EEC
    if (readBool(0x4478, 0x0040)) setCPBit(data, CPLINE1);     // CAUTION_Unused1 (=> für Light-Test; BMS nutzt das für "alle an")
    if (readBool(0x4478, 0x0080)) setCPBit(data, CPCABIN);     // CAUTION_CabinPressure
    if (readBool(0x4478, 0x0100)) setCPBit(data, CPFWDFUEL);   // CAUTION_FUEL_LO_FWD
    if (readBool(0x4478, 0x0200)) setCPBit(data, CPBUC);       // CAUTION_BUC
    if (readBool(0x4478, 0x0400)) setCPBit(data, CPLINE2);     // CAUTION_Unused2
    if (readBool(0x4478, 0x0800)) setCPBit(data, CPLINE3);     // CAUTION_Unused3
    if (readBool(0x4478, 0x1000)) setCPBit(data, CPAFTFUEL);   // CAUTION_FUEL_LO_AFT
    if (readBool(0x4478, 0x2000)) setCPBit(data, CPLINE4);     // CAUTION_Unused4
    if (readBool(0x4478, 0x4000)) setCPBit(data, CPLINE5);     // CAUTION_Unused5
    if (readBool(0x4478, 0x8000)) setCPBit(data, CPLINE6);     // CAUTION_Unused6

    // Hook Caution
    if (readBool(0x4476, 0x8000)) setCPBit(data, CPHOOK);      // CAUTION_Hook

    // Fuel Low BMS-Mapping: du verwendest im BMSReader CPFWDFUEL/CPAFTFUEL; das ist bereits oben umgesetzt.

    // ProbeHeat + Elec_Fault Blink-Logik:
    // In DCSDataReceiver CautionIndexerLights wird für Blink noch FlightData2.BlinkBits benutzt,
    // die wir hier nicht haben. Daher setzen wir sie einfach statisch.
    // Elec Fault:
    if (readBool(0x4476, 0x0010)) setCPBit(data, CPELECSYS);   // auf ElecSys abgebildet
}

// ------------------------------------------------------
// 4) PFD (PFL) – benutzen DED/PFL aus DCSDataReceiver
//    DCSDataReceiver.DED_PFL.DED_Line*()
//    Adressen: 0x450A, 0x4528, 0x4546, 0x4564, 0x4582
//    transform_DED(): a->$, %-><, o->?  – übernehmen wir 1:1
// ------------------------------------------------------

void DCSReader::mapPFD(F16Data* data)
{
    auto transform_DED = [](std::string s) {
        for (char& c : s) {
            if (c == 'a') c = '$';
            else if (c == '%') c = '<';
            else if (c == 'o') c = '?';
        }
        return s;
    };

    data->pfdLine1 = transform_DED(readString(0x450A, 24));
    data->pfdLine2 = transform_DED(readString(0x4528, 24));
    data->pfdLine3 = transform_DED(readString(0x4546, 24));
    data->pfdLine4 = transform_DED(readString(0x4564, 24));
    data->pfdLine5 = transform_DED(readString(0x4582, 24));
}

// ------------------------------------------------------
// 5) Engine Cluster (Oil, Nozzle, RPM, FTIT, FuelFlow)
//    aus Engines.* und Fuel.* in DCSDataReceiver
// ------------------------------------------------------

void DCSReader::mapEngineCluster(F16Data* data)
{
    // Oil Pressure:
    // Engines.ENGINE_OilPressure_L_g(): Gauge(0x44CC)
    uint16_t oil_g = readGauge(0x44CC);
    // DCSDataReceiver.ENGINE_OilPressure_L(): floor(g/6553) -> 0..10 (0..100%)
    int oilPct = static_cast<int>(std::floor(oil_g / 6553.0));
    // F16Data: oilPressure 0..10000 (0..100 * 100)
    data->oilPressure = static_cast<unsigned short>(oilPct * 100);

    // Nozzle Position:
    // Engines.ENGINE_Nozzle_L_g(): Gauge(0x44DC)
    uint16_t noz_g = readGauge(0x44DC);
    int nozPct = static_cast<int>(std::floor(noz_g / 6553.5)); // wie DCSDataReceiver.ENGINE_Nozzle_L()
    data->nozzlePos = static_cast<unsigned short>(nozPct * 100);

    // RPM:
    // Engines.ENGINE_RPM_L_g(): Gauge(0x44DE), ENGINE_RPM_L(): floor(g/6553.5) -> Prozent
    uint16_t rpm_g = readGauge(0x44DE);
    float rpmPct = std::floor(rpm_g / 6553.5f); // 0..100
    // BMSReader: data->rpm = TrimGauge(flightData->rpm, GAUGERPM);
    data->rpm = static_cast<unsigned short>(util.TrimGauge(rpmPct, GAUGERPM));

    // FTIT:
    // Engines.ENGINE_FTIT_L_g(): Gauge(0x44E0), ENGINE_FTIT_L(): floor(g/6553.5)
    uint16_t ftit_g = readGauge(0x44E0);
    float ftitC = std::floor(ftit_g / 6553.5f); // 0..1000 (in C# wird später noch als Grad interpretiert)
    data->ftit = static_cast<unsigned short>(util.TrimGauge(ftitC, GAUGEFTIT));

    // FuelFlow:
    // Engines.ENGINE_FF_L(): tt/t/h aus Gauges 0x44E4/0x44E6/0x44E8:
    // tt = floor(Gauge(0x44E4)/6553.0)   -> 10k
    //  t = floor(Gauge(0x44E6)/6553.0)   -> 1k
    //  h = floor(Gauge(0x44E8)/6553.0)   -> 100
    uint16_t ff10k_g = readGauge(0x44E4);
    uint16_t ff1k_g  = readGauge(0x44E6);
    uint16_t ff100_g = readGauge(0x44E8);
    int ff10k = static_cast<int>(std::floor(ff10k_g / 6553.0));
    int ff1k  = static_cast<int>(std::floor(ff1k_g / 6553.0));
    int ff100 = static_cast<int>(std::floor(ff100_g / 6553.0));
    int fuelFlow = ff10k * 10000 + ff1k * 1000 + ff100 * 100;
    data->fuelFlow = static_cast<unsigned int>(fuelFlow);
}

// ------------------------------------------------------
// 6) Main Instruments (Airspeed, Mach, Altimeter, VVI, AOA)
//    Altimeter-Logik 1:1 aus DCSDataReceiver.Gauges.Altimeter()
// ------------------------------------------------------

void DCSReader::mapMainInstruments(F16Data* data)
{
    // AirspeedIndicator_g():
    // F16C: Gauge(0x44A6) (Lua: AIRSPEED-Argument 48)
    uint16_t asi_g = readGauge(0x44A6);
    // Skala: 0..65535 -> 0..850 KIAS (wie in BMSReader: TrimGauge)
    float asi = asi_g / 65535.0f * 850.0f;
    data->kias = static_cast<unsigned short>(util.TrimGauge(asi, GAUGEASI));

    // Mach:
    // Gauges.ASI_Mach_g(): Gauge(0x44AC) (Lua: MACH_INDICATOR)
    uint16_t mach_g = readGauge(0x44AC);
    float mach = mach_g / 65535.0f * 2.0f; // F-16 Anzeige bis ca 2 Mach
    data->mach = static_cast<unsigned short>(mach * FLOATMULT);

    // Altimeter – LOGIK exakt wie DCSDataReceiver.Gauges.Altimeter()
    // Altimeter_10K_g(): Gauge(0x4496)
    // Altimeter_1K_g():  Gauge(0x4498)
    // Altimeter_100_g(): Gauge(0x4494) !!! in C# Altimeter() wird 0x4494 für 100 ft Drums verwendet
    uint16_t alt10k_g = readGauge(0x4496);
    uint16_t alt1k_g  = readGauge(0x4498);
    uint16_t alt100_g = readGauge(0x4494);

    // aus C#:
    // alt = floor( (Alt10K * 10000 / 65535) +
    //              (Alt1K  * 1000  / 65535) +
    //              (Alt100 * 100  / 65535.0) )
    double tenK = std::floor(alt10k_g * 10000.0 / 65535.0);
    double oneK = std::floor(alt1k_g  * 1000.0  / 65535.0);
    double oneH = std::floor(alt100_g * 100.0   / 65535.0);
    uint16_t alt = static_cast<uint16_t>(std::floor(tenK + oneK + oneH));
    data->altitude = static_cast<int>(alt);

    // Altimeter Pointer 100 ft (ALT_100_FT_PTR: Gauge(0x449A))
    uint16_t altPtr_g = readGauge(0x449A);
    // Skala 0..65535 -> 0..1000 ft
    int altPtr = static_cast<int>(std::round(altPtr_g / 65535.0 * 1000.0));
    data->altPointer = static_cast<unsigned short>(altPtr);

    // Baro Pressure Setting:
    // DCSDataReceiver.Gauges.PressureSetting():
    //   Drum0: Gauge(0x449C)
    //   Drum1: Gauge(0x449E)
    //   Drum2: Gauge(0x44A0)
    //   Drum3: Gauge(0x44A2)
    //   je Digit: floor(Gauge/6553)
    auto digitFromGauge = [this](uint16_t addr) {
        uint16_t g = readGauge(addr);
        return static_cast<int>(std::floor(g / 6553.0));
    };
    int d0 = digitFromGauge(0x449C);
    int d1 = digitFromGauge(0x449E);
    int d2 = digitFromGauge(0x44A0);
    int d3 = digitFromGauge(0x44A2);
    data->altCalibration =
        std::to_string(d0) +
        std::to_string(d1) +
        std::to_string(d2) +
        std::to_string(d3);

    // altThousands – wie im BMSReader:
    data->altThousands = std::to_string(data->altitude / 1000);

    // VVI:
    // Gauges.VVI(): F16C Gauge(0x44B8) (Lua: VVI, -1..1 -> -6000..6000 ft/min)
    uint16_t vvi_g = readGauge(0x44B8);
    double vviNorm = (vvi_g / 65535.0) * 2.0 - 1.0; // -1..1
    double vviFtPerMin = vviNorm * 6000.0;
    data->vvi = static_cast<short>(vviFtPerMin);

    // AOA:
    // Gauges.AOA(): F16C Gauge(0x4492) (Lua: AOA_VALUE, -1..1 -> -??..?? deg)
    uint16_t aoa_g = readGauge(0x4492);
    double aoaNorm = (aoa_g / 65535.0) * 2.0 - 1.0;
    // typische Range -10..30°, wir skalieren 0..1 direkt auf -10..30 zur Anzeige,
    // aber in F16Data steht Kommentar "aoa * 100(?)".
    double aoaDeg = aoaNorm * 20.0; // grobe lineare Skalierung
    data->aoa = static_cast<short>(aoaDeg * 100.0);
}

// ------------------------------------------------------
// 7) HSI / EHSI
//    DCSDataReceiver.Navigation/EHSI nutzt keine Float-Adressen direkt,
//    hier lassen wir HSI vorerst minimal gefüllt, bis wir deine
//    gewünschte Skala genau geklärt haben.
// ------------------------------------------------------

void DCSReader::mapHSI(F16Data* data)
{
    // Kurs, Range, Heading etc. baust du dir wie gehabt aus den Floats:
    /*
    float crs = readFloat01(ADR_EHSI_COURSE, 0xFFFF, 0) * 360.0f;
    if (crs < 0.0f)   crs += 360.0f;
    if (crs >= 360.0f) crs -= 360.0f;
    data->hsiDesiredCourse = static_cast<unsigned short>(crs);

    float rng = readFloat01(ADR_EHSI_RANGE, 0xFFFF, 0) * 999.0f;
    data->hsiDistanceToBeacon = static_cast<short>(rng);

    float hdg = readFloat01(ADR_STBY_HDG, 0xFFFF, 0) * 360.0f;
    data->hsiCurrentHeading = static_cast<unsigned short>(hdg);    
    */
    // Hier die Mode-Konstruktion:
    data->hsiMode = buildHSIMode();
    // Vorläufig: nur Heading aus CommonData.heading()
    // CommonData.heading(): ReadInt16(0x0436, 2, 0x01FF)
    
    data->hsiDesiredHeading =  0;
    data->hsiCourseDeviation   = 0;
    data->hsiDesiredCourse     = 0;    
    data->hsiBearingToBeacon   = 0;
    data->hsiDeviationLimit    = 0;
    data->hsiHalfDeviationLimit= 0;
    data->hsiLocalizerCourse   = 0;
    data->hsiAirbaseX          = 0;
    data->hsiAirbaseY          = 0;
}

unsigned short DCSReader::buildHSIMode()
{
    // Strings aus dem DCS-BIOS-State lesen
    std::string left  = readString(ADR_EHSI_MODE_LEFT,  LEN_EHSI_MODE_LEFT);
    std::string right = readString(ADR_EHSI_MODE_RIGHT, LEN_EHSI_MODE_RIGHT);

    // Robustheit: Leading/Trailing Spaces entfernen
    auto trim = [](std::string& s) {
        // vorne
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
            s.erase(s.begin());
        // hinten
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
            s.pop_back();
    };
    trim(left);
    trim(right);

    // Groß-/Kleinschreibung egal machen
    auto to_upper = [](std::string& s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c){ return static_cast<char>(std::toupper(c)); });
    };
    to_upper(left);
    to_upper(right);

    // Erwartete Inhalte von DCS:
    // left:  "" oder "PLS"  -> ILS aktiv?
    // right: "NAV" oder "TCN" (evtl. auch "" wenn aus?)
    bool ils = (left.find("PLS") != std::string::npos);
    bool tcn = (right.find("TCN") != std::string::npos);
    bool nav = (right.find("NAV") != std::string::npos);

    // Mapping auf BMS-Konstanten aus f16common.h:
    // MODE_ILS_TACAN = 0
    // MODE_TACAN     = 1
    // MODE_NAV       = 2
    // MODE_ILS_NAV   = 3

    if (ils && tcn) return MODE_ILS_TACAN; // PLS + TCN
    if (ils && nav) return MODE_ILS_NAV;   // PLS + NAV
    if (!ils && tcn) return MODE_TACAN;    // nur TCN
    if (!ils && nav) return MODE_NAV;      // nur NAV

    // Fallback, falls DCS etwas Unbekanntes liefert:
    // Sinnvollste Annahme: reines NAV
    return MODE_NAV;
}

// ------------------------------------------------------
// 8) Instrument Bits (Flags ADI/HSI)
//    DCSDataReceiver.Gauges.*Flag() gibt nur bools zurück,
//    aber BMSReader nutzt F4SharedMem-Enums.
//    Ohne 1:1-Entsprechung lassen wir instrumentBits 0.
// ------------------------------------------------------

void DCSReader::mapInstrumentBits(F16Data* data)
{
    data->instrumentBits = 0;

    // ADI OFF Flag
    // DCSDataReceiver.Gauges.ADI_OFF_Flag():
    //   F-16C: ReadMemoryBool(0x44BE, 2, 0x0001)
    if (readBool(0x44BE, 0x0001)) {
        setDatabit(data->instrumentBits, ADIADI);
    }

    // ADI LOC Flag
    // DCSDataReceiver.Gauges.ADI_LOC_Flag():
    //   F-16C: ReadMemoryBool(0x44C0, 2, 0x0001)
    if (readBool(0x44C0, 0x0001)) {
        // In f16common.h gibt es nur ein gemeinsames ILS-Bit (ADIILS)
        // für LOC/GS, daher setzen wir ADIILS, wenn LOC oder GS an ist.
        setDatabit(data->instrumentBits, ADIILS);
    }

    // ADI GS Flag
    // DCSDataReceiver.Gauges.ADI_GS_Flag():
    //   F-16C: ReadMemoryBool(0x44C4, 2, 0x0001)
    if (readBool(0x44C4, 0x0001)) {
        setDatabit(data->instrumentBits, ADIILS);
    }

    // HSI-/sonstige Instrument-Flags:
    // In DCSDataReceiver.cs gibt es aktuell keine separaten Bool-Methoden
    // für HSI_TO, HSI_FROM, HSI_ILS, HSI_INEFF etc. (nur BMS-seitig).
    // Solange wir keine klaren DCS-Quellen dafür haben, lassen wir
    // diese Bits vorerst 0 und ergänzen sie später gezielt.
}

// ------------------------------------------------------
// 9) Instrument Panel Lights (Eyebrows, TWP, Indexer, Marker)
//    Aus DCSDataReceiver.CautionIndexerLights.* und CMDS_ECM.*
// ------------------------------------------------------

void DCSReader::mapInstPanelLights(F16Data* data)
{
    data->instPanelLights = 0;

    // Master Caution (Eyebrow)
    if (readBool(0x447A, 0x0001)) setDatabit(data->instPanelLights, EBMASTERC);

    // TF FAIL
    if (readBool(0x447A, 0x0004)) setDatabit(data->instPanelLights, EBTFFAIL);

    // Right Eyebrows:
    if (readBool(0x447A, 0x0008)) setDatabit(data->instPanelLights, EBENGFIRE);
    if (readBool(0x447A, 0x0010)) setDatabit(data->instPanelLights, EBENGINE);
    if (readBool(0x447A, 0x0020)) setDatabit(data->instPanelLights, EBHYDOILPRESS);
    if (readBool(0x447A, 0x0040)) setDatabit(data->instPanelLights, EBFLCS);
    if (readBool(0x447A, 0x0080)) setDatabit(data->instPanelLights, EBDBUON);
    if (readBool(0x447A, 0x0100)) setDatabit(data->instPanelLights, EBTOLDGCFG);
    if (readBool(0x447A, 0x0200)) setDatabit(data->instPanelLights, EBCANOPY);
    if (readBool(0x447A, 0x0400)) setDatabit(data->instPanelLights, EBOXYLOW);

    // AOA Indexer:
    if (readBool(0x447A, 0x0800)) setDatabit(data->instPanelLights, IDXAOAABOVE);
    if (readBool(0x447A, 0x1000)) setDatabit(data->instPanelLights, IDXAOAON);
    if (readBool(0x447A, 0x2000)) setDatabit(data->instPanelLights, IDXAOABELOW);

    // AR/NWS, DISC, RDY:
    if (readBool(0x447C, 0x0008)) setDatabit(data->instPanelLights, IDXARNWS);
    if (readBool(0x447C, 0x0010)) setDatabit(data->instPanelLights, IDXDISC);
    if (readBool(0x447C, 0x0004)) setDatabit(data->instPanelLights, IDXRDY);

    // TWP (RWR Panel):
    if (readBool(0x447E, 0x0400)) setDatabit(data->instPanelLights, TWPHANDOFF);     // SEARCH
    if (readBool(0x447E, 0x8000)) setDatabit(data->instPanelLights, TWPPRIMODE);     // ACTIVITY/POWER
    if (readBool(0x447E, 0x2000)) setDatabit(data->instPanelLights, TWPLAST);        // ALTLOW
    if (readBool(0x447E, 0x4000)) setDatabit(data->instPanelLights, TWPUNKNOWN);     // ALT
    if (readBool(0x4480, 0x0004)) setDatabit(data->instPanelLights, TWPLAUNCH);      // MSL LAUNCH
    if (readBool(0x4480, 0x0010)) setDatabit(data->instPanelLights, TWPOUTBOUND);    // MODE OPEN
    if (readBool(0x4480, 0x0008)) setDatabit(data->instPanelLights, TWPPRETHREAT);   // MODE PRI
    if (readBool(0x4480, 0x0020)) setDatabit(data->instPanelLights, TWPSYSTEST);     // SHIP UNKNOWN
    if (readBool(0x4480, 0x0080)) setDatabit(data->instPanelLights, TWPTGTSEP);      // TGTSEP UP
    if (readBool(0x4480, 0x0100)) setDatabit(data->instPanelLights, TWPOVERHEAT);    // TGTSEP DOWN

    // ECM main light (Edge of glareshield)
    if (readBool(0x447E, 0x1000)) setDatabit(data->instPanelLights, ECMON);

    // Marker Beacon:
    if (readBool(0x448C, 0x2000)) setDatabit(data->instPanelLights, MARKERBEACON);
}

// ------------------------------------------------------
// 10) Left Console Lights (Gear Panel, EPU/ELEC/FLCS, AVTR, JFS)
//     aus DCSDataReceiver.GearFlapsSpeedbrake, Engines, Electrics, OtherSystems
// ------------------------------------------------------

void DCSReader::mapLeftConsole(F16Data* data)
{
    data->leftConsLights = 0;

    // Gear Panel:
    if (readBool(0x447C, 0x0002)) setDatabit(data->leftConsLights, GEARLIGHT); // Gear Unsafe
    if (readBool(0x447A, 0x4000)) setDatabit(data->leftConsLights, WNOSE);     // NoseGearLight
    if (readBool(0x447A, 0x8000)) setDatabit(data->leftConsLights, WLEFT);     // LeftGearLight
    if (readBool(0x447C, 0x0001)) setDatabit(data->leftConsLights, WRIGHT);    // RightGearLight

    // JFS RUN:
    if (readBool(0x447C, 0x0020)) setDatabit(data->leftConsLights, JFSRUN);

    // EPU Panel:
    if (readBool(0x447C, 0x0100)) setDatabit(data->leftConsLights, EPURUN);   // EPU_Active/EPU_Run
    if (readBool(0x447C, 0x0040)) setDatabit(data->leftConsLights, EPUHYD);   // JFS_HYDRA
    if (readBool(0x447C, 0x0080)) setDatabit(data->leftConsLights, EPUAIR);   // JFS_AIR

    // ELEC Panel:
    if (readBool(0x447C, 0x0200)) setDatabit(data->leftConsLights, ELPMG);     // FLCS_PMG
    if (readBool(0x447C, 0x0400)) setDatabit(data->leftConsLights, ELMAINGEN); // MAIN_GEN
    if (readBool(0x447C, 0x0800)) setDatabit(data->leftConsLights, ELSTDBYGEN);// STBY_GEN
    if (readBool(0x447C, 0x2000)) setDatabit(data->leftConsLights, ELEPUGEN);  // EPU_GEN
    if (readBool(0x447C, 0x8000)) setDatabit(data->leftConsLights, ELTOFLCS);  // TO_FLCS
    if (readBool(0x447E, 0x0001)) setDatabit(data->leftConsLights, ELFLCSRLY); // FLCS_RLY
    if (readBool(0x447E, 0x0002)) setDatabit(data->leftConsLights, ELBATTFAIL);// BatFail
    if (readBool(0x4476, 0x0200)) setDatabit(data->leftConsLights, ELEPUPMG);  // EPU_PMG

    // FLCS Test Panel (FLCS_PWR_A..D):
    if (readBool(0x447E, 0x0040)) setDatabit(data->leftConsLights, TESTA); // FLCS_PWR_A
    if (readBool(0x447E, 0x0080)) setDatabit(data->leftConsLights, TESTB); // FLCS_PWR_B
    if (readBool(0x447E, 0x0100)) setDatabit(data->leftConsLights, TESTC); // FLCS_PWR_C
    if (readBool(0x447E, 0x0200)) setDatabit(data->leftConsLights, TESTD); // FLCS_PWR_D

    // FLT CTRL BIT RUN / FAIL:
    if (readBool(0x447C, 0x0004)) setDatabit(data->leftConsLights, FLTCTLRUN);  // FLCS_RUN
    if (readBool(0x447E, 0x0020)) setDatabit(data->leftConsLights, FLTCTLFAIL); // FLCS_FAIL
}

// ------------------------------------------------------
// 11) UHF – aus DCSDataReceiver.Radios.RADIO_COM1_*
//      Adressen: 0x45C8 (freq string), 0x45B0 (preset)
// ------------------------------------------------------

void DCSReader::mapUHF(F16Data* data)
{
    // Channel (Preset):
    uint16_t ch = readInt16(0x45B0);
    data->uhfChannel = static_cast<unsigned char>(ch & 0xFF);

    // Frequency:
    std::string freqStr = readString(0x45C8, 7); // "xxx.xxy"
    // In C#: Remove(3,1) -> ohne Punkt
    if (freqStr.size() >= 4) {
        freqStr.erase(3, 1);
    }
    data->uhfFrequency = freqStr;
}

// ------------------------------------------------------
// 12) IFF / AUX Display – aus OtherSystems.IFFCode()
//      IFFDig1: 0x44FA, IFFDig2: 0x44FC, IFFDig3: 0x4EFE, IFFDig4: 0x4500
// ------------------------------------------------------

void DCSReader::mapIFFAux(F16Data* data)
{
    uint8_t d1 = static_cast<uint8_t>(readInt16(0x44FA));
    uint8_t d2 = static_cast<uint8_t>(readInt16(0x44FC));
    uint8_t d3 = static_cast<uint8_t>(readInt16(0x4EFE));
    uint8_t d4 = static_cast<uint8_t>(readInt16(0x4500));

    data->iffDisplay =
        std::to_string(d1) +
        std::to_string(d2) +
        std::to_string(d3) +
        std::to_string(d4);

    // AUX Display: DCSDataReceiver nutzt für AUX die BMS-AUXTChan,
    // die wir hier nicht haben. Vorläufig leer.
    data->auxDisplay = "    ";
}

// ------------------------------------------------------
// 13) Trim (Pitch/Roll) – aus OtherSystems.TrimPitch/TrimRoll
//      TrimPitch: Gauge(0x44D2), TrimRoll: Gauge(0x44D0)
//      DCSDataReceiver gibt Rohwerte, BMS speichert in FlightData.* *FLOATMULT.
// ------------------------------------------------------

void DCSReader::mapTrim(F16Data* data)
{
    uint16_t trimPitch_g = readGauge(0x44D2);
    uint16_t trimRoll_g  = readGauge(0x44D0);

    // Lua: ROLLTRIMIND/PITCHTRIMIND Range -1..1
    double pitchNorm = trimPitch_g / 65535.0 * 2.0 - 1.0;
    double rollNorm  = trimRoll_g  / 65535.0 * 2.0 - 1.0;

    // F16Data: -0.5 .. +0.5 * 100
    data->trimPitch = static_cast<short>(pitchNorm * 50.0 * 100.0 / 100.0);
    data->trimRoll  = static_cast<short>(rollNorm  * 50.0 * 100.0 / 100.0);
}

// ------------------------------------------------------
// 14) CMDS – aus CMDS_ECM.*
//      Displays O1/O2/Chaff/Flare, GO/NOGO/RDY/DISP
// ------------------------------------------------------

void DCSReader::mapCMDS(F16Data* data)
{
    data->cmdsBits = 0;

    // O1/O2 Display Strings:
    data->cmds01Str = readString(0x45A0, 4); // O1Display()
    data->cmds02Str = readString(0x45A4, 4); // O2Display()

    // Chaff/Flare Displays: logik aus DCSDataReceiver.ChaffDisplay()/FlareDisplay()
    auto parseCMDSCount = [this](uint16_t addr) -> int {
        std::string s = readString(addr, 4);
        if (s.empty()) return 0;
        if (s[0] == 0 || s[0] == '0' || s[0] == ' ') {
            if (s.size() < 4) return 0;
            std::string sub = s.substr(2, 2);
            try {
                return std::stoi(sub);
            }
            catch (...) { return 0; }
        } else {
            // Hunderterstelle => "99" wenn >0 sonst 0
            if (isdigit(static_cast<unsigned char>(s[0]))) {
                int c = s[0] - '0';
                if (c > 0) return 99;
            }
            return 0;
        }
    };

    int chaff = parseCMDSCount(0x45A8);
    int flare = parseCMDSCount(0x45AC);
    data->cmdsChaffStr = (chaff < 10 ? "0" : "") + std::to_string(chaff);
    data->cmdsFlareStr = (flare < 10 ? "0" : "") + std::to_string(flare);

    // Status-Bits:
    // CMDS_GO():   ReadBool(0x4480, 0x0400)
    // CMDS_NOGO(): ReadBool(0x4480, 0x0200)
    // CMDS_RDY():  ReadBool(0x4480, 0x1000)
    // CMDS_DISP(): ReadBool(0x4480, 0x0800)
    if (readBool(0x4480, 0x0400)) setDatabit(data->cmdsBits, CMDSGO);
    if (readBool(0x4480, 0x0200)) setDatabit(data->cmdsBits, CMDSNOGO);
    if (readBool(0x4480, 0x1000)) setDatabit(data->cmdsBits, CMDSRDY);
    if (readBool(0x4480, 0x0800)) setDatabit(data->cmdsBits, CMDSDEGR); // DEGR gibt es in DCSDataReceiver nicht, hier notfalls auf DISP oder weglassen
}

// ------------------------------------------------------
// 15) ADI – aus Lua: ADI_PITCH/BANK/LOC_BAR/GS_BAR/BUBBLE
//     DCSDataReceiver hat nur bool-Flags; konkrete Gauges nutzen wir aus Lua.
// ------------------------------------------------------

void DCSReader::mapADI(F16Data* data)
{
    // ADI_PITCH: Float(17) -> Gaugeadresse in DCS-BIOS-Buffer entspricht Lua-Index,
    // aber wir haben hier keine direkte 0x44xx Adresse aus DCSDataReceiver.
    // DCSDataReceiver fragt nur Flags (ADI_OFF_Flag etc.), nicht die Balken.
    // Daher setzen wir ADI-Werte vorerst 0.
    data->adiPitch     = 0;
    data->adiRoll      = 0;
    data->adiIlsHorPos = 0;
    data->adiIlsVerPos = 0;
    data->adiSideslip  = 0;
}

// ------------------------------------------------------
// 16) ECM – ECM-Lichter matrix aus CMDS_ECM.*
//     C# nutzt ecmBits[] aus FlightData2, die wir hier nicht haben.
//     Wir mappen nur einfache Lampen: ECM-Panel Buttons S/A/F/T
// ------------------------------------------------------

void DCSReader::mapECM(F16Data* data)
{
    data->ecmLights = 0;

    // 1er Button (PROGRAM 1)
    // C# CMDS_ECM.ECM_Button1S/A/F/T
    if (readBool(0x4480, 0x2000)) setDatabit(data->ecmLights, ECM1S);
    if (readBool(0x4480, 0x4000)) setDatabit(data->ecmLights, ECM1A);
    if (readBool(0x4480, 0x8000)) setDatabit(data->ecmLights, ECM1F);
    if (readBool(0x448A, 0x0001)) setDatabit(data->ecmLights, ECM1T);

    // 2er Button (PROGRAM 2)
    // C# CMDS_ECM.ECM_Button2S/A/F/T
    if (readBool(0x448A, 0x0002)) setDatabit(data->ecmLights, ECM2S);
    if (readBool(0x448A, 0x0004)) setDatabit(data->ecmLights, ECM2A);
    if (readBool(0x448A, 0x0008)) setDatabit(data->ecmLights, ECM2F);
    if (readBool(0x448A, 0x0010)) setDatabit(data->ecmLights, ECM2T);

    // 3er Button (PROGRAM 3)
    // C# CMDS_ECM.ECM_Button3S/A/F/T
    if (readBool(0x448A, 0x0020)) setDatabit(data->ecmLights, ECM3S);
    if (readBool(0x447C, 0x0020)) setDatabit(data->ecmLights, ECM3A);
    if (readBool(0x448A, 0x0080)) setDatabit(data->ecmLights, ECM3F);
    if (readBool(0x448A, 0x0100)) setDatabit(data->ecmLights, ECM3T);

    // 4er Button (PROGRAM 4)
    // C# CMDS_ECM.ECM_Button4S/A/F/T
    if (readBool(0x448A, 0x0200)) setDatabit(data->ecmLights, ECM4S);
    if (readBool(0x448A, 0x0400)) setDatabit(data->ecmLights, ECM4A);
    if (readBool(0x448A, 0x0800)) setDatabit(data->ecmLights, ECM4F);
    if (readBool(0x448A, 0x1000)) setDatabit(data->ecmLights, ECM4T);

    // 5er Button (PROGRAM 5)
    // C# CMDS_ECM.ECM_Button5S/A/F/T
    if (readBool(0x448A, 0x2000)) setDatabit(data->ecmLights, ECM5S);
    if (readBool(0x448A, 0x4000)) setDatabit(data->ecmLights, ECM5A);
    if (readBool(0x448A, 0x8000)) setDatabit(data->ecmLights, ECM5F);
    if (readBool(0x448C, 0x0001)) setDatabit(data->ecmLights, ECM5T);

    // FRM (Frame) – C# CMDS_ECM.ECM_FRM_S/A/F/T
    if (readBool(0x448C, 0x0002)) setDatabit(data->ecmLights, ECMFRMS);
    if (readBool(0x448C, 0x0004)) setDatabit(data->ecmLights, ECMFRMA);
    if (readBool(0x448C, 0x0008)) setDatabit(data->ecmLights, ECMFRMF);
    if (readBool(0x448C, 0x0010)) setDatabit(data->ecmLights, ECMFRMT);

    // SPL (Single) – C# CMDS_ECM.ECM_SPL_S/A/F/T
    if (readBool(0x448C, 0x0020)) setDatabit(data->ecmLights, ECMSPLS);
    if (readBool(0x448C, 0x0040)) setDatabit(data->ecmLights, ECMSPLA);
    if (readBool(0x448C, 0x0080)) setDatabit(data->ecmLights, ECMSPLF);
    if (readBool(0x448C, 0x0100)) setDatabit(data->ecmLights, ECMSPLT);
}