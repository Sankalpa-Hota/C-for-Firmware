/******************************************************
 * Filename: 17_bootloader_sim.cpp
 * Purpose: Simulates Bootloader and Main Firmware.
 *          Demonstrates boot process, firmware validation,
 *          and jump to main application in C++.
 ******************************************************/

#include <iostream>
#include <string>
using namespace std;

// --------------------------- Bootloader -----------------------------
bool validateFirmware(const string &fw) {
    // Simulate checksum/signature validation
    if(fw == "VALID_FW") {
        cout << "[BOOTLOADER] Firmware validation SUCCESS\n";
        return true;
    } else {
        cout << "[BOOTLOADER] Firmware validation FAILED\n";
        return false;
    }
}

void loadFirmware(const string &fw) {
    cout << "[BOOTLOADER] Loading firmware: " << fw << endl;
    // Simulate delay
    for(int i=0;i<3;i++) {
        cout << "[BOOTLOADER] Initializing memory/peripherals...\n";
    }
}

// --------------------------- Main Firmware --------------------------
void mainFirmware() {
    cout << "[MAIN FW] Firmware execution started!\n";
    cout << "[MAIN FW] Running tasks...\n";
}

// --------------------------- Main -----------------------------
int main() {
    cout << "=== Bootloader Simulation ===\n";

    string firmware = "VALID_FW"; // try "CORRUPT_FW" to test failure

    cout << "[BOOTLOADER] Starting boot sequence...\n";

    if(validateFirmware(firmware)) {
        loadFirmware(firmware);
        cout << "[BOOTLOADER] Jumping to main firmware...\n";
        mainFirmware(); // simulate jump
    } else {
        cout << "[BOOTLOADER] Staying in bootloader. Awaiting recovery...\n";
    }

    cout << "=== Simulation Complete ===\n";
    return 0;
}
