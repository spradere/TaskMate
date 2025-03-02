# TaskMate RTOS
## AVR preemptive real time oprating system
 
### intro Introduction

TaskMate is a lightweight RTOS for AVR microcontrollers with a focus on reliability, real-time, and modularity.

Each task have it's own stack, used to save and restore context for task switching


### Installation

* Compile the source code using # make all
* Upload the firmware to the ATmega2560 using # make upload.


### RTC Real Time Clock

Each task have one 16 bits timer/counter sycronized wtih other tasks.

If not zero the counter is decremented at 10 ms rate.
 

### Features

* Hybrid multitasking (cooperative & preemptive)
* Dynamic driver & task  management
* Real-time clock (RTC) support
* Serial command-line interface (CLI)
 
  
### Layers
 
* 4 - user task & system services
* 3 - system call 
* 2 - system core (scheduler & RTC)
* 1 - drivers
* 0 - hardware

![System Layer Diagram](TaskMate_layers.png)

### License
 
TaskMate is not licensed at all. For the moment.

