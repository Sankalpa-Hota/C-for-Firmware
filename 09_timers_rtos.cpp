/*
===============================================================================
Purpose: Demonstrate Timers, Delays, and RTOS concepts in firmware using C++
Author: Sankalpa Hota
How to compile:
  g++ 09_timers_rtos.cpp -o timers_rtos_demo -std=c++11 -pthread
  ./timers_rtos_demo

This simulation teaches:
- Timer interrupts
- Task scheduling (RTOS)
- Task priorities
===============================================================================
*/

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
using namespace std;

// -----------------------------------------------------------------------------
// SECTION 1: GLOBAL FLAGS AND MUTEX
// -----------------------------------------------------------------------------
atomic<bool> timerFlag(false);      // Flag set by timer ISR
mutex uartMutex;                     // Simulate UART shared resource

// -----------------------------------------------------------------------------
// SECTION 2: ISR SIMULATION (TIMER)
// -----------------------------------------------------------------------------
void timerISR() {
    cout << "[TIMER ISR] Timer event occurred!" << endl;
    timerFlag = true; // Set flag for main task
}

// -----------------------------------------------------------------------------
// SECTION 3: TASKS (RTOS SIMULATION)
// -----------------------------------------------------------------------------
void ledTask() {
    for(int i=0;i<5;i++) {
        cout << "[LED TASK] Toggling LED..." << endl;
        this_thread::sleep_for(chrono::milliseconds(500)); // Simulate work
    }
}

void uartTask() {
    for(int i=0;i<3;i++) {
        lock_guard<mutex> lock(uartMutex); // Simulate resource protection
        cout << "[UART TASK] Sending data over UART..." << endl;
        this_thread::sleep_for(chrono::milliseconds(700)); // Simulate work
    }
}

void mainRTOSLoop() {
    int loopCounter = 0;
    while(loopCounter < 10) { // Limit for demo
        // Check timer flag
        if(timerFlag) {
            cout << "[MAIN LOOP] Handling Timer Event" << endl;
            timerFlag = false;
        }

        // Simulate main RTOS scheduler tasks
        this_thread::sleep_for(chrono::milliseconds(200));
        loopCounter++;
    }
}

// -----------------------------------------------------------------------------
// SECTION 4: SIMULATED TIMER INTERRUPT
// -----------------------------------------------------------------------------
void simulateHardwareTimer() {
    for(int i=0;i<3;i++) {
        this_thread::sleep_for(chrono::milliseconds(600)); // Timer interval
        timerISR();
    }
}

// -----------------------------------------------------------------------------
// SECTION 5: MAIN FUNCTION
// -----------------------------------------------------------------------------
int main() {
    cout << "==== Firmware Timers & RTOS Simulation ====" << endl;

    // Start tasks (threads)
    thread led_thread(ledTask);       // LED task
    thread uart_thread(uartTask);     // UART task
    thread timer_thread(simulateHardwareTimer); // Timer interrupt
    thread rtos_loop(mainRTOSLoop);  // RTOS main loop

    // Join threads for demo purposes
    led_thread.join();
    uart_thread.join();
    timer_thread.join();
    rtos_loop.join();

    cout << "==== Demo Complete ====" << endl;
    return 0;
}

/*
===============================================================================
Key Concepts Demonstrated:

1. Timer Interrupt:
   - ISR sets flag
   - Main loop reacts when flag is set
   - Simulates hardware timer triggering periodic events

2. Tasks (Threads):
   - Each thread simulates a separate RTOS task
   - Tasks can run concurrently
   - Mutex protects shared resources (UART)

3. Main RTOS Loop:
   - Scheduler checks flags and executes tasks
   - Mimics priority-based task execution

4. atomic & mutex:
   - Safe communication between ISR and tasks
   - Prevent race conditions
===============================================================================
*/
