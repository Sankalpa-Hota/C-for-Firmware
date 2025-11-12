/******************************************************
 * Filename: 15_register_memory.cpp
 * Purpose: Demonstrates memory-mapped registers,
 *          bit manipulation, volatile access, and
 *          safe read-modify-write patterns in C++.
 ******************************************************/

#include <iostream>
using namespace std;

// --------------------------- Register Map -----------------------------
struct LED_REGS {
    volatile uint8_t DATA;   // LED output data
    volatile uint8_t CTRL;   // Control register (enable, mode)
};

#define LED_BASE 0x4000
LED_REGS* const LED = reinterpret_cast<LED_REGS*>(LED_BASE);

// Bit positions
#define LED_ENABLE 0
#define LED_MODE0  1
#define LED_MODE1  2

// --------------------------- Functions -----------------------------
void enableLED() {
    LED->CTRL |= (1 << LED_ENABLE); // Set bit 0 to enable LED
    cout << "[DEBUG] LED enabled" << endl;
}

void setLEDMode(uint8_t mode) {
    LED->CTRL &= ~((1 << LED_MODE0) | (1 << LED_MODE1)); // Clear mode bits
    LED->CTRL |= ((mode & 0x03) << LED_MODE0);           // Set new mode
    cout << "[DEBUG] LED mode set to: " << (int)mode << endl;
}

void toggleLED(uint8_t pin) {
    LED->DATA ^= (1 << pin); // Toggle specific LED pin
    cout << "[DEBUG] LED pin " << (int)pin << " toggled" << endl;
}

// --------------------------- Main -----------------------------
int main() {
    cout << "=== Register & Memory Demo ===" << endl;

    enableLED();      // Enable LED
    setLEDMode(2);    // Set LED mode to 2
    toggleLED(0);     // Toggle LED pin 0
    toggleLED(1);     // Toggle LED pin 1

    cout << "CTRL Register: 0b" << bitset<8>(LED->CTRL) << endl;
    cout << "DATA Register: 0b" << bitset<8>(LED->DATA) << endl;

    cout << "=== Demo Complete ===" << endl;
    return 0;
}
