#pragma once  
  
#include "DataReader.h"  
#include "../lib/FlightData.h" // nur falls du gemeinsame Typen/Utils brauchst; sonst entfernen  
#include <string>  
#include <unordered_map>  
#include <functional>  
#include <mutex>  
#include <thread>  
#include <atomic>  
#include <vector>  
#include <chrono>  
#include <cctype>  
#include <sstream>  
#include <iostream>  
#ifdef _WIN32  
#include <winsock2.h>  
#pragma comment(lib, "ws2_32.lib")  
#endif  
  
// Ein einfacher DCS-BIOS Hub TCP-Client.  
// Erwartet, dass der DCS-BIOS Hub läuft und ein TCP-Export anbietet (z. B. 7778).  
// Alternativ kannst du die „Serial“-Variante verwenden und die COM-Zeilen parsen.  
  
class DCSReader : public DataReader {  
public:  
    DCSReader(const std::string& host = "127.0.0.1", int port = 7778);  
    ~DCSReader();  
  
    bool connectToSim();  
    void disconnect();  
  
    // Füllt F16Data aus den zuletzt empfangenen DCS-BIOS-Werten  
    void readF16Data(F16Data* data);  
  
    // Optional: Startet einen Hintergrundthread, der kontinuierlich Daten vom Hub liest  
    void start();  
    void stop();  
  
private:  
    std::string host;  
    int port;  
  
#ifdef _WIN32  
    SOCKET sock = INVALID_SOCKET;  
#else  
    int sock = -1;  
#endif  
  
    std::atomic<bool> running{false};  
    std::thread rxThread;  
    std::mutex dataMutex;  
  
    // Rohdaten-Cache: key = DCS-BIOS control ID / field name, value = last string/numeric  
    std::unordered_map<std::string, std::string> strVals;  
    std::unordered_map<std::string, double> numVals;  
    std::unordered_map<std::string, int> intVals;  
    std::unordered_map<std::string, bool> boolVals;  
  
    // Hilfsfunktionen  
    bool initSocket();  
    void rxLoop();  
    void parseHubMessage(const std::string& msg);  
  
    // Formatierungs-/Mapping-Helfer (ähnlich zu deinem miUtility)  
    unsigned short mapU16(double in, double inMin, double inMax, unsigned short outMin, unsigned short outMax);  
    std::string trimDED_PFD(const std::string& line, const std::string& invMask);  
  
    // Blink-Logik analog BMSReader  
    static constexpr int BLINKBITNUM = 13;  
    int intervals[BLINKBITNUM] = { BLINKSLOW, BLINKFAST, BLINKSLOW, BLINKFAST, BLINKFAST, BLINKFAST, BLINKSLOW, BLINKSLOW, BLINKSLOW, BLINKSLOW, BLINKSLOW, BLINKFAST, BLINKSLOW };  
    bool status[BLINKBITNUM]{};  
    std::chrono::steady_clock::time_point lastToggleTimes[BLINKBITNUM];  
    bool getBlinkStatus(int blinkIndex);  
  
    // DCS-BIOS -> F16Data Mapping: Namen der Controls/Exports  
    // Hinweis: Passe die Namen an deine DCS-BIOS-Version/Module an.  
    struct DcsKeys {  
        // Right AUX  
        const char* fuelFwd = "F16C_FUEL_QTY_FWD";       // TODO: kontrolliere den genauen Namen  
        const char* fuelAft = "F16C_FUEL_QTY_AFT";       // TODO  
        const char* fuelTotal = "F16C_FUEL_TOTAL";       // TODO  
        const char* hydA = "F16C_HYD_PRESS_A";           // TODO  
        const char* hydB = "F16C_HYD_PRESS_B";           // TODO  
        const char* epuFuel = "F16C_EPU_FUEL";           // TODO (Prozent)  
        const char* cabinAlt = "F16C_CABIN_ALT";         // TODO  
  
