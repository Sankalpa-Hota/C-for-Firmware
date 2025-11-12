/*
===============================================================================
Purpose: Demonstrate memory alignment, struct padding, and firmware-style
         memory mapping with register simulation.
Author: Sankalpa Hota
How to compile:
  g++ 07_memory_mapping_and_alignment.cpp -o memory_mapping_demo
  ./memory_mapping_demo

This code is designed to teach beginners:
- How memory is laid out for structs in C++
- What padding is and why it matters
- How firmware maps structs to hardware addresses
- How to safely access hardware registers via pointers
===============================================================================
*/

#include <iostream>  // For input/output to console
#include <cstdint>   // For fixed-width integer types like uint32_t
#include <iomanip>   // For formatting output in hexadecimal
using namespace std;

// -----------------------------------------------------------------------------
// SECTION 1: STRUCT PADDING DEMO
// -----------------------------------------------------------------------------
/*
  A struct is a way to group variables under one name.
  The compiler may insert "padding" to align members to memory boundaries
  for faster access. This is important in firmware because hardware registers
  have exact memory layouts.
*/
struct DefaultStruct {
    char a;    // 1 byte
    int b;     // 4 bytes, aligned to next 4-byte boundary automatically
    char c;    // 1 byte
};

/*
  PackedStruct disables padding using #pragma pack.
  All fields are stored consecutively in memory with no extra space.
  This is critical when you want your struct to exactly match a hardware register layout.
*/
#pragma pack(push, 1) // Disable padding
struct PackedStruct {
    char a;
    int b;
    char c;
};
#pragma pack(pop)

// -----------------------------------------------------------------------------
// SECTION 2: REGISTER MAPPING SIMULATION
// -----------------------------------------------------------------------------
/*
  In firmware, peripherals (like GPIO, UART, or timers) have registers
  mapped to fixed memory addresses. You can access registers by treating
  the memory address as a pointer to a struct representing the register block.

  Here we simulate 3 consecutive 32-bit registers for a peripheral:
  CTRL   - control operations
  STATUS - status flags
  DATA   - data value
*/
#pragma pack(push, 1)
struct RegisterBlock {
    volatile uint32_t CTRL;   // Control register, 4 bytes
    volatile uint32_t STATUS; // Status register, 4 bytes
    volatile uint32_t DATA;   // Data register, 4 bytes
};
#pragma pack(pop)

/*
  Define the base address of the peripheral in memory.
  This is typically provided in the hardware datasheet.
*/
#define PERIPHERAL_BASE 0x4000

/*
  Map the memory address to a struct pointer.
  reinterpret_cast tells C++: "treat this numeric address as a pointer
  to our RegisterBlock struct." 

  Explanation:
  - Peripheral is a pointer to the struct
  - Dereferencing it accesses the "registers" inside the struct
  - const means the pointer itself cannot change
  - volatile ensures the compiler always reads/writes actual memory
    (important for hardware registers that can change outside program control)
*/
RegisterBlock* const Peripheral = reinterpret_cast<RegisterBlock*>(PERIPHERAL_BASE);

// -----------------------------------------------------------------------------
// SECTION 3: MAIN APPLICATION
// -----------------------------------------------------------------------------
int main() {
    cout << "==== Firmware Demo: Memory Mapping & Alignment ====" << endl;

    // 1️⃣ Compare sizes of struct with and without padding
    // DefaultStruct has padding to align 'b' to 4-byte boundary
    // PackedStruct has no padding, matches hardware registers exactly
    cout << "Size of DefaultStruct = " << sizeof(DefaultStruct) << " bytes" << endl;
    cout << "Size of PackedStruct  = " << sizeof(PackedStruct)  << " bytes" << endl;

    // 2️⃣ Show memory addresses of each member
    // This helps visualize how padding inserts extra bytes
    DefaultStruct ds;
    cout << "Address of a: " << (void*)&ds.a << " (1 byte)" << endl;
    cout << "Address of b: " << (void*)&ds.b << " (4 bytes, aligned)" << endl;
    cout << "Address of c: " << (void*)&ds.c << " (1 byte, after padding)" << endl;

    // 3️⃣ Simulate a peripheral register map
    cout << "\n[REGISTER MAP DEMO]" << endl;
    cout << "Peripheral Base Address: 0x" << hex << PERIPHERAL_BASE << endl;
    cout << "CTRL  offset = 0x0" << endl;
    cout << "STATUS offset = 0x4" << endl;
    cout << "DATA   offset = 0x8" << endl;

    // 4️⃣ Simulate firmware writing and reading registers
    // These lines would normally write to real hardware
    Peripheral->CTRL = 0x01;        // Example: start peripheral operation
    Peripheral->DATA = 0xABCD1234;  // Example: write some data
    Peripheral->STATUS = 0x05;      // Example: update status flags

    // Display register values
    cout << "\nCTRL  = 0x" << hex << setw(8) << setfill('0') << Peripheral->CTRL
         << " (Control register)" << endl;
    cout << "STATUS= 0x" << hex << setw(8) << setfill('0') << Peripheral->STATUS
         << " (Status register)" << endl;
    cout << "DATA  = 0x" << hex << setw(8) << setfill('0') << Peripheral->DATA
         << " (Data register)" << endl;

    cout << "\n==== Demo Complete ====" << endl;
    return 0;
}

/*
===============================================================================
Key Takeaways for Non-C++ Programmers:

1. Memory mapping allows firmware to communicate with hardware by using
   memory addresses as if they were variables.

2. Padding occurs when the compiler adds extra bytes to align data for
   faster access. This is okay for normal software but can break hardware access.

3. #pragma pack(push, 1) disables padding, which is required when struct
   layout must exactly match hardware.

4. volatile ensures that the compiler always reads/writes to memory, not cached values.
   This is essential for registers that can change due to hardware events.

5. reinterpret_cast allows treating a number (memory address) as a pointer
   to a struct, so we can read/write registers easily using -> operator.
===============================================================================
*/
