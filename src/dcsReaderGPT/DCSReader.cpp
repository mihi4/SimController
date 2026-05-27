#include "DCSReader.h"  
#include "F16Data.h"  
#include "../../Arduino/SimController/f16common.h" // für Bits/Defines und FLOATMULT etc.  
  
DCSReader::DCSReader(const std::string& host, int port) : host(host), port(port) {  
    for (int i = 0; i < BLINKBITNUM; ++i) {  
        status[i] = true;  
        lastToggleTimes[i] = std::chrono::steady_clock::now();  
    }  
}  
  
DCSReader::~DCSReader() {  
    stop();  
    disconnect();  
}  
  
bool DCSReader::initSocket() {  
#ifdef _WIN32  
    WSADATA wsa;  
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;  
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
    if (sock == INVALID_SOCKET) return false;  
  
    sockaddr_in srv = {};  
    srv.sin_family = AF_INET;  
    srv.sin_port = htons((u_short)port);  
    srv.sin_addr.s_addr = inet_addr(host.c_str());  
  
    if (connect(sock, (sockaddr*)&srv, sizeof(srv)) == SOCKET_ERROR) {  
        closesocket(sock);  
        sock = INVALID_SOCKET;  
        return false;  
    }  
    return true;  
#else  
    // TODO: POSIX Sockets  
    return false;  
#endif  
}  
  
bool DCSReader::connectToSim() {  
    if (sock != INVALID_SOCKET) return true;  
    return initSocket();  
}  
  
void DCSReader::disconnect() {  
#ifdef _WIN32  
    if (sock != INVALID_SOCKET) {  
        closesocket(sock);  
        sock = INVALID_SOCKET;  
        WSACleanup();  
    }  
#endif  
}  
  
void DCSReader::start() {  
    if (running) return;  
    if (!connectToSim()) {  
        std::cerr << "DCSReader: Verbindung zum DCS-BIOS Hub fehlgeschlagen\n";  
        return;  
    }  
    running = true;  
    rxThread = std::thread(&DCSReader::rxLoop, this);  
}  
  
void DCSReader::stop() {  
    if (!running) return;  
    running = false;  
    if (rxThread.joinable()) rxThread.join();  
}  
  
void DCSReader::rxLoop() {  
    std::string buffer;  
    buffer.reserve(4096);  
  
#ifdef _WIN32  
    char rx[2048];  
    while (running) {  
        int n = recv(sock, rx, sizeof(rx), 0);  
        if (n <= 0) {  
            std::this_thread::sleep_for(std::chrono::milliseconds(50));  
            continue;  
        }  
        buffer.append(rx, rx + n);  
  
        // Nachrichten trennen (angenommen: '\n' getrennt; passe an dein Hub-Format an)  
        size_t pos;  
        while ((pos = buffer.find('\n')) != std::string::npos) {  
            std::string line = buffer.substr(0, pos);  
            buffer.erase(0, pos + 1);  
            parseHubMessage(line);  
        }  
    }  
#endif  
}  
  
// Beispielparser: erwartet einfaches "key=value" oder JSON-ähnliche Zeilen.  
// Passe das an dein tatsächliches DCS-BIOS Hub-Format an.  
void DCSReader::parseHubMessage(const std::string& msg) {  
    std::lock_guard<std::mutex> lk(dataMutex);  
  
    // simple key=value  
    auto eq = msg.find('=');  
    if (eq != std::string::npos) {  
        std::string key = msg.substr(0, eq);  
        std::string val = msg.substr(eq + 1);  
        // Versuch numerisch zu interpretieren  
        try {  
            double d = std::stod(val);  
            numVals[key] = d;  
            intVals[key] = (int)d;  
            boolVals[key] = (d != 0.0);  
        }  
        catch (...) {  
            strVals[key] = val;  
        }  
        return;  
    }  
  
    // TODO: JSON parser oder anderes Protokoll  
}  
  
