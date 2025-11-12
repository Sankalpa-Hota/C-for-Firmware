/*
===============================================================================
File: 03_control_flow_and_loops.cpp
Purpose: Teach control flow and loops with firmware-style examples (line-by-line)
Author: Sankalpa Hota
How to compile:
  g++ 04_control_flow_and_loops.cpp -o control_demo
  ./control_demo
===============================================================================
*/

#include <iostream>
#include <chrono>
#include <thread>
#include <bitset>

using namespace std;
using namespace std::chrono;

// --------- Helper (simulated hardware) --------------
bool simulated_ready_flag = false;   // simulate a hardware status bit
int simulated_button_pin = 0;        // simulate a button (0 or 1)
int simulated_i2c_attempts = 0;      // simulate failures then success

// Simulate reading a hardware register bit (ready flag)
bool hw_is_ready() {
    return simulated_ready_flag;
}

// Simulate an action that will set ready after some time (for demo)
void hw_make_ready_after_ms(int ms) {
    // spawn a thread that sets ready flag after ms milliseconds (simulation)
    thread([ms]() {
        this_thread::sleep_for(milliseconds(ms));
        simulated_ready_flag = true;
    }).detach();
}

// Simulate reading a GPIO pin (button)
int read_button_pin() {
    return simulated_button_pin;
}

// Simulate an I2C transfer that fails first 2 times, succeeds on 3rd attempt
bool i2c_transfer_once() {
    simulated_i2c_attempts++;
    if (simulated_i2c_attempts >= 3) return true; // succeed on 3rd try
    return false;
}

// Simulate a small delay function (in real firmware use timer or sleep)
void delay_ms(int ms) {
    this_thread::sleep_for(milliseconds(ms));
}

// ----------------- DEMONSTRATIONS --------------------

int demo_if_else(int temp_reading) {
    // Demonstrates basic if/else
    // Input: temp_reading in degrees Celsius (int)
    cout << "\n--- demo_if_else ---\n";
    const int TEMP_WARN = 70; // threshold
    if (temp_reading >= TEMP_WARN) {
        cout << "TEMP: " << temp_reading << "C -> WARNING: Overtemp!\n";
        return 1; // error code 1
    } else if (temp_reading >= 60) {
        cout << "TEMP: " << temp_reading << "C -> Caution: High temp\n";
        return 0;
    } else {
        cout << "TEMP: " << temp_reading << "C -> Normal\n";
        return 0;
    }
}

void demo_switch(int cmd) {
    // Demonstrates switch-case for command decoding
    cout << "\n--- demo_switch ---\n";
    switch (cmd) {
        case 0:
            cout << "CMD 0: No-op\n";
            break;
        case 1:
            cout << "CMD 1: Start measurement\n";
            break;
        case 2:
            cout << "CMD 2: Stop measurement\n";
            break;
        case 3:
        case 4:
            // fall-through intentional: 3 and 4 handled similarly
            cout << "CMD 3/4: Special modes\n";
            break;
        default:
            cout << "CMD unknown: Ignoring\n";
            break;
    }
}

void demo_for_loop() {
    // Demonstrates for-loop usage: iterating known count
    cout << "\n--- demo_for_loop ---\n";
    for (int i = 0; i < 5; ++i) {
        cout << "Iteration " << i << "\n";
        // simulate small work
        delay_ms(50);
    }
}

void demo_while_poll_with_timeout(int timeout_ms) {
    // Demonstrates polling a hardware-ready flag with timeout (must-have pattern)
    cout << "\n--- demo_while_poll_with_timeout ---\n";
    // Start hardware to become ready after 200ms (simulation)
    hw_make_ready_after_ms(200);

    auto start = steady_clock::now();                 // record start time
    while (!hw_is_ready()) {                          // repeat while hardware not ready
        auto now = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(now - start).count();
        if (elapsed > timeout_ms) {                  // check timeout
            cout << "Timeout! device not ready after " << elapsed << " ms\n";
            return;                                  // give up or handle error
        }
        // Optionally do small sleep to avoid tight busy-wait in simulation
        delay_ms(10);
    }
    cout << "Device became ready!\n";
}

void demo_do_while() {
    // Demonstrates do-while: execute once then test condition
    cout << "\n--- demo_do_while ---\n";
    int counter = 0;
    do {
        cout << "do-while iteration: " << counter << "\n";
        ++counter;
    } while (counter < 3);
}

void demo_debounce_button() {
    // Debouncing example: require N consistent reads before accepting change
    cout << "\n--- demo_debounce_button ---\n";
    const int STABLE_REQUIRED = 3;    // number of consistent samples required
    int stable_count = 0;
    int prev = read_button_pin();     // initial reading
    cout << "Initial pin reading: " << prev << "\n";

    // simulate bouncing by changing simulated_button_pin in background
    thread([](){
        // bounce: 0 -> 1 -> 0 -> 1 then stay 1
        delay_ms(10); simulated_button_pin = 1;
        delay_ms(10); simulated_button_pin = 0;
        delay_ms(10); simulated_button_pin = 1;
        // then remain 1
    }).detach();

    // sample until we see STABLE_REQUIRED consistent reads of the same value
    while (stable_count < STABLE_REQUIRED) {
        int cur = read_button_pin();
        if (cur == prev) {
            stable_count++;
        } else {
            // bounce detected, reset stable count
            stable_count = 0;
            prev = cur;
        }
        cout << "Sample read: " << cur << "  (stable_count=" << stable_count << ")\n";
        delay_ms(5); // small sample interval
    }
    cout << "Debounced value: " << prev << "\n";
}

void demo_retry_with_backoff() {
    // Retry pattern: try operation up to N times with small delay/backoff
    cout << "\n--- demo_retry_with_backoff ---\n";
    const int MAX_RETRIES = 5;
    int attempt = 0;
    bool success = false;
    while (attempt < MAX_RETRIES) {
        ++attempt;
        cout << "Attempt " << attempt << " ... ";
        if (i2c_transfer_once()) {
            cout << "Success!\n";
            success = true;
            break;             // break out of loop on success
        } else {
            cout << "Fail\n";
            // simple backoff: wait longer each attempt
            delay_ms(10 * attempt);
            continue;          // go to next attempt
        }
    }
    if (!success) {
        cout << "All retries failed\n";
    }
}

void demo_nested_loops() {
    cout << "\n--- demo_nested_loops ---\n";
    for (int r = 0; r < 3; ++r) {
        cout << "Row " << r << ":\n";
        for (int c = 0; c < 4; ++c) {
            cout << "  Col " << c << "\n";
        }
    }
}

int main() {
    cout << "==== Control Flow & Loops Demo ====\n";

    // 1) if/else demo with various inputs
    demo_if_else(55);
    demo_if_else(65);
    demo_if_else(75);

    // 2) switch demo
    demo_switch(0);
    demo_switch(3);
    demo_switch(99);

    // 3) for loop demo
    demo_for_loop();

    // 4) while polling with timeout (timeout = 500 ms)
    demo_while_poll_with_timeout(500);

    // 5) do-while demo
    demo_do_while();

    // 6) debounce demo
    demo_debounce_button();

    // 7) retry-with-backoff demo (I2C)
    demo_retry_with_backoff();

    // 8) nested loops demo
    demo_nested_loops();

    cout << "\n==== Demo Complete ====\n";
    return 0;
}
