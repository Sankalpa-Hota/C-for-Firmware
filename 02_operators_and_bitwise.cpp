/*
===============================================================================
MODULE 02: BITWISE FIELD READ/WRITE (Solutions + Explanation)
Author  : Sankalpa Hota
Purpose : Demonstrate how firmware reads/writes specific bits in registers
===============================================================================

Concepts Covered:
Bit shifting and masking
Reading a bitfield from a register
Writing (updating) a bitfield into a register
===============================================================================
*/

#include <iostream>
#include <bitset>  // for printing binary format
using namespace std;

int main() {
    cout << "===============================================" << endl;
    cout << " Firmware Bitfield Read/Write Simulation " << endl;
    cout << "===============================================" << endl << endl;

    /*
    ===========================================================================
    Step 1 : Initialize the register and parameters
    ---------------------------------------------------------------------------
    In a microcontroller, 'reg' could represent an 8-bit hardware register,
    e.g., a GPIO configuration register, control register, or status flag.
    ===========================================================================
    */

    unsigned char reg = 0b11000110;  // 0xC6 in hex
    int pos = 4;                     // start bit of field
    int width = 4;                   // field width in bits
    unsigned char value = 0b1000;    // value to write into field (0x8)

    cout << "[INIT VALUES]" << endl;
    cout << "Register (reg): " << bitset<8>(reg) << " (0x" << hex << (int)reg << ")" << dec << endl;
    cout << "Position (pos): " << pos << endl;
    cout << "Width    (bits): " << width << endl;
    cout << "Value to Write : " << bitset<4>(value) << " (0x" << hex << (int)value << ")" << dec << endl;
    cout << endl;

    /*
    ===========================================================================
    Step 2 : READ FIELD
    Formula: field = (reg >> pos) & ((1 << width) - 1)
    ---------------------------------------------------------------------------
    This extracts a subset of bits from a register.
    Example: If reg = 1100 0110, pos = 4, width = 4
             we extract bits [7:4] = 1100
    ===========================================================================
    */

    unsigned char field = (reg >> pos) & ((1 << width) - 1);

    cout << "[READ FIELD]" << endl;
    cout << "Shift reg >> " << pos << "  →  " << bitset<8>(reg >> pos) << endl;
    cout << "Mask ((1 << width)-1) →  " << bitset<8>((1 << width) - 1) << endl;
    cout << "Field = (reg >> pos) & mask → " << bitset<8>(field) << " = " << (int)field << endl << endl;

    /*
    ===========================================================================
    Step 3 : WRITE FIELD
    Formula: reg = (reg & ~(mask << pos)) | ((value & mask) << pos)
    ---------------------------------------------------------------------------
    - First, we clear the bits in that field (set them to 0)
    - Then, we insert the new value at that position
    ===========================================================================
    */

    unsigned char mask = (1 << width) - 1;     // e.g. width=4 → mask=0b1111
    unsigned char cleared = reg & ~(mask << pos); // clear field bits
    unsigned char newbits = (value & mask) << pos; // prepare bits to insert
    unsigned char new_reg = cleared | newbits;     // combine both

    cout << "[WRITE FIELD]" << endl;
    cout << "Mask        : " << bitset<8>(mask) << endl;
    cout << "Mask << pos : " << bitset<8>(mask << pos) << endl;
    cout << "~(mask<<pos): " << bitset<8>(~(mask << pos)) << endl;
    cout << "Cleared reg : " << bitset<8>(cleared) << endl;
    cout << "(value&mask)<<pos : " << bitset<8>(newbits) << endl;
    cout << "Final reg   : " << bitset<8>(new_reg) << " (0x" << hex << (int)new_reg << ")" << dec << endl << endl;

    /*
    ===========================================================================
    Step 4 : Display Final Results
    ---------------------------------------------------------------------------
    In embedded code, this helps confirm correct register bit manipulation.
    ===========================================================================
    */

    cout << "================== SUMMARY ==================" << endl;
    cout << "Original reg : " << bitset<8>(0b11000110) << " (0xC6)" << endl;
    cout << "Read Field   : " << bitset<4>(field) << " (0xC)" << endl;
    cout << "New Value    : " << bitset<4>(value) << " (0x8)" << endl;
    cout << "Updated reg  : " << bitset<8>(new_reg) << " (0x86)" << endl;
    cout << "=============================================" << endl;

    return 0;
}

/*
===============================================
 Firmware Bitfield Read/Write Simulation 
===============================================

[INIT VALUES]
Register (reg): 11000110 (0xc6)
Position (pos): 4
Width    (bits): 4
Value to Write : 1000 (0x8)

[READ FIELD]
Shift reg >> 4  →  00001100
Mask ((1 << width)-1) →  00001111
Field = (reg >> pos) & mask → 00001100 = 12

[WRITE FIELD]
Mask        : 00001111
Mask << pos : 11110000
~(mask<<pos): 00001111
Cleared reg : 00000110
(value&mask)<<pos : 10000000
Final reg   : 10000110 (0x86)

================== SUMMARY ==================
Original reg : 11000110 (0xC6)
Read Field   : 1100 (0xC)
New Value    : 1000 (0x8)
Updated reg  : 10000110 (0x86)
=============================================
*/
