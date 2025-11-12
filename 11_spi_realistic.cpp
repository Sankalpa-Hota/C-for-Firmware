/******************************************************
 * Purpose: Simulate realistic SPI communication between
 *          master and slave, bit-by-bit transfer, with
 *          echo back. Demonstrates shift register behavior.
 *
 * Compile:
 *   g++ 11_spi_realistic.cpp -o spi_demo -std=c++11 -pthread
 *   ./spi_demo
 ******************************************************/

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

// -----------------------------------------------------------------------------
// SECTION 1: SPI Bus Simulation
// -----------------------------------------------------------------------------
struct SPIBus {
    uint8_t MOSI = 0;    // Master Out Slave In
    uint8_t MISO = 0;    // Master In Slave Out
    bool CS = true;      // Chip select (active low)
    bool SCLK = false;   // Clock signal
    mutex spiLock;       // Protect shared bus access
} SPI;

// -----------------------------------------------------------------------------
// SECTION 2: SPI Slave (bit-by-bit shift)
// -----------------------------------------------------------------------------
void SPISlave(uint8_t &received_byte) {
    /*
      Simulate a hardware SPI slave that receives one byte
      from master, MSB first, and can optionally echo it back.
    */
    received_byte = 0; // initialize received byte

    // Wait for CS low (start of transaction)
    while (SPI.CS) {
        this_thread::sleep_for(chrono::milliseconds(1));
    }

    cout << "[SLAVE] Transaction started" << endl;

    // Receive 8 bits (MSB first)
    for (int i = 7; i >= 0; i--) {
        // Wait for SCLK rising edge
        while (!SPI.SCLK) this_thread::sleep_for(chrono::milliseconds(1));

        {
            lock_guard<mutex> lock(SPI.spiLock);

            // Read master bit (MOSI) and store in correct position
            received_byte |= ((SPI.MOSI & 1) << i);

            // Echo bit back: here we just send the same bit
            SPI.MISO = (SPI.MOSI & 1);
        }

        // Wait for clock to go low (falling edge) before next bit
        while (SPI.SCLK) this_thread::sleep_for(chrono::milliseconds(1));
    }

    cout << "[SLAVE] Byte received: 0b"
         << bitset<8>(received_byte) << endl;
}

// -----------------------------------------------------------------------------
// SECTION 3: SPI Master (send byte, read echo)
// -----------------------------------------------------------------------------
void SPIMaster(uint8_t data_out) {
    cout << "[MASTER] Sending byte: 0b" << bitset<8>(data_out) << endl;

    // Start transaction by pulling CS low
    SPI.CS = false;

    uint8_t received_byte = 0;

    // Send 8 bits, MSB first
    for (int i = 7; i >= 0; i--) {
        {
            lock_guard<mutex> lock(SPI.spiLock);

            // Set MOSI bit
            SPI.MOSI = (data_out >> i) & 1;

            // Toggle clock high → slave reads bit
            SPI.SCLK = true;
        }

        // Small delay to simulate hardware clock
        this_thread::sleep_for(chrono::milliseconds(50));

        {
            lock_guard<mutex> lock(SPI.spiLock);

            // Read MISO bit from slave
            received_byte |= ((SPI.MISO & 1) << i);

            // Toggle clock low → prepare next bit
            SPI.SCLK = false;
        }

        this_thread::sleep_for(chrono::milliseconds(50));
    }

    // End transaction
    SPI.CS = true;

    cout << "[MASTER] Byte received from slave: 0b"
         << bitset<8>(received_byte) << endl;
}

// -----------------------------------------------------------------------------
// SECTION 4: MAIN
// -----------------------------------------------------------------------------
int main() {
    cout << "==== SPI Realistic Simulation ====" << endl;

    // Byte to send
    uint8_t data_to_send = 0b10101100;
    uint8_t received_by_slave = 0;

    // Start slave thread
    thread slave_thread(SPISlave, ref(received_by_slave));

    // Give slave a moment to start
    this_thread::sleep_for(chrono::milliseconds(10));

    // Master sends byte
    SPIMaster(data_to_send);

    // Wait for slave to finish
    slave_thread.join();

    cout << "==== SPI Simulation Complete ====" << endl;
    return 0;
}

/*
===============================================================================
✅ Concepts Demonstrated:

1. Bit-by-bit transfer:
   - MSB-first
   - Rising edge reads
   - Falling edge prepares next bit

2. Master / Slave relationship:
   - CS low = transaction active
   - Clock synchronization

3. Shift register simulation:
   - Slave reconstructs byte from individual bits

4. Echo back:
   - MISO returns same bit sent by master
   - Can be replaced by computed response

5. Mutex usage:
   - Simulates exclusive bus access (like disabling IRQs in hardware)

Real-world relevance:
- SPI flash, ADCs, sensors use similar bit-level transfer.
- This simulation teaches the underlying timing and sequence
  for building firmware drivers.
===============================================================================
*/
