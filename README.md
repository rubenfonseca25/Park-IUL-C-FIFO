# Park-IUL - Client-Server Parking Management System (C)

This project was developed for the **Operating Systems course** at **ISCTE – Instituto Universitário de Lisboa**.

It implements a **client–server parking management system in C**, where a central server manages parking requests and spawns **dedicated server processes** to handle each client individually.

The system simulates a fully automated parking platform capable of processing vehicle check-ins and check-outs while maintaining logs of parking activity.

---

# System Architecture

The system follows a **client–server architecture** composed of three main components:

* **Client process**
* **Main server process**
* **Dedicated server processes**

The interaction between these components is achieved through **inter-process communication mechanisms**, including:

* **Named Pipes (FIFO)**
* **Unix signals**
* **Process creation with `fork()`**

Workflow overview:

1. The **Client** collects vehicle information from the user.
2. The Client sends a parking request to the **Server** through a **FIFO (named pipe)**.
3. The **Server** reads the request and searches for an available parking space.
4. If possible, the Server creates a **Dedicated Server process** using `fork()`.
5. The Dedicated Server manages the entire parking session of that client.
6. Parking activity is recorded in a **binary log file**.

---

# Main Concepts Used

This project applies several key **Operating Systems concepts**, including:

* **Process creation and management** (`fork`, `wait`)
* **Signal handling** (`SIGUSR1`, `SIGUSR2`, `SIGHUP`, `SIGINT`, `SIGALRM`)
* **Inter-process communication**
* **Named Pipes / FIFO**
* **Concurrent processes**
* **Binary file manipulation**
* **Timestamp handling**
* **Dynamic memory allocation**

---

# Project Modules

## `server.c`

The server is responsible for managing parking requests and controlling parking availability.

Main responsibilities:

* Initialize the parking database
* Create the **server FIFO (`server.fifo`)**
* Receive client requests
* Allocate parking spaces
* Spawn **Dedicated Server processes**
* Handle server shutdown
* Manage termination of child processes

The server remains active waiting for requests from clients.

---

## `Dedicated Server Process`

Each parking request is handled by a **dedicated server process**, created using `fork()`.

Responsibilities include:

* Validating client requests
* Confirming parking entry
* Logging vehicle entry and exit
* Monitoring the parked vehicle
* Handling exit requests from the client
* Releasing parking space when the vehicle leaves

---

## `client.c`

The client module handles interaction with the user.

Main responsibilities:

* Collect vehicle information:

  * License plate
  * Country code
  * Vehicle category
  * Driver name
* Send the parking request to the server
* Wait for server confirmation
* Allow the user to terminate the parking session

---

# Data Structures

The project uses several shared data structures defined in `common.h`.

### Vehicle information

```
typedef struct {
    char matricula[10];
    char pais[3];
    char categoria;
    char nomeCondutor[80];
} Viatura;
```

### Parking request

```
typedef struct {
    Viatura viatura;
    int pidCliente;
    int pidServidorDedicado;
} Estacionamento;
```

### Log entry

```
typedef struct {
    Viatura viatura;
    char dataEntrada[17];
    char dataSaida[17];
} LogItem;
```

---

# Parking Logs

Vehicle activity is stored in a **binary log file**:

```
estacionamentos.txt
```

Each entry records:

* Vehicle information
* Entry timestamp
* Exit timestamp

---

### About `common.h` and the `so_utils` library

The `common.h` file contains all the definitions shared between the Client and Server modules, including:

- Data structures: `Viatura`, `Estacionamento`, and `LogItem`  
- Project constants: `MAX_ESPERA`, `FILE_REQUESTS`, `FILE_LOGFILE`, `DISPONIVEL`, `INVALID`  
- Function prototypes for Server, Dedicated Server, and Client modules  
- Common includes for signals, processes, time, and file handling

`common.h` acts as **the contract between the client and server**, ensuring that both use the same structures, functions, and constants.

#### Attention to the `so_utils.h` path

Currently, `common.h` references the `so_utils.h` library via an **absolute path**:

```c
#include "/home/so/utils/include/so_utils.h"

---

# Technologies Used

* **C programming language**
* **Unix/Linux system calls**
* **Signals**
* **Named pipes (FIFO)**
* **Binary file operations**
* **Process management**

---

# Learning Objectives

This project was designed to reinforce key **Operating Systems topics**, including:

* Process concurrency
* Signal-based synchronization
* Inter-process communication
* System-level programming in C
* File system interaction

---

# Author

**Rúben Fonseca**

Developed as part of the **Operating Systems course** at
**ISCTE – Instituto Universitário de Lisboa**.
