/*
===============================================================================
Purpose: Demonstrate usage of structures, unions, and bitfields 
         for firmware-style register and peripheral mapping.
Author: Sankalpa Hota
How to compile:
  g++ 06_structs_unions.cpp -o structs_unions_demo
  ./structs_unions_demo
===============================================================================
*/

#include <iostream>
#include <bitset>
#include <thread>
#include <chrono>
using namespace std;

// -----------------------------------------------------------------------------
// SECTION 1: STRUCT EXAMPLE — Representing LEDs as Firmware Components
// -----------------------------------------------------------------------------
/*
  A 'struct' lets us group multiple related fields.
  Here we create an LED struct to store pin number and state.
*/
struct LED {
    int pin;        // Physical pin number
    bool state;     // Current LED state (true = ON, false = OFF)
};

/*
  Function: initLED
  Initializes an LED with its pin and default state.
*/
void initLED(LED &led, int pin_num) {
    led.pin = pin_num;
    led.state = false;
    cout << "[INIT] LED on pin " << led.pin << " initialized to OFF" << endl;
}

/*
  Function: setLED
  Turns ON or OFF a given LED.
*/
void setLED(LED &led, bool on) {
    led.state = on;
    cout << "[SET] LED pin " << led.pin << " set to " << (on ? "ON" : "OFF") << endl;
}

/*
  Function: toggleLED
  Toggles the current state of the LED.
*/
void toggleLED(LED &led) {
    led.state = !led.state;
    cout << "[TOGGLE] LED pin " << led.pin << " is now " << (led.state ? "ON" : "OFF") << endl;
}

// -----------------------------------------------------------------------------
// SECTION 2: UNION + BITFIELD — Simulating a Hardware Register
// -----------------------------------------------------------------------------
/*
  In firmware, each hardware register controls multiple bits.
  Using a union with bitfields allows us to access individual bits or the whole register.
*/
union LEDRegister {
    uint8_t value;  // Full 8-bit register
    struct {
        uint8_t LED0 : 1;   // Bit 0 - LED0
        uint8_t LED1 : 1;   // Bit 1 - LED1
        uint8_t LED2 : 1;   // Bit 2 - LED2
        uint8_t LED3 : 1;   // Bit 3 - LED3
        uint8_t reserved : 4; // Remaining bits unused
    } bits;
};

/*
  Function: printLEDRegister
  Displays the 8-bit binary value of the register and each LED bit.
*/
void printLEDRegister(const LEDRegister &reg) {
    cout << "LED Register Value = " << bitset<8>(reg.value)
         << " [LED3:" << (int)reg.bits.LED3
         << " LED2:" << (int)reg.bits.LED2
         << " LED1:" << (int)reg.bits.LED1
         << " LED0:" << (int)reg.bits.LED0 << "]" << endl;
}

/*
  Function: setLEDRegisterBit
  Sets or clears individual LED bits in the union register.
*/
void setLEDRegisterBit(LEDRegister &reg, int led_num, bool on) {
    switch (led_num) {
        case 0: reg.bits.LED0 = on; break;
        case 1: reg.bits.LED1 = on; break;
        case 2: reg.bits.LED2 = on; break;
        case 3: reg.bits.LED3 = on; break;
        default: cout << "Invalid LED number!" << endl; return;
    }
    printLEDRegister(reg);
}

// -----------------------------------------------------------------------------
// SECTION 3: ARRAY OF STRUCTS + UNIONS — Managing Multiple LEDs
// -----------------------------------------------------------------------------
/*
  Here we combine struct arrays (for multiple LEDs) with a union register
  to simulate controlling multiple LEDs on a device.
*/
void blinkAllLEDs(LED leds[], int num_leds, LEDRegister &reg) {
    for (int i = 0; i < num_leds; i++) {
        // Turn LED ON
        setLED(leds[i], true);
        setLEDRegisterBit(reg, leds[i].pin, true);
        this_thread::sleep_for(chrono::milliseconds(200));

        // Turn LED OFF
        setLED(leds[i], false);
        setLEDRegisterBit(reg, leds[i].pin, false);
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

// -----------------------------------------------------------------------------
// MAIN APPLICATION
// -----------------------------------------------------------------------------
int main() {
    cout << "==== Firmware Demo: Structures, Unions, Bitfields ====" << endl;

    // 1️⃣ Initialize 4 LEDs using a struct array
    LED leds[4];
    for (int i = 0; i < 4; i++) {
        initLED(leds[i], i);
    }

    // 2️⃣ Create a union to represent the LED control register
    LEDRegister reg{};
    reg.value = 0x00; // All LEDs initially OFF
    printLEDRegister(reg);

    // 3️⃣ Blink each LED sequentially using both struct & register logic
    blinkAllLEDs(leds, 4, reg);

    // 4️⃣ Manually toggle LED bits using the union
    cout << "\n[MANUAL] Turning ON LED0 and LED2 manually..." << endl;
    setLEDRegisterBit(reg, 0, true);
    setLEDRegisterBit(reg, 2, true);

    cout << "\n[FINAL STATE]" << endl;
    printLEDRegister(reg);

    cout << "==== Demo Complete ====" << endl;
    return 0;
}
