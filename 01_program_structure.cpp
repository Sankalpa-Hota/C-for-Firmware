/*
=======================================================================
C++ BASICS FOR FIRMWARE ENGINEERS
Author  : Sankalpa Hota
Purpose : Demonstrate C++ syntax, structure, and firmware-level concepts
=======================================================================
 Topics Covered
1. Structure of a C++ Program
2. Data Types & Variables
3. Firmware Example using Variables
=======================================================================
*/

#include <iostream>
using namespace std;

/*
=======================================================================
 1. STRUCTURE OF A C++ PROGRAM
-----------------------------------------------------------------------
Every C++ program must have:
  - Preprocessor directives (#include)
  - A main() function (entry point)
  - Statements inside { } braces
=======================================================================
*/

int main() {
    cout << "===============================" << endl;
    cout << " Firmware Boot Sequence Started " << endl;
    cout << "===============================" << endl << endl;

    cout << "[BOOT] Loading System Modules..." << endl;
    cout << "[BOOT] Initializing peripherals..." << endl;
    cout << "[BOOT] System Online!" << endl << endl;

    /*
    ===================================================================
    2. DATA TYPES & VARIABLES
    -------------------------------------------------------------------
    Each variable reserves memory inside the MCU (RAM or register).
    Choosing correct types saves memory and improves speed.
    ===================================================================
    */

    // Integer (4 bytes): for counting, indexing
    int packet_count = 10;

    // Float (4 bytes): analog or sensor values
    float temperature = 25.6;

    // Character (1 byte): stores ASCII character or byte value
    char state = 'A'; // 'A' = Active

    // Boolean (1 byte): ON/OFF, TRUE/FALSE status
    bool system_on = true;

    // Unsigned Integer: only positive values (used for registers)
    unsigned int reg_value = 0xFF; // 255 in hexadecimal

    cout << "[DATA LOG]" << endl;
    cout << "  Packets Processed : " << packet_count << endl;
    cout << "  Temperature        : " << temperature << " °C" << endl;
    cout << "  System State       : " << state << endl;
    cout << "  System ON Flag     : " << system_on << endl;
    cout << "  Register Value     : " << reg_value << endl << endl;

    /*
    ===================================================================
     3. FIRMWARE-STYLE DATA HANDLING
    -------------------------------------------------------------------
    In embedded systems, variables often represent hardware values:
      - ADC readings
      - Sensor voltages
      - Threshold flags
    ===================================================================
    */

    // Simulate ADC reading (0–1023 range)
    unsigned int adc_value = 512; // read from analog sensor

    // Convert ADC value to actual voltage (assuming 3.3V reference)
    float voltage = (adc_value * 3.3) / 1023.0;

    // Overheat condition flag
    bool overheat = (voltage > 2.5);

    cout << "[SENSOR DATA]" << endl;
    cout << "  ADC Reading : " << adc_value << endl;
    cout << "  Voltage     : " << voltage << " V" << endl;

    if (overheat)
        cout << "    Warning: Overheat Detected!" << endl;
    else
        cout << "   System Temperature Normal." << endl;

    cout << endl;

    /*
    ===================================================================
     FIRMWARE INSIGHT
    -------------------------------------------------------------------
    In real microcontrollers:
      - 'cout' is replaced by 'printf' or UART debug messages.
      - Each variable lives in physical memory.
      - Efficiency matters: using smaller data types saves RAM.
    ===================================================================
    */

    cout << "[SYSTEM STATUS] Shutting Down Firmware Simulation..." << endl;
    cout << "======================================================" << endl;

    return 0;
}
