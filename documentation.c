/**
 * @mainpage TaskMate RTOS
 *
 * @section intro Introduction
 * TaskMate is a lightweight RTOS for AVR microcontrollers with a focus
 * on reliability, real-time, and modularity.
 * 
 * Each task have it's own stack, used to save and restore context for task switching
 *
 * @section installation Installation
 * 1. Compile the source code using # make all
 * 2. Upload the firmware to the ATmega2560 using # make upload.
 *
 * @section RTC Real Time Clock
 * Each task have one 16 bits timer/counter sycronized wtih other tasks.
 * If not zero the counter is decremented at 10 ms rate.
 * 
 * @section features Features
 * - Hybrid multitasking (cooperative & preemptive)
 * - Dynamic driver & task  management
 * - Real-time clock (RTC) support
 * - Serial command-line interface (CLI)
 * 
 * @section Layers
 * 
 * @image html TaskMate_layers.png "TaskMate System Layer Diagram"
 * 
 * @section license License
 * TaskMate is not licensed at all. For the moment.
 * 
 * @author Doul
 * @date 2025/02
 */

