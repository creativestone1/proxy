# Tunnel for OpenVPN TCP with Traffic Obfuscation

A program that creates a tunnel for OpenVPN TCP with the ability to change the traffic obfuscation algorithm.  
It allows proxying traffic through OpenVPN, providing an additional layer of protection and concealment of data transmission.

---

## Description

This project implements a TCP tunnel that can be used alongside OpenVPN.  
Its main feature is the ability to write your own traffic obfuscation algorithms in MessageProcessors.cpp to bypass blocks and packet inspection attempts.  
The program accepts incoming traffic and routes it through the OpenVPN proxy, "hiding" the content using obfuscation algorithms.

---

## Features

- Creates a TCP tunnel for OpenVPN
- Supports multiple traffic obfuscation algorithm
- Ensures privacy and circumvents restrictions

---

## Installation (Linux)

1. Clone the repository:  

git clone <your-repo-URL>

2. Build the program (if needed):
- create build directory(if it does not exist)
```bash
mkdir build
```

- build into "build" directory
```bash
cd build
cmake ../
make
```

3. Make sure OpenVPN is installed on your system.

---

## Usage

./proxy


- In the configuration file (config.txt), specify tunnel parameters
- Example configuration:
```config.txt
[Host]
CLIENT

[Connect]
remoteIp=127.0.0.1
remotePort=5001

[Accept]
localPort=7001
connectionsCount=1
```
---

## Supported Obfuscation Algorithms

- Transparent
- Custom (support for user-defined scripts)

---

## Contact and Support

If you have questions or suggestions, please open an issue or write to creativestone1@mail.ru

---

## License

This project is licensed under the MIT License. See the LICENSE file for details.
