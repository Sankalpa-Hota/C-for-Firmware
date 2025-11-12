/******************************************************
 * Filename: 16_firmware_power.cpp
 * Purpose: Demonstrates firmware optimization and
 *          low-power simulation using sleep modes,
 *          peripheral enable/disable, and efficient loops.
 ******************************************************/

#include <iostream>
#include <thread>
#include <bitset>
using namespace std;

// Simulated registers
struct LED_REGS {
    volatile uint8_t DATA;
    volatile uint8_t CTRL;
};
LED_REGS LED = {0,0};

// Bit positions
#define LED_ENABLE 0

// Efficient function to toggle LED
inline void toggleLED(uint8_t pin) {
    LED.DATA ^= (1 << pin); // XOR toggles
    cout << "[DEBUG] LED pin " << (int)pin 
         << " toggled: " << bitset<8>(LED.DATA) << endl;
}

// Simulate low-power sleep
void lowPowerSleep(int ms) {
    cout << "[DEBUG] Entering low-power sleep for " << ms << " ms" << endl;
    this_thread::sleep_for(chrono::milliseconds(ms));
    cout << "[DEBUG] Woke up from sleep" << endl;
}

// Simulate peripheral control
void controlPeripheral(bool enable) {
    LED.CTRL = enable ? (1 << LED_ENABLE) : 0;
    cout << "[DEBUG] LED peripheral " 
         << (enable ? "enabled" : "disabled") << endl;
}

int main() {
    cout << "=== Firmware Optimization & Power Management ===" << endl;

    // Enable peripheral
    controlPeripheral(true);

    // Optimized loop: unrolled
    for(int i=0;i<2;i++) toggleLED(i);
    toggleLED(0);
    toggleLED(1);

    // Enter low-power sleep for 500 ms
    lowPowerSleep(500);

    // Disable peripheral
    controlPeripheral(false);

    cout << "=== Demo Complete ===" << endl;
    return 0;
}
