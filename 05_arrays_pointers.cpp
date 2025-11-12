/*
===============================================================================
File: 05_arrays_pointers.cpp
Purpose: Demonstrate arrays, pointers, pointer arithmetic, 
         and memory-mapped register simulation in C++
Author: Sankalpa Hota
How to compile:
  g++ 06_arrays_pointers.cpp -o arrays_pointers_demo
  ./arrays_pointers_demo
===============================================================================
*/

#include <iostream>
#include <bitset>
#include <thread>
#include <chrono>

using namespace std;

// ------------------- Simulated Hardware Registers -------------------
/*
  In firmware, registers are memory-mapped.
  Here we simulate 4 registers as bytes.
*/
volatile unsigned char REG_LED = 0x00;       // 8-bit LED register
volatile unsigned char REG_STATUS = 0x00;    // 8-bit status register
volatile unsigned char REG_CONTROL = 0x00;   // 8-bit control register
volatile unsigned char REG_TEMP[4] = {25, 26, 24, 23}; // temp sensor array

// ------------------- Functions -------------------

/*
  printRegister:
  Purpose: Print the 8-bit register in binary
  Parameters:
    name: string name of register
    reg: register value (byte)
*/
void printRegister(string name, unsigned char reg) {
    cout << name << " = " << bitset<8>(reg) << endl;
}

/*
  blinkLEDArray:
  Purpose: Blink LEDs using an array of LED numbers
  Parameters:
    leds: pointer to array of LED indices
    size: number of LEDs
*/
void blinkLEDArray(int *leds, int size) {
    for (int i = 0; i < size; i++) {
        int led_num = *(leds + i);  // pointer arithmetic
        REG_LED |= (1 << led_num);  // turn ON
        printRegister("REG_LED", REG_LED);
        this_thread::sleep_for(chrono::milliseconds(100));
        REG_LED &= ~(1 << led_num); // turn OFF
        printRegister("REG_LED", REG_LED);
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

/*
  pollStatus:
  Purpose: Wait until a specific bit in STATUS register is set
  Parameters:
    bit_pos: bit to check
    timeout_ms: timeout in milliseconds
*/
bool pollStatus(int bit_pos, int timeout_ms) {
    auto start = chrono::steady_clock::now();
    while (!(REG_STATUS & (1 << bit_pos))) { // poll until bit set
        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - start).count();
        if (elapsed > timeout_ms) {
            cout << "Timeout waiting for STATUS bit " << bit_pos << endl;
            return false;
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    cout << "STATUS bit " << bit_pos << " is set!" << endl;
    return true;
}

/*
  updateTemperatureArray:
  Purpose: Update temperature array using pointer iteration
*/
void updateTemperatureArray(volatile unsigned char *temps, int size, unsigned char delta) {
    for (int i = 0; i < size; i++) {
        *(temps + i) += delta;   // pointer arithmetic to access array
    }
}

/*
  printTemperatureArray:
  Purpose: Print temperature array
*/
void printTemperatureArray(volatile unsigned char *temps, int size) {
    cout << "Temperature Array: ";
    for (int i = 0; i < size; i++) {
        cout << (int)*(temps + i) << " ";
    }
    cout << endl;
}

// ------------------- Main Application -------------------
int main() {
    cout << "==== Arrays & Pointers Firmware Demo ====" << endl;

    // 1) Print initial registers
    printRegister("REG_LED", REG_LED);
    printRegister("REG_STATUS", REG_STATUS);
    printRegister("REG_CONTROL", REG_CONTROL);

    // 2) Blink LEDs using array and pointers
    int led_sequence[3] = {0, 3, 5};  // LED numbers to blink
    blinkLEDArray(led_sequence, 3);

    // 3) Poll STATUS bit 2 (simulate setting after 300ms)
    thread([](){
        this_thread::sleep_for(chrono::milliseconds(300));
        REG_STATUS |= (1 << 2);
    }).detach();
    pollStatus(2, 500); // wait max 500ms

    // 4) Update temperature array using pointer arithmetic
    printTemperatureArray(REG_TEMP, 4);
    updateTemperatureArray(REG_TEMP, 4, 2);
    printTemperatureArray(REG_TEMP, 4);

    cout << "==== Demo Complete ====" << endl;
    return 0;
}
