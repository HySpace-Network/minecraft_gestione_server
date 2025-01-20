#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <limits>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct ServerConfig {
    std::string name;
    int ram;
    std::string dir;
    std::string jar;
    std::string javaArgs;
};

// ======================
//    PROTOTIPI
// ======================

bool isServerRunning(const std::string &serverName);
std::vector<ServerConfig> loadServers(const std::string &jsonPath);
bool startServer(const ServerConfig &server);
bool stopServer(const std::string &serverName);

// ======================
//    DEFINIZIONI
// ======================

// Carica i server dal file JSON
std::vector<ServerConfig> loadServers(const std::string &jsonPath) {
    std::vector<ServerConfig> servers;

    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        std::cerr << "Impossibile aprire il file " << jsonPath << std::endl;
        return servers;
    }

    json j;
    file >> j;

    for (auto &elem : j) {
        ServerConfig sc;
        sc.name     = elem.value("name", "");
        sc.ram      = elem.value("ram", 1024);
        sc.dir      = elem.value("dir", "");
        sc.jar      = elem.value("jar", "");
        sc.javaArgs = elem.value("javaArgs", "");
        servers.push_back(sc);
    }

    return servers;
}

bool isServerRunning(const std::string &serverName) {
    FILE *pipe = popen("screen -list", "r");
    if (!pipe) {
        std::cerr << "Impossibile eseguire screen -list!\n";
        return false;
    }

    char buffer[256];
    bool running = false;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        if (line.find(serverName) != std::string::npos) {
            running = true;
            break;
        }
    }

    pclose(pipe);
    return running;
}

bool startServer(const ServerConfig &server) {
    if (isServerRunning(server.name)) {
        std::cerr << "Server " << server.name << " e' gia' avviato!\n";
        return false;
    }

    std::ostringstream javaOpts;
    javaOpts << "-Xmx" << server.ram << "M "
             << "-Xms" << server.ram << "M ";
    if (!server.javaArgs.empty()) {
        javaOpts << server.javaArgs << " ";
    }

    std::ostringstream cmd;
    cmd << "screen -dmS " << server.name
        << " bash -c \"cd " << server.dir
        << " && java " << javaOpts.str()
        << " -jar " << server.jar
        << " nogui\"";

    std::cout << "Avvio del server " << server.name << "...\n";
    int ret = std::system(cmd.str().c_str());
    return (ret == 0);
}

bool stopServer(const std::string &serverName) {
    if (!isServerRunning(serverName)) {
        std::cerr << "Il server " << serverName << " non risulta avviato!\n";
        return false;
    }

    std::ostringstream cmd;
    cmd << "screen -S " << serverName << " -p 0 -X stuff \"stop\\n\"";

    int ret = std::system(cmd.str().c_str());
    if (ret != 0) {
        std::cerr << "Errore nell'invio del comando stop al server " << serverName << "\n";
        return false;
    }

    std::cout << "Comando stop inviato a " << serverName << ". Attendere qualche secondo...\n";
    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <path_al_file_JSON>\n";
        return 1;
    }

    std::vector<ServerConfig> servers = loadServers(argv[1]);
    if (servers.empty()) {
        std::cerr << "Nessun server trovato nel file JSON.\n";
        return 1;
    }

    while (true) {
        std::cout << "\n--- Minecraft Server Manager ---\n"
                  << "Scegli un'opzione:\n"
                  << "1) Lista server\n"
                  << "2) Avvia server\n"
                  << "3) Ferma server\n"
                  << "4) Verifica se un server e' in esecuzione\n"
                  << "5) Esci\n"
                  << ">> ";

        int scelta;
        std::cin >> scelta;
        if (!std::cin) {
            std::cerr << "Input non valido, riprova.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (scelta) {
            case 1: {
                std::cout << "Elenco Server:\n";
                for (auto &s : servers) {
                    std::cout << " - " << s.name
                              << " (RAM: " << s.ram << "MB, Dir: " << s.dir
                              << ", Jar: " << s.jar
                              << ", JavaArgs: [" << s.javaArgs << "])\n";
                }
                break;
            }
            case 2: {
                std::cout << "Inserisci il nome del server da avviare: ";
                std::string name;
                std::cin >> name;

                bool found = false;
                for (auto &s : servers) {
                    if (s.name == name) {
                        found = true;
                        startServer(s);
                        break;
                    }
                }
                if (!found) {
                    std::cerr << "Server \"" << name << "\" non trovato!\n";
                }
                break;
            }
            case 3: {
                std::cout << "Inserisci il nome del server da fermare: ";
                std::string name;
                std::cin >> name;
                stopServer(name);
                break;
            }
            case 4: {
                std::cout << "Inserisci il nome del server da verificare: ";
                std::string name;
                std::cin >> name;
                bool running = isServerRunning(name);
                if (running) {
                    std::cout << "Il server \"" << name << "\" e' in esecuzione.\n";
                } else {
                    std::cout << "Il server \"" << name << "\" e' fermo.\n";
                }
                break;
            }
            case 5:
                std::cout << "Uscita dal manager.\n";
                return 0;
            default:
                std::cerr << "Scelta non valida!\n";
                break;
        }
    }

    return 0;
}