        // Caution panel – einzelne Lampenbits; hier exemplarisch  
        const char* cp_seatArm = "F16C_CAUT_SEAT_ARM";   // TODO  
        const char* cp_nws = "F16C_CAUT_NWS";            // TODO  
        const char* cp_antiSkid = "F16C_CAUT_ANTI_SKID"; // TODO  
        const char* cp_hook = "F16C_CAUT_HOOK";          // TODO  
        const char* cp_avionics = "F16C_CAUT_AVIONICS";  // TODO  
        const char* cp_equipHot = "F16C_CAUT_EQUIP_HOT"; // TODO  
        const char* cp_radarAlt = "F16C_CAUT_RADAR_ALT"; // TODO  
        const char* cp_iff = "F16C_CAUT_IFF";            // TODO  
        const char* cp_engine = "F16C_CAUT_ENGINE";      // TODO  
        const char* cp_sec = "F16C_CAUT_SEC";            // TODO  
        const char* cp_fuelOil = "F16C_CAUT_FUEL_OIL";   // TODO  
        const char* cp_inletIcing = "F16C_CAUT_INLET";   // TODO  
        const char* cp_flcs = "F16C_CAUT_FLCS";          // TODO  
        const char* cp_elecSys = "F16C_CAUT_ELEC";       // TODO  
        const char* cp_probeHeat = "F16C_CAUT_PROBE_HEAT";// TODO  
        const char* cp_cadc = "F16C_CAUT_CADC";          // TODO  
        const char* cp_aftFuelLow = "F16C_CAUT_AFT_FUEL";// TODO  
        const char* cp_fwdFuelLow = "F16C_CAUT_FWD_FUEL";// TODO  
        const char* cp_atf = "F16C_CAUT_ATF";            // TODO  
        const char* cp_storesCfg = "F16C_CAUT_STORES_CFG";// TODO  
        const char* cp_cabin = "F16C_CAUT_CABIN";        // TODO  
        const char* cp_oxyLow = "F16C_CAUT_OXY_LOW";     // TODO  
        // CP line lamps (1..6, NUCLEAR, EEC) sind spezifisch – ggf. als Testlampen verfügbar  
  
        // PFD strings  
        const char* pfl1 = "F16C_PFL_LINE1";             // TODO  
        const char* pfl2 = "F16C_PFL_LINE2";             // TODO  
        const char* pfl3 = "F16C_PFL_LINE3";             // TODO  
        const char* pfl4 = "F16C_PFL_LINE4";             // TODO  
        const char* pfl5 = "F16C_PFL_LINE5";             // TODO  
        const char* pflInv1 = "F16C_PFL_INV1";           // TODO (Maske)  
        const char* pflInv2 = "F16C_PFL_INV2";           // TODO  
        const char* pflInv3 = "F16C_PFL_INV3";           // TODO  
        const char* pflInv4 = "F16C_PFL_INV4";           // TODO  
        const char* pflInv5 = "F16C_PFL_INV5";           // TODO  
  
        // Engine cluster  
        const char* oilPressure = "F16C_OIL_PRESS";      // TODO (%)  
        const char* nozzlePos = "F16C_NOZZLE_POS";       // TODO (%)  
        const char* rpm = "F16C_RPM";                    // TODO (% oder N2/N1)  
        const char* ftit = "F16C_FTIT";                  // TODO (°C)  
  
        const char* fuelFlow = "F16C_FUEL_FLOW";         // TODO (pph)  
  
        // DED strings  
        const char* ded1 = "F16C_DED_LINE1";             // TODO  
        const char* ded2 = "F16C_DED_LINE2";             // TODO  
        const char* ded3 = "F16C_DED_LINE3";             // TODO  
        const char* ded4 = "F16C_DED_LINE4";             // TODO  
        const char* ded5 = "F16C_DED_LINE5";             // TODO  
        const char* dedInv1 = "F16C_DED_INV1";           // TODO  
        const char* dedInv2 = "F16C_DED_INV2";           // TODO  
        const char* dedInv3 = "F16C_DED_INV3";           // TODO  
        const char* dedInv4 = "F16C_DED_INV4";           // TODO  
        const char* dedInv5 = "F16C_DED_INV5";           // TODO  
  
