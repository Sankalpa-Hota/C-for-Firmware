/*
===============================================================================
Purpose: Simulate Interrupts and ISRs in C++ for firmware learning.
         Demonstrates:
         - ISR simulation using threads
         - Flag-based communication
         - Timer-based interrupt simulation
Author: Sankalpa Hota
How to compile:
  g++ 08_interrupts_isrs.cpp -o interrupts_demo -std=c++11 -pthread
  ./interrupts_demo

This code teaches:
- How firmware responds to asynchronous events
- How ISRs communicate with the main program
- How to safely simulate hardware interrupts in C++
===============================================================================
*/

#include <iostream>     // For console input/output
#include <thread>       // For creating threads to simulate asynchronous events
#include <atomic>       // For atomic flags (safe communication between threads)
#include <chrono>       // For timing and delays
using namespace std;

// -----------------------------------------------------------------------------
// SECTION 1: GLOBAL FLAGS
// -----------------------------------------------------------------------------
/*
  In firmware, ISRs and main program often share a flag or variable.
  atomic<bool> ensures thread-safe access (no race condition).
*/
atomic<bool> buttonPressed(false);   // Flag set by ISR when button is pressed
atomic<bool> timerElapsed(false);    // Flag set by ISR when timer interrupt occurs

// -----------------------------------------------------------------------------
// SECTION 2: INTERRUPT SERVICE ROUTINES (ISR)
// -----------------------------------------------------------------------------
/*
  ISR for button press.
  In real firmware, the CPU jumps here automatically when the button is pressed.
  In this simulation, we call it from a separate thread.
*/
void buttonISR() {
    cout << "[ISR] Button Press Detected!" << endl;
    buttonPressed = true; // Set the flag for main program
}

/*
  ISR for timer overflow.
  In real firmware, hardware timer triggers this.
*/
void timerISR() {
    cout << "[ISR] Timer Interrupt Triggered!" << endl;
    timerElapsed = true; // Set the flag for main program
}

// -----------------------------------------------------------------------------
// SECTION 3: MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
/*
  This simulates the main firmware loop.
  It continuously checks flags set by ISRs and handles tasks.
  Heavy processing is done here instead of inside ISR.
*/
void mainLoop() {
    int loopCounter = 0;

    while (loopCounter < 10) {  // Limit for demo purposes
        // Handle button press event
        if (buttonPressed) {
            cout << "[MAIN LOOP] Handling Button Press" << endl;
            buttonPressed = false; // Clear flag after handling
        }

        // Handle timer event
        if (timerElapsed) {
            cout << "[MAIN LOOP] Handling Timer Event" << endl;
            timerElapsed = false; // Clear flag after handling
        }

        // Simulate other main tasks
        cout << "[MAIN LOOP] Performing regular tasks..." << endl;

        this_thread::sleep_for(chrono::milliseconds(200)); // Simulate task delay
        loopCounter++;
    }
}

// -----------------------------------------------------------------------------
// SECTION 4: SIMULATED HARDWARE INTERRUPTS
// -----------------------------------------------------------------------------
/*
  These functions simulate asynchronous hardware events by using threads.
  - Button press occurs after 500ms
  - Timer interrupt occurs every 300ms
*/

// Simulate a button press after a delay
void simulateButtonPress() {
    this_thread::sleep_for(chrono::milliseconds(500)); // Wait 500ms
    buttonISR(); // Call ISR as if hardware triggered
}

// Simulate a timer interrupt periodically
void simulateTimerInterrupt() {
    for (int i = 0; i < 3; i++) { // Simulate 3 timer events
        this_thread::sleep_for(chrono::milliseconds(300)); // Wait 300ms
        timerISR(); // Call ISR as if timer triggered
    }
}

// -----------------------------------------------------------------------------
// SECTION 5: MAIN FUNCTION
// -----------------------------------------------------------------------------
int main() {
    cout << "==== Firmware Interrupt Simulation Demo ====" << endl;

    // Start the main loop in a separate thread (like the main firmware loop)
    thread main_thread(mainLoop);

    // Start simulated hardware interrupts in separate threads
    thread button_thread(simulateButtonPress);
    thread timer_thread(simulateTimerInterrupt);

    // Wait for the simulated hardware events to finish
    button_thread.join();
    timer_thread.join();

    // Detach main loop (in real firmware it runs forever)
    main_thread.detach();

    cout << "==== Demo Complete ====" << endl;
    return 0;
}

/*
===============================================================================
Key Learnings:

1. Interrupt: A signal from hardware or software telling the CPU to execute
   a specific routine (ISR) immediately.

2. ISR (Interrupt Service Routine): The function executed when an interrupt occurs.
   - Must be fast
   - Often sets a flag or copies data
   - Avoid heavy processing

3. Main Loop: Performs heavy or time-consuming tasks after ISR sets flags.

4. atomic<bool>: Ensures safe communication between ISR and main program.

5. Thread Simulation: Used here to mimic asynchronous hardware events for learning.

6. Timer-based Interrupts: Show how firmware handles periodic events without
   continuously polling in main loop.

This code gives a **firmware-style pattern**:
- ISR triggers → set flag → main loop handles processing.
- This pattern is widely used in embedded systems, including Apple firmware.
===============================================================================
*/
