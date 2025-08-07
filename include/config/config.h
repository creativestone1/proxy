#pragma once
#include <fstream>
#include "ConnectionCreator.h"
#include "ProxySessionHandler.h"
class Config {
public:
    HostType hostType;
    std::vector<ConnectConfig> connectConfigs;
    std::vector<AcceptConfig> acceptConfigs;

    bool loadFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return false;

        std::string line;
        ConnectConfig tempConnect;
        AcceptConfig tempAccept;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;

            if (line == "[Host]") {
                std::getline(file, line);
                hostType = (line == "CLIENT") ? CLIENT : SERVER;
            } else if (line == "[Connect]") {
                tempConnect = ConnectConfig{};
                while (std::getline(file, line) && !line.empty()) {
                    auto kv = parseKeyValue(line);
                    if (kv.first == "remoteIp") tempConnect.remoteIp = kv.second;
                    else if (kv.first == "remotePort") tempConnect.remotePort = std::stoi(kv.second);
                    else if (kv.first == "localPort") tempConnect.localPort = std::stoi(kv.second);
                    else if (kv.first == "connectionTryTimes") tempConnect.connectionTryTimes = std::stoi(kv.second);
                    else if (kv.first == "tryType") tempConnect.tryType = parseTryType(kv.second);
                }
                connectConfigs.push_back(tempConnect);
            } else if (line == "[Accept]") {
                tempAccept = AcceptConfig{};
                while (std::getline(file, line) && !line.empty()) {
                    auto kv = parseKeyValue(line);
                    if (kv.first == "localPort") tempAccept.localPort = std::stoi(kv.second);
                    else if (kv.first == "connectionsCount") tempAccept.connectionsCount = std::stoi(kv.second);
                }
                acceptConfigs.push_back(tempAccept);
            }
        }
        return true;
    }

    bool saveToFile(const std::string& path) {
        std::ofstream file(path);
        if (!file.is_open()) return false;

        file << "[Host]\n";
        file << (hostType == CLIENT ? "CLIENT\n" : "SERVER\n");
        file << "\n";

        for (const auto& cfg : connectConfigs) {
            file << "[Connect]\n";
            file << "remoteIp=" << cfg.remoteIp << "\n";
            file << "remotePort=" << cfg.remotePort << "\n";
            file << "localPort=" << cfg.localPort << "\n";
            file << "connectionTryTimes=" << cfg.connectionTryTimes << "\n";
            file << "tryType=" << tryTypeToString(cfg.tryType) << "\n\n";
        }

        for (const auto& cfg : acceptConfigs) {
            file << "[Accept]\n";
            file << "localPort=" << cfg.localPort << "\n";
            file << "connectionsCount=" << cfg.connectionsCount << "\n\n";
        }

        return true;
    }

private:
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t");
        auto end = s.find_last_not_of(" \t");
        return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    }

    static std::pair<std::string, std::string> parseKeyValue(const std::string& line) {
        auto pos = line.find('=');
        if (pos == std::string::npos) return {"", ""};
        auto key = trim(line.substr(0, pos));
        auto value = trim(line.substr(pos + 1));
        return {key, value};
    }

    static std::string tryTypeToString(TryType t) {
        switch (t) {
            case TRY_ONCE: return "TRY_ONCE";
            case TRY_MULTIPLE_TIMES: return "TRY_MULTIPLE_TIMES";
            case TRY_INFINITE: return "TRY_INFINITE";
        }
        return "UNKNOWN";
    }

    static TryType parseTryType(const std::string& str) {
        if (str == "TRY_ONCE") return TRY_ONCE;
        if (str == "TRY_MULTIPLE_TIMES") return TRY_MULTIPLE_TIMES;
        return TRY_INFINITE;
    }
};

