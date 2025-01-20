# Minecraft Server Manager in C++

This project provides a simple **manager** for Minecraft servers written in C++, leveraging **screen** sessions to start, stop, and check the status of the servers.

## Main Features
- Reads configuration from a JSON file.
- Dynamically assigns RAM (in MB) to servers, generating `-Xmx` and `-Xms` Java arguments automatically.
- Allows for additional custom Java arguments.
- Uses **screen** to run servers in **detached mode**, enabling:
  - Status checks to see if a server is running.
  - Stopping a server by sending the `stop` command.
- Includes a basic **CLI menu** for interactive management.

## Requirements
- **C++17** or later (e.g., `g++` or `clang++`).
- [nlohmann/json](https://github.com/nlohmann/json) library. On Debian/Ubuntu:
  ```bash
  sudo apt-get install nlohmann-json3-dev
  ```
  On Arch Linux:
  ```bash
  sudo pacman -S nlohmann-json
  ```
  *(Or download the single `json.hpp` file from the official repository.)*
- **screen** installed on your system:
  ```bash
  sudo apt-get install screen
  ```
  or
  ```bash
  sudo pacman -S screen
  ```

## JSON Structure Example

An example `servers.json` file:

```json
[
  {
    "name": "Server1",
    "ram": 2048,
    "dir": "/home/minecraft/server1",
    "jar": "server.jar",
    "javaArgs": "-Dfile.encoding=UTF-8 -XX:+UseG1GC"
  },
  {
    "name": "Server2",
    "ram": 4096,
    "dir": "/home/minecraft/server2",
    "jar": "server.jar",
    "javaArgs": ""
  }
]
```

- **name**: The server name, also used as the `screen` session name.  
- **ram**: Amount of RAM in MB (e.g., `2048` = 2GB).  
- **dir**: Directory containing the Minecraft server files.  
- **jar**: Minecraft server JAR file name.  
- **javaArgs**: Additional Java arguments (optional).

## Compilation
Make sure the required packages (see **Requirements**) are installed.  
Then, in the project directory, run:

```bash
g++ -std=c++17 server_manager.cpp -o server_manager
```

If successful, an executable named `server_manager` will be created.

## Usage
Run the manager with:

```bash
./server_manager servers.json
```

Ensure that your `servers.json` contains the desired configuration.

## Menu Overview
Upon starting, the program displays a CLI menu with the following options:

1. **List Servers**  
   Shows the list of servers loaded from the JSON file and their parameters.  
2. **Start Server**  
   Prompts for the server name. Checks if it is already running and, if not, starts it in a **screen** session.  
3. **Stop Server**  
   Prompts for the server name. If running, sends the `stop` command to the console and closes the **screen** session.  
4. **Check if a Server is Running**  
   Prompts for the server name and uses **screen** to check if the server is active or not.  
5. **Exit**  
   Exits the program.

### Attaching to a Server Console
If you want to directly interact with the server console (beyond the menu’s stop function), use:
```bash
screen -r <server_name>
```
Detach from the session by pressing `Ctrl+A`, then `D`.

## Customizations
- **Logging**: To redirect the server output to a file, modify the `startServer` function line:
  ```cpp
  cmd << "screen -dmS " << server.name
      << " bash -c \"cd " << server.dir
      << " && java " << javaOpts.str()
      << " -jar " << server.jar
      << " nogui > server.log 2>&1\"";
  ```
- **Sending Custom Commands**: Create a function similar to `stopServer` to send any console command (e.g., `/say Hello!`).

## Conclusion
This project provides a foundation for managing Minecraft servers on Linux using C++ and **screen**. Feel free to extend it with additional features (log management, automatic backups, custom commands, etc.) to suit your needs. Have fun!