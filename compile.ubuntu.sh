#!/usr/bin/bash

# Verifica se nlohmann-json3-dev è già installato
if dpkg -s nlohmann-json3-dev &>/dev/null
then
    echo "nlohmann-json3-dev è già installato: salto l'installazione."
else
    echo "Installo nlohmann-json3-dev..."
    sudo apt-get update -y
    sudo apt-get install nlohmann-json3-dev -y
fi

# Compila il file C++
g++ -std=c++17 server_manager.cpp -o server_manager
echo "Compilazione completata!"
