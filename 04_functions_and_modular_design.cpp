/*
===============================================================================
File: 04_functions_and_modular_design.cpp
Purpose: Demonstrate functions, modular design, and parameter passing
Author: Sankalpa Hota
How to compile:
  g++ 05_functions_and_modular_design.cpp -o functions_demo
  ./functions_demo
===============================================================================
*/

#include <iostream>
#include <bitset>   // For visualizing register bits
#include <thread>   // For delays (simulated)
#include <chrono>

using namespace std;

// ------------------- Simulated Hardware Layer -------------------
/*
  In real firmware, these would be hardware registers.
  Here, we simulate an 8-bit LED control register.
*/
unsigned char LED_REG = 0x00;  // 8 LEDs, each bit controls one LED

// ------------------- Hardware Abstraction Functions -------------------

/*
  initLED:
  Purpose: Initialize all LEDs to OFF (clear register)
  Parameters: none
  Return: none
*/
void initLED() {
    LED_REG = 0x00;  // Clear all LEDs
    cout << "[initLED] All LEDs initialized to OFF: " 
         << bitset<8>(LED_REG) << endl;
}

/*
  setLED:
  Purpose: Set a specific LED ON or OFF
  Parameters:
    led_num: LED index (0-7)
    state: true=ON, false=OFF
  Return: none
*/
void setLED(int led_num, bool state) {
    if (led_num < 0 || led_num > 7) {
        cout << "[setLED] Error: Invalid LED number " << led_num << endl;
        return;
    }
    if (state) {
        LED_REG |= (1 << led_num);    // Set bit to 1
    } else {
        LED_REG &= ~(1 << led_num);   // Clear bit to 0
    }
    cout << "[setLED] LED " << led_num 
         << (state ? " ON : " : " OFF : ") 
         << bitset<8>(LED_REG) << endl;
}

/*
  toggleLED:
  Purpose: Toggle a specific LED (ON→OFF, OFF→ON)
  Parameters:
    led_num: LED index (0-7)
  Return: none
*/
void toggleLED(int led_num) {
    if (led_num < 0 || led_num > 7) {
        cout << "[toggleLED] Error: Invalid LED number " << led_num << endl;
        return;
    }
    LED_REG ^= (1 << led_num);  // XOR flips the bit
    cout << "[toggleLED] LED " << led_num 
         << " toggled: " << bitset<8>(LED_REG) << endl;
}

/*
  blinkLED:
  Purpose: Blink a specific LED N times with delay
  Parameters:
    led_num: LED index
    times: number of blinks
    delay_ms: delay between ON/OFF in milliseconds
  Return: none
*/
void blinkLED(int led_num, int times, int delay_ms) {
    for (int i = 0; i < times; i++) {
        setLED(led_num, true);   // Turn ON
        this_thread::sleep_for(chrono::milliseconds(delay_ms));
        setLED(led_num, false);  // Turn OFF
        this_thread::sleep_for(chrono::milliseconds(delay_ms));
    }
}

// ------------------- Application Layer -------------------
/*
  main():
  Purpose: Demonstrate modular LED control functions
*/
int main() {
    cout << "==== Firmware Modular Functions Demo ====" << endl;

    // Initialize all LEDs
    initLED();

    // Set LED 0 ON
    setLED(0, true);

    // Set LED 3 ON
    setLED(3, true);

    // Toggle LED 0
    toggleLED(0);

    // Toggle LED 2 (was OFF → becomes ON)
    toggleLED(2);

    // Blink LED 1 three times with 200ms interval
    blinkLED(1, 3, 200);

    // Attempt invalid LED number
    setLED(10, true);

    cout << "==== Demo Complete ====" << endl;
    return 0;
}
