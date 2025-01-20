#!/usr/bin/bash

# Verifica se nlohmann-json è già installato
if pacman -Qi nlohmann-json &> /dev/null
then
    echo "nlohmann-json è già installato: salto l'installazione."
else
    echo "Installo nlohmann-json..."
    sudo pacman -Syy nlohmann-json --noconfirm
fi

# Compila il file C++
g++ -std=c++17 server_manager.cpp -o server_manager
echo "Compilazione completata!"