        // Main instruments  
        const char* kias = "F16C_KIAS";                  // TODO  
        const char* mach = "F16C_MACH";                  // TODO  
        const char* altitude = "F16C_ALTITUDE";          // TODO (Baro alt)  
        const char* altCal = "F16C_ALT_CAL";             // TODO (2992/1013)  
        const char* vvi = "F16C_VVI";                    // TODO (ft/min)  
        const char* aoa = "F16C_AOA";                    // TODO (deg)  
  
        // Instrument bits/flags  
        const char* inst_pneu = "F16C_ALT_PNEU_FLAG";    // TODO  
        const char* vvi_flag = "F16C_VVI_FLAG";          // TODO  
        const char* aoa_flag = "F16C_AOA_FLAG";          // TODO  
        const char* adi_off = "F16C_ADI_OFF";            // TODO  
        const char* adi_aux = "F16C_ADI_AUX";            // TODO  
        const char* adi_gs = "F16C_ADI_GS";              // TODO  
        const char* adi_loc = "F16C_ADI_LOC";            // TODO  
        const char* hsi_off = "F16C_HSI_OFF";            // TODO  
        const char* hsi_to = "F16C_HSI_TO";              // TODO  
        const char* hsi_from = "F16C_HSI_FROM";          // TODO  
        const char* hsi_ils_warn = "F16C_HSI_ILS_WARN";  // TODO  
        const char* hsi_crs_warn = "F16C_HSI_CRS_WARN";  // TODO  
        const char* hsi_init = "F16C_HSI_INIT";          // TODO  
  
        // InstPanel lights (Eyebrows, TWP etc.)  
        const char* eb_masterC = "F16C_LMP_MASTER_CAUTION"; // TODO  
        const char* eb_tfFail = "F16C_LMP_TF_FAIL";         // TODO  
        const char* twp_handOff = "F16C_LMP_HANDOFF";       // TODO  
        const char* twp_launch = "F16C_LMP_LAUNCH";         // TODO  
        const char* twp_priMode = "F16C_LMP_PRI_MODE";      // TODO  
        const char* twp_unknown = "F16C_LMP_UNK";           // TODO  
        const char* twp_sysTest = "F16C_LMP_SYS_TEST";      // TODO  
        const char* twp_tgtSep = "F16C_LMP_TGT_SEP";        // TODO  
        const char* ecm_on = "F16C_LMP_ECM_CONSENT";        // TODO  
        const char* tfr_engaged = "F16C_LMP_TFR_ENGAGED";   // TODO  
        const char* tfr_stby = "F16C_LMP_TFR_STBY";         // TODO  
        const char* idx_aoaAbove = "F16C_IDX_AOA_ABOVE";    // TODO  
        const char* idx_aoaOn = "F16C_IDX_AOA_ON";          // TODO  
        const char* idx_aoaBelow = "F16C_IDX_AOA_BELOW";    // TODO  
        const char* idx_rdy = "F16C_IDX_RDY";               // TODO  
        const char* idx_arNws = "F16C_IDX_AR_NWS";          // TODO  
        const char* idx_disc = "F16C_IDX_DISC";             // TODO  
        const char* eb_engFire = "F16C_LMP_ENG_FIRE";       // TODO  
        const char* eb_engine = "F16C_LMP_ENGINE";          // TODO  
        const char* eb_hydOil = "F16C_LMP_HYD_OIL";         // TODO  
        const char* eb_flcs = "F16C_LMP_FLCS";              // TODO  
        const char* eb_dbuOn = "F16C_LMP_DBU_ON";           // TODO  
        const char* eb_toldgCfg = "F16C_LMP_T_L_CFG";       // TODO  
        const char* eb_canopy = "F16C_LMP_CANOPY";          // TODO  
        const char* eb_oxyLow = "F16C_LMP_OXY_LOW";         // TODO  
        const char* marker_outer = "F16C_MARKER_OUTER";     // TODO  
        const char* marker_middle = "F16C_MARKER_MIDDLE";   // TODO  
  
        // Left console lights  
        const char* gearHandle = "F16C_GEAR_HANDLE";        // TODO  
        const char* noseDown = "F16C_NLG_DOWN";             // TODO  
        const char* leftDown = "F16C_LLG_DOWN";             // TODO  
        const char* rightDown = "F16C_RLG_DOWN";            // TODO  
        const char* twa_search = "F16C_TWA_SEARCH";         // TODO  
        const char* twa_act = "F16C_TWA_ACT";               // TODO  
        const char* twa_lowAlt = "F16C_TWA_LOW_ALT";        // TODO  
        const char* twa_sysPwr = "F16C_TWA_SYS_PWR";        // TODO  
        const char* jfs_run = "F16C_JFS_RUN";               // TODO  
        const char* epu_run = "F16C_EPU_RUN";               // TODO  
        const char* epu_hyd = "F16C_EPU_HYD";               // TODO  
        const char* epu_air = "F16C_EPU_AIR";               // TODO  
        const char* el_pmg = "F16C_EL_PMG";                 // TODO  
        const char* el_mainGen = "F16C_EL_MAIN_GEN";        // TODO  
        const char* el_stbyGen = "F16C_EL_STBY_GEN";        // TODO  
        const char* el_epuGen = "F16C_EL_EPU_GEN";          // TODO  
        const char* el_epuPmg = "F16C_EL_EPU_PMG";          // TODO  
        const char* el_toFlcs = "F16C_EL_TO_FLCS";          // TODO  
        const char* el_battFail = "F16C_EL_BATT_FAIL";      // TODO  
        const char* el_flcsRly = "F16C_EL_FLCS_RLY";        // TODO  
        const char* avtr_run = "F16C_AVTR_RUN";             // TODO  
        const char* avtr_ctvs = "F16C_AVTR_CTVS";           // TODO  
        const char* fltctl_run = "F16C_FLTCTL_RUN";         // TODO  
        const char* fltctl_fail = "F16C_FLTCTL_FAIL";       // TODO  
        const char* testA = "F16C_TEST_A";                  // TODO  
        const char* testB = "F16C_TEST_B";                  // TODO  
        const char* testC = "F16C_TEST_C";                  // TODO  
        const char* testD = "F16C_TEST_D";                  // TODO  
  
        // UHF/AUX/IFF/CMDS/Trim  
        const char* uhf_preset = "F16C_UHF_PRESET";         // TODO  
        const char* uhf_frequency = "F16C_UHF_FREQ";        // TODO  
        const char* iff_mode_str = "F16C_IFF_BACKUP";       // TODO (aufgeteilt)  
        const char* aux_tacan_band = "F16C_TCN_BAND";       // TODO  
        const char* aux_tacan_chan = "F16C_TCN_CHAN";       // TODO  
        const char* trim_pitch = "F16C_TRIM_PITCH";         // TODO (deg oder %-Skala)  
        const char* trim_roll = "F16C_TRIM_ROLL";           // TODO  
        const char* cmds_chaff = "F16C_CMDS_CHAFF";         // TODO  
        const char* cmds_flare = "F16C_CMDS_FLARE";         // TODO  
        const char* cmds_go = "F16C_CMDS_GO";               // TODO  
        const char* cmds_nogo = "F16C_CMDS_NOGO";           // TODO  
        const char* cmds_degr = "F16C_CMDS_DEGR";           // TODO  
        const char* cmds_rdy = "F16C_CMDS_RDY";             // TODO  
  
        // ADI  
        const char* adi_pitch = "F16C_ADI_PITCH";           // TODO (deg)  
        const char* adi_roll = "F16C_ADI_ROLL";             // TODO (deg)  
        const char* adi_ils_hor = "F16C_ADI_ILS_HOR";       // TODO  
        const char* adi_ils_ver = "F16C_ADI_ILS_VER";       // TODO  
        const char* sideslip = "F16C_SIDESLIP_DEG";         // TODO  
    } keys;  
};  