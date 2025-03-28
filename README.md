# TaskMate RTOS
## AVR preemptive real time oprating system

> **TaskMate Project Stats (v0.10)**  
>  
> 71 commit, 38 source files in 6 directories, 2,394 lines of code, 2752 bytes in flash. 


### ▶️ Introduction

**TaskMate** is a lightweight RTOS for AVR microcontrollers with a focus on reliability, real-time, and modularity.

No external RTOS used — 100% built from scratch.  
 
Each task have it's own stack, used to save and restore context for task switching.


### ⚙️ Installation

Custom POSIX Makefile with auto-deps and all project workflow management.

* See : [Makefile Features & Usage](doc/Makefile_summary.md)


### ♻️ RTC Real Time Clock

Each task have one 16 bits timer/counter sycronized with other tasks.

If not zero, the counter is decremented at 10 ms rate.
 

### ✔️ Features

* Hybrid multitasking (cooperative & preemptive)
* Dynamic driver & task  management
* Real-time clock (RTC) support
* Serial command-line interface (CLI)
 
  
### ⤴️ Layers
 
![System Layer Diagram](doc/TaskMate_layers_v2.png)


### ➡️ Project progress ...

* See : [Road map](doc/check_list.md)

### ️⚠ ️License

This software is distributed under the **TaskMate License v1.0**.

* Free for **non-commercial use** under conditions described in the `LICENSE` file.
* Commercial use requires a **separate paid license**.

To inquire about commercial licensing, please open an issue in this repository:  
[Open Licensing Issue](https://codeberg.org/Doul09/TaskMate/issues)

By using this software, you agree to the terms of the TaskMate License v1.0.

See the `LICENSE` file for full details.

