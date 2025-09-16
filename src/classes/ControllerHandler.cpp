#include "ControllerHandler.h"




ControllerHandler::ControllerHandler(std::string cfgFileName)
{

}

ControllerHandler::~ControllerHandler()
{
}

void ControllerHandler::showControllers()
{
    for (int i = 0; i < contNumber; i++) {
       std::cout << i << ": " << allControllers.at(i).getName() << " portNum: " << allControllers.at(i).getComport() << " baudrate: " << allControllers.at(i).getBaudrate() << std::endl;
    }
}


void ControllerHandler::readControllerComms() {
    for (int i = 0; i < allControllers.size(); i++) {
        allControllers[i].readSerial();
    }
}


std::vector<std::vector<std::string>> ControllerHandler::readConfig(const std::string& filename) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open controller config file: " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ';')) {
            row.push_back(cell);
        }

        data.push_back(row);
    }

    file.close();
    return data;
}


void ControllerHandler::createControllerVector()
{
    // TODO: read this data from config file!
    auto conf = readConfig(CONFIGFILE);

    for (const auto& row : conf) {
        allControllers.emplace_back(Controller(row.at(0), std::stoi(row.at(1)), std::stol(row.at(2))));
    }

    contNumber = (int) allControllers.size();
    
}

void ControllerHandler::setupControllers()
{
    createControllerVector();
    // std::cout << "vector created\n";
    for (int i = 0; i < allControllers.size(); i++) {
        allControllers[i].connect();
    }

}

void ControllerHandler::updateControllers(F16Data* data, F16Data *prevData)
{
    for (int i = 0; i < contNumber; i++) {
        allControllers.at(i).updateController(data, prevData);
    }
}

void ControllerHandler::initControllers(F16Data* data, F16Data* prevData)
{
    for (int i = 0; i < contNumber; i++) {
        allControllers.at(i).initController(data, prevData);
    }
}