unsigned short DCSReader::mapU16(double in, double inMin, double inMax, unsigned short outMin, unsigned short outMax) {  
    if (in < inMin) in = inMin;  
    if (in > inMax) in = inMax;  
    double ratio = (in - inMin) / (inMax - inMin);  
    return (unsigned short)(outMin + ratio * (outMax - outMin));  
}  
  
// Nutzt dieselbe Logik wie in BMSReader::trimDED_PFD (angepasst, da DCS-BIOS i. d. R. 24/25 Zeichen und invertierte Maske separat liefert)  
std::string DCSReader::trimDED_PFD(const std::string& line, const std::string& invMask) {  
    const char charNum = 25;  
    if (line.size() < charNum) {  
        std::string l = line;  
        l.resize(charNum, ' ');  
        // einfach auffüllen  
        // invMask analog auffüllen  
    }  
    std::string out(charNum, ' ');  
    for (char ctr = 0; ctr < charNum; ctr++) {  
        char c = line[ctr];  
        char inv = ctr < (char)invMask.size() ? invMask[ctr] : 0;  
  
        if (inv == 2) { // invertiert  
            if (std::isalpha((unsigned char)c)) { out[ctr] = (char)(c + 36); }  
            else if (std::isdigit((unsigned char)c)) { out[ctr] = (char)(c + 43); }  
            else if (c == 1) { out[ctr] = (char)36; }  
            else if (c == 2) { out[ctr] = (char)37; }  
            else if (c == 3) { out[ctr] = (char)223; }  
            else if (c == 32) { out[ctr] = (char)33; }  
            else { out[ctr] = c; }  
        } else {  
            if (c == 1) { out[ctr] = (char)36; }  
            else if (c == 2) { out[ctr] = (char)37; }  
            else if (c == 3) { out[ctr] = '_'; }  
            else if (c == '~') { out[ctr] = (char)29; }  
            else if (c == 94) { out[ctr] = (char)63; }  
            else if (c == 125) { out[ctr] = (char)30; }  
            else if (c == 123) { out[ctr] = (char)31; }  
            else { out[ctr] = c; }  
        }  
    }  
    return out;  
}  
  
bool DCSReader::getBlinkStatus(int idx) {  
    auto now = std::chrono::steady_clock::now();  
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastToggleTimes[idx]).count();  
    if (elapsed >= intervals[idx]) {  
        status[idx] = !status[idx];  
        lastToggleTimes[idx] = now;  
    }  
    return status[idx];  
}  
  
void DCSReader::readF16Data(F16Data* data) {  
    std::lock_guard<std::mutex> lk(dataMutex);  
  
    // SIM/power/relay states:  
    // DCS-BIOS bietet hier weniger direkte Bits als BMS. Falls verfügbar, setze sie entsprechend;  
    // sonst lass sie 0 oder implementiere eine Logik über System-Flags.  
    data->simStates = 0;  
    data->powerStates = 0;  
    data->relayStates = 0;  
  
    // Right AUX  
    data->fuelFWD = (unsigned short)(intVals[keys.fuelFwd]); // DCS liefert oft in lbs; dein F16Data erwartet 0..4200  
    data->fuelAFT = (unsigned short)(intVals[keys.fuelAft]);  
    data->fuelTotal = (unsigned char)(intVals[keys.fuelTotal] / 100); // Hunderter lbs  
  
    data->hydA = (unsigned short)(intVals[keys.hydA]); // 0..4000  
    data->hydB = (unsigned short)(intVals[keys.hydB]);  
    data->epuFuel = mapU16(numVals[keys.epuFuel], 0, 100, 0, 65535); // Prozent auf 0..65535  
    data->cabinPress = mapU16(intVals[keys.cabinAlt], 0, 50000, 0, 65535);  
  
    // Caution panel bits  
    auto setCP = [&](unsigned long bit, const char* key) {  
        if (boolVals[key]) data->cautionPanelLights |= bit; else data->cautionPanelLights &= ~bit;  
    };  
    setCP(CPSEAT, keys.cp_seatArm);  
    setCP(CPNWS, keys.cp_nws);  
    setCP(CPANTISKID, keys.cp_antiSkid);  
    setCP(CPHOOK, keys.cp_hook);  
    setCP(CPAVIONICS, keys.cp_avionics);  
    setCP(CPEQUIPHOT, keys.cp_equipHot);  
    setCP(CPRADARALT, keys.cp_radarAlt);  
    setCP(CPIFF, keys.cp_iff);  
    setCP(CPENGINE, keys.cp_engine);  
    setCP(CPSEC, keys.cp_sec);  
    setCP(CPFUELOIL, keys.cp_fuelOil);  
    setCP(CPINLET, keys.cp_inletIcing);  
    setCP(CPFLCS, keys.cp_flcs);  
    setCP(CPELECSYS, keys.cp_elecSys);  
    setCP(CPPROBEHEAT, keys.cp_probeHeat);  
    setCP(CPCADC, keys.cp_cadc);  
    setCP(CPAFTFUEL, keys.cp_aftFuelLow);  
    setCP(CPFWDFUEL, keys.cp_fwdFuelLow);  
    setCP(CPATF, keys.cp_atf);  
    setCP(CPSTORESCFG, keys.cp_storesCfg);  
    setCP(CPCABIN, keys.cp_cabin);  
    setCP(CPOXYLOW, keys.cp_oxyLow);  
  
    // PFL (PFD) Strings  
    data->pfdLine1 = trimDED_PFD(strVals[keys.pfl1], strVals[keys.pflInv1]);  
    data->pfdLine2 = trimDED_PFD(strVals[keys.pfl2], strVals[keys.pflInv2]);  
    data->pfdLine3 = trimDED_PFD(strVals[keys.pfl3], strVals[keys.pflInv3]);  
    data->pfdLine4 = trimDED_PFD(strVals[keys.pfl4], strVals[keys.pflInv4]);  
    data->pfdLine5 = trimDED_PFD(strVals[keys.pfl5], strVals[keys.pflInv5]);  
  
    // Engine cluster  
    data->oilPressure = mapU16(numVals[keys.oilPressure], 0, 100, 0, 65535);  
    data->nozzlePos = mapU16(numVals[keys.nozzlePos], 0, 100, 0, 65535);  
    data->rpm = (unsigned short)(numVals[keys.rpm] * FLOATMULT);  
    data->ftit = (unsigned short)(numVals[keys.ftit]); // ggf. eigene Skalierung  
  
    data->fuelFlow = (unsigned int)(intVals[keys.fuelFlow] / 100); // analog BMS nur erste 3 Stellen  
  
    // DED strings  
    data->dedLine1 = trimDED_PFD(strVals[keys.ded1], strVals[keys.dedInv1]);  
    data->dedLine2 = trimDED_PFD(strVals[keys.ded2], strVals[keys.dedInv2]);  
    data->dedLine3 = trimDED_PFD(strVals[keys.ded3], strVals[keys.dedInv3]);  
    data->dedLine4 = trimDED_PFD(strVals[keys.ded4], strVals[keys.dedInv4]);  
    data->dedLine5 = trimDED_PFD(strVals[keys.ded5], strVals[keys.dedInv5]);  
  
    // Main instruments  
    data->kias = (unsigned short)(numVals[keys.kias] * FLOATMULT);  
    data->mach = (unsigned short)(numVals[keys.mach] * FLOATMULT);  
    data->altitude = (int)(intVals[keys.altitude]); // DCS ist i. d. R. positiv, deine BMS-Logik invertiert AAUZ  
    data->altPointer = data->altitude % 1000;  
    data->altCalibration = std::to_string(intVals[keys.altCal]);  
    data->altThousands = std::to_string((int)(data->altitude / 100));  
    data->vvi = (short)intVals[keys.vvi];  
    data->aoa = (short)(numVals[keys.aoa] * FLOATMULT);  
  
    // Instrument bits  
    auto setIB = [&](unsigned short bit, const char* key) {  
        if (boolVals[key]) data->instrumentBits |= bit; else data->instrumentBits &= ~bit;  
    };  
    setIB(INSTPNEU, keys.inst_pneu);  
    setIB(INSTVVIFLAG, keys.vvi_flag);  
    setIB(INSTAOAFLAG, keys.aoa_flag);  
    setIB(INSTADIOFF, keys.adi_off);  
    setIB(INSTADIAUX, keys.adi_aux);  
    setIB(INSTADIGS, keys.adi_gs);  
    setIB(INSTADILOC, keys.adi_loc);  
    setIB(INSTHSIOFF, keys.hsi_off);  
    setIB(INSTHSITO, keys.hsi_to);  
    setIB(INSTHSIFROM, keys.hsi_from);  
    setIB(INSTHSIILSWARN, keys.hsi_ils_warn);  
    setIB(INSTHSICRSWARN, keys.hsi_crs_warn);  
    setIB(INSTHSIINIT, keys.hsi_init);  
  
    // InstPanel lights (MM5451 Bits)  
    auto setIPL = [&](unsigned int bit, const char* key, int blinkIdx = -1) {  
        bool on = boolVals[key];  
        if (blinkIdx >= 0 && on) on = getBlinkStatus(blinkIdx);  
        if (on) data->instPanelLights |= bit; else data->instPanelLights &= ~bit;  
    };  
    setIPL(EBMASTERC, keys.eb_masterC);  
    setIPL(EBTFFAIL, keys.eb_tfFail);  
    setIPL(TWPHANDOFF, keys.twp_handOff);  
    setIPL(TWPLAUNCH, keys.twp_launch, 1);     // Beispiel: Blinkindex wie in BMS  
    setIPL(TWPPRIMODE, keys.twp_priMode, 2);  
    setIPL(TWPUNKNOWN, keys.twp_unknown, 3);  
    setIPL(TWPSYSTEST, keys.twp_sysTest);  
    setIPL(TWPTGTSEP, keys.twp_tgtSep);  
    setIPL(ECMON, keys.ecm_on);  
    setIPL(MODEACTIVE, keys.tfr_engaged);  
    setIPL(MODESTBY, keys.tfr_stby);  
    setIPL(IDXAOAABOVE, keys.idx_aoaAbove);  
    setIPL(IDXAOAON, keys.idx_aoaOn);  
    setIPL(IDXAOABELOW, keys.idx_aoaBelow);  
    setIPL(IDXRDY, keys.idx_rdy);  
    setIPL(IDXARNWS, keys.idx_arNws);  
    setIPL(IDXDISC, keys.idx_disc);  
    setIPL(EBENGFIRE, keys.eb_engFire);  
    setIPL(EBENGINE, keys.eb_engine);  
    setIPL(EBHYDOILPRESS, keys.eb_hydOil);  
    setIPL(EBFLCS, keys.eb_flcs);  
    setIPL(EBDBUON, keys.eb_dbuOn);  
    setIPL(EBTOLDGCFG, keys.eb_toldgCfg);  
    setIPL(EBCANOPY, keys.eb_canopy);  
    setIPL(EBOXYLOW, keys.eb_oxyLow);  
  
    // Marker beacon  
    setIPL(MARKERBEACON, keys.marker_outer, 10);  
    setIPL(MARKERBEACON, keys.marker_middle, 11);  
  
    // Left console lights  
    auto setLCL = [&](unsigned int bit, const char* key, int blinkIdx = -1) {  
        bool on = boolVals[key];  
        if (blinkIdx >= 0 && on) on = getBlinkStatus(blinkIdx);  
        if (on) data->leftConsLights |= bit; else data->leftConsLights &= ~bit;  
    };  
  
    setLCL(GEARLIGHT, keys.gearHandle);  
    setLCL(WNOSE, keys.noseDown);  
    setLCL(WLEFT, keys.leftDown);  
    setLCL(WRIGHT, keys.rightDown);  
    setLCL(TWASEARCH, keys.twa_search, 4);  
    setLCL(TWAACT, keys.twa_act);  
    setLCL(TWALOWALT, keys.twa_lowAlt);  
    setLCL(TWASYSPWT, keys.twa_sysPwr);  
    setLCL(JFSRUN, keys.jfs_run, 5);  
    setLCL(EPURUN, keys.epu_run, 6);  
    setLCL(EPUHYD, keys.epu_hyd);  
    setLCL(EPUAIR, keys.epu_air);  
    setLCL(ELPMG, keys.el_pmg);  
    setLCL(ELMAINGEN, keys.el_mainGen);  
    setLCL(ELSTDBYGEN, keys.el_stbyGen);  
    setLCL(ELEPUGEN, keys.el_epuGen);  
    setLCL(ELEPUPMG, keys.el_epuPmg);  
    setLCL(ELTOFLCS, keys.el_toFlcs);  
    setLCL(ELBATTFAIL, keys.el_battFail);  
    setLCL(ELFLCSRLY, keys.el_flcsRly);  
    setLCL(AVTRRUN, keys.avtr_run);  
    setLCL(AVTRCTVS, keys.avtr_ctvs);  
    setLCL(FLTCTLRUN, keys.fltctl_run);  
    setLCL(FLTCTLFAIL, keys.fltctl_fail);  
    setLCL(TESTA, keys.testA);  
    setLCL(TESTB, keys.testB);  
    setLCL(TESTC, keys.testC);  
    setLCL(TESTD, keys.testD);  
  
    // UHF/AUX/IFF  
    data->uhfChannel = (unsigned char)intVals[keys.uhf_preset];  
    data->uhfFrequency = std::to_string(intVals[keys.uhf_frequency]);  
  
    // IFF/AUX Displays  
    // Beispiel: Mode1/3A Digits zusammenbauen oder aus Backup-String extrahieren  
    data->iffDisplay = "    ";  
    if (strVals.count(keys.iff_mode_str)) {  
        std::string s = strVals[keys.iff_mode_str];  
        s.resize(4, ' ');  
        data->iffDisplay = s;  
    }  
    // TACAN Anzeige  
    int tcnChan = intVals[keys.aux_tacan_chan];  
    int tcnBand = intVals[keys.aux_tacan_band]; // 0=A, 1=X, etc.  
    data->auxDisplay = (tcnBand == 0 ? "A" : "X") + std::to_string(tcnChan);  
    if (data->auxDisplay.size() < 4) data->auxDisplay.insert(data->auxDisplay.begin(), 4 - data->auxDisplay.size(), ' ');  
  
    // Trim  
    data->trimPitch = (short)(numVals[keys.trim_pitch] * FLOATMULT);  
    data->trimRoll = (short)(numVals[keys.trim_roll] * FLOATMULT);  
  
    // CMDS  
    data->cmdsChaffStr = std::to_string(intVals[keys.cmds_chaff]);  
    data->cmdsFlareStr = std::to_string(intVals[keys.cmds_flare]);  
    auto setCMDS = [&](unsigned char bit, const char* key) {  
        if (boolVals[key]) data->cmdsBits |= bit; else data->cmdsBits &= ~bit;  
    };  
    setCMDS(CMDSGO, keys.cmds_go);  
    setCMDS(CMDSNOGO, keys.cmds_nogo);  
    setCMDS(CMDSDEGR, keys.cmds_degr);  
    setCMDS(CMDSRDY, keys.cmds_rdy);  
  
    // ADI  
    data->adiPitch = (int)(numVals[keys.adi_pitch] * FLOATMULT);  
    data->adiRoll = (int)(numVals[keys.adi_roll] * FLOATMULT);  
    data->adiIlsHorPos = (int)(numVals[keys.adi_ils_hor] * FLOATMULT);  
    data->adiIlsVerPos = (int)(numVals[keys.adi_ils_ver] * FLOATMULT);  
    data->adiSideslip = (int)(numVals[keys.sideslip] * FLOATMULT);  
  
    // SimVersion – für DCS kannst du eine feste Bezeichnung setzen  
    data->simVersion = "DCS-BIOS";  
}  