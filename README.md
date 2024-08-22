<p align="center">
  <img src="https://cdn-icons-png.flaticon.com/512/6295/6295417.png" width="100" />
</p>
<p align="center">
    <h1 align="center">OS_PROJECT</h1>
</p>
<p align="center">
    <em><code>► A comprehensive project featuring multithreaded system simulations and a virtual memory simulation in C</code></em>
</p>
<p align="center">
	<img src="https://img.shields.io/github/license/ParamBhaskar/OS_Project?style=flat&color=0080ff" alt="license">
	<img src="https://img.shields.io/github/last-commit/ParamBhaskar/OS_Project?style=flat&logo=git&logoColor=white&color=0080ff" alt="last-commit">
	<img src="https://img.shields.io/github/languages/top/ParamBhaskar/OS_Project?style=flat&color=0080ff" alt="repo-top-language">
	<img src="https://img.shields.io/github/languages/count/ParamBhaskar/OS_Project?style=flat&color=0080ff" alt="repo-language-count">
<p>
<p align="center">
		<em>Developed with the software and tools below.</em>
</p>
<p align="center">
	<img src="https://img.shields.io/badge/C-A8B9CC.svg?style=flat&logo=C&logoColor=black" alt="C">
</p>
<hr>

##  Quick Links

> - [Overview](#overview)
> - [Features](#features)
> - [Repository Structure](#repository-structure)
> - [Modules](#modules)
> - [Getting Started](#getting-started)
>   - [Installation](#installation)
>   - [Running OS_Project](#running-os_project)
> - [Contributing](#contributing)
> - [Acknowledgments](#acknowledgments)

---

##  Overview

This project consists of three main components: **my custom Thread Library**, a **Multithreaded Doctor's Office Simulation** and a **Virtual Memory Simulation** demonstrating pure demand paging.

### Custom Thread Library
- **Technology**: C using Semaphores and Memory Barriers
- **Features**:
  - Built at the user level.
  - Mimics pthread functionalities.

### Multithreaded Doctor's Office Simulation
- **Technology**: C using pthreads
- **Features**:
  - Priority-based scheduling for different visitor types.
  - Management of concurrent threads.
  - Ensured synchronization for optimal performance.

### Virtual Memory Simulation
- **Architecture**: Master-Slave
- **Features**:
  - Process scheduling and memory management.
  - Page replacement using the Least Recently Used (LRU) algorithm.
  - Accurate handling of page faults and invalid references.

---

##  Features

- **Custom Thread Library**
  - Built at the user level using Semaphores and Memory Barriers.
  - Mimics pthread functionalities.

- **Multithreaded Doctor's Office Simulation**
  - Implemented in C using pthreads.
  - Features priority-based scheduling and thread synchronization.

- **Virtual Memory Simulation**
  - Demonstrates pure demand paging.
  - Focuses on accurate page fault handling and invalid references.
  - Utilizes a master-slave architecture for effective process scheduling and memory management.


---

##  Repository Structure

```sh
└── OS_Project/
    ├── Mutithreaded Doctor's Office Simulation
    │   ├── arrival.c
    │   ├── arrival.txt
    │   ├── event.c
    │   ├── event.h
    │   ├── event.o
    │   ├── makefile
    │   ├── session
    │   └── session.c
    ├── README.md
    ├── Virtual Memory Simulation
    │   ├── makefile
    │   ├── master.c
    │   ├── mmu.c
    │   ├── process.c
    │   ├── result.txt
    │   └── sched.c
    └── foothread - My Custom Thread Library
        ├── computesum.c
        ├── foothread.c
        ├── foothread.h
        ├── gentree.c
        └── makefile
```

---

##  Modules

<details closed><summary>foothread - My Custom Thread Library</summary>

| File                                                                                                                     | Summary                         |
| ------------------------------------------------------------------------------------------------------------------------ | ------------------------------- |
| [foothread.c](https://github.com/ParamBhaskar/OS_Project/blob/main/foothread%20-%20My%20Custom%20Thread%20Library/foothread.c)   | <code>► Contains the implementation of the custom thread library.</code> |
| [foothread.h](https://github.com/ParamBhaskar/OS_Project/blob/main/foothread%20-%20My%20Custom%20Thread%20Library/foothread.h)   | <code>► Header file for the custom thread library.</code> |
| [computesum.c](https://github.com/ParamBhaskar/OS_Project/blob/main/foothread%20-%20My%20Custom%20Thread%20Library/computesum.c) | <code>► Example program to compute tree sum using custom threads.</code> |
| [gentree.c](https://github.com/ParamBhaskar/OS_Project/blob/main/foothread%20-%20My%20Custom%20Thread%20Library/gentree.c)       | <code>► Generates a tree structure for testing the thread library.</code> |
| [makefile](https://github.com/ParamBhaskar/OS_Project/blob/main/foothread%20-%20My%20Custom%20Thread%20Library/makefile)         | <code>► Example program to compute sum using custom threads.</code> |

</details>

<details closed><summary>Multithreaded Doctor's Office Simulation</summary>

| File                                                                                                                      | Summary                         |
| ------------------------------------------------------------------------------------------------------------------------- | ------------------------------- |
| [arrival.c](https://github.com/ParamBhaskar/OS_Project/blob/main/Mutithreaded%20Doctor's%20Office%20Simulation/arrival.c)     | <code>► Manages visitor arrivals.</code> |
| [arrival.txt](https://github.com/ParamBhaskar/OS_Project/blob/main/Mutithreaded%20Doctor's%20Office%20Simulation/arrival.txt) | <code>► Sample arrival data for simulation.</code> |
| [event.c](https://github.com/ParamBhaskar/OS_Project/blob/main/Mutithreaded%20Doctor's%20Office%20Simulation/event.c)         | <code>► Implements event handling functions.</code> |
| [event.h](https://github.com/ParamBhaskar/OS_Project/blob/main/Mutithreaded%20Doctor's%20Office%20Simulation/event.h)         | <code>► Header file for event handling.</code> |
| [makefile](https://github.com/ParamBhaskar/OS_Project/blob/main/Mutithreaded%20Doctor's%20Office%20Simulation/makefile)       | <code>► Makefile to build the simulation.</code> |
| [session.c](https://github.com/ParamBhaskar/OS_Project/blob/main/Mutithreaded%20Doctor's%20Office%20Simulation/session.c)     | <code>► Manages session activities.</code> |

</details>

<details closed><summary>Virtual Memory Simulation</summary>

| File                                                                                                      | Summary                         |
| --------------------------------------------------------------------------------------------------------- | ------------------------------- |
| [master.c](https://github.com/ParamBhaskar/OS_Project/blob/main/Virtual%20Memory%20Simulation/master.c)     | <code>► Coordinates the master-slave architecture.</code> |
| [mmu.c](https://github.com/ParamBhaskar/OS_Project/blob/main/Virtual%20Memory%20Simulation/mmu.c)           | <code>► Manages memory unit operations.</code> |
| [process.c](https://github.com/ParamBhaskar/OS_Project/blob/main/Virtual%20Memory%20Simulation/process.c)   | <code>► Implements process behaviors.</code> |
| [result.txt](https://github.com/ParamBhaskar/OS_Project/blob/main/Virtual%20Memory%20Simulation/result.txt) | <code>► Contains the simulation results.</code> |
| [sched.c](https://github.com/ParamBhaskar/OS_Project/blob/main/Virtual%20Memory%20Simulation/sched.c)       | <code>► Handles process scheduling tasks.</code> |
| [makefile](https://github.com/ParamBhaskar/OS_Project/blob/main/Virtual%20Memory%20Simulation/makefile)     | <code>► Makefile to build the virtual memory simulation.</code> |

</details>

---

##  Getting Started

***Requirements***

Ensure you have the following dependencies installed on your system:

* **C**: `version above C11`

###  Installation

1. Clone the OS_Project repository:

```sh
git clone https://github.com/ParamBhaskar/OS_Project
```

2. Change to the project directory:

```sh
cd OS_Project
```
###  Running OS_Project

###  Running foothread - My Custom Thread Library

Use the following command to build and test foothread:

1. Go to the project directory:

```sh
cd foothread\ -\ My\ Custom\ Thread\ Library/
```

2. Build foothread library:

```sh
make
```

3. Compile gentree.c:
```sh
make tree
```
4. Run ./gentree to build tree.txt:
```sh
./gentree
```
5. Generate and run ./computesum to test our custom thread library:
```sh
make run
```
![image](https://github.com/ParamBhaskar/OS_Project/assets/108367037/f2a6352b-1bb2-4b3b-b2f4-294188079dd1)

###  Running Mutithreaded Doctor's Office Simulation

1. Go to the project directory:

```sh
cd Mutithreaded\ Doctor\'s\ Office\ Simulation/
```

2. To run the simulation:

```sh
make all
```
![os1](https://github.com/ParamBhaskar/OS_Project/assets/108367037/247bb539-bdd9-4ee2-a777-e77377d5f01d)
![os2](https://github.com/ParamBhaskar/OS_Project/assets/108367037/8b94c6fe-e7fc-4679-9ed5-d5111684e73b)

###  Running Virtual Memory Simulation

Use the following command to build and test foothread:

1. Go to the project directory:

```sh
cd Virtual\ Memory\ Simulation/
```

2. Build and Run the Simulation:

```sh
make
```
![image](https://github.com/ParamBhaskar/OS_Project/assets/108367037/f15f832b-0ad9-4405-8fc4-9b287727e2a0)
![image](https://github.com/ParamBhaskar/OS_Project/assets/108367037/47cb4c51-4bd6-44d5-8288-6fd1b94f5176)

###  Cleaning .o files and IPCs used in System in any of the three Project Directory

```sh
make clean
```

---

##  Contributing

Contributions are welcome! Here are several ways you can contribute:

- **[Submit Pull Requests](https://github.com/ParamBhaskar/OS_Project/blob/main/CONTRIBUTING.md)**: Review open PRs, and submit your own PRs.
- **[Join the Discussions](https://github.com/ParamBhaskar/OS_Project/discussions)**: Share your insights, provide feedback, or ask questions.
- **[Report Issues](https://github.com/ParamBhaskar/OS_Project/issues)**: Submit bugs found or log feature requests for Os_project.

<details closed>
    <summary>Contributing Guidelines</summary>

1. **Fork the Repository**: Start by forking the project repository to your GitHub account.
2. **Clone Locally**: Clone the forked repository to your local machine using a Git client.
   ```sh
   git clone https://github.com/ParamBhaskar/OS_Project
   ```
3. **Create a New Branch**: Always work on a new branch, giving it a descriptive name.
   ```sh
   git checkout -b new-feature-x
   ```
4. **Make Your Changes**: Develop and test your changes locally.
5. **Commit Your Changes**: Commit with a clear message describing your updates.
   ```sh
   git commit -m 'Implemented new feature x.'
   ```
6. **Push to GitHub**: Push the changes to your forked repository.
   ```sh
   git push origin new-feature-x
   ```
7. **Submit a Pull Request**: Create a PR against the original project repository. Clearly describe the changes and their motivations.

Once your PR is reviewed and approved, it will be merged into the main branch.

</details>

---


##  Acknowledgments

- Special thanks to Prof. Abhijit Das and Prof. Bivas Mitra for guidance and mentorship throughout the project.


[**Return**](#quick-links)

---
