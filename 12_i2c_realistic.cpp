/******************************************************
 * Purpose: Realistic I2C master-slave simulation in C++.
 *          Demonstrates start/stop, 7-bit address, ACK/NACK,
 *          and bit-by-bit data transfer using threads.
 *
 * Compile:
 *   g++ 12_i2c_realistic.cpp -o i2c_demo -std=c++11 -pthread
 *   ./i2c_demo
 ******************************************************/

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <bitset>
using namespace std;

// -----------------------------------------------------------------------------
// SECTION 1: I2C Bus Simulation
// -----------------------------------------------------------------------------
struct I2CBus {
    bool SDA = true;       // Data line (1 = idle)
    bool SCL = true;       // Clock line (1 = idle)
    mutex busLock;         // Protects SDA/SCL access
} I2C;

// -----------------------------------------------------------------------------
// SECTION 2: I2C Slave
// -----------------------------------------------------------------------------
void I2CSlave(uint8_t address, uint8_t &received_data) {
    /*
      Simulates a slave waiting for its address,
      ACK/NACK, and receiving 8-bit data from master
    */
    received_data = 0;

    // Wait for Start condition (SDA goes low while SCL is high)
    while (I2C.SDA || !I2C.SCL) this_thread::sleep_for(chrono::milliseconds(1));
    cout << "[SLAVE] Detected START condition" << endl;

    // Receive 7-bit address + R/W bit
    uint8_t received_address = 0;
    for (int i = 7; i >= 1; i--) {  // 7-bit address
        while (!I2C.SCL) this_thread::sleep_for(chrono::milliseconds(1)); // wait SCL high
        {
            lock_guard<mutex> lock(I2C.busLock);
            received_address |= ((I2C.SDA & 1) << i);
        }
        while (I2C.SCL) this_thread::sleep_for(chrono::milliseconds(1)); // wait SCL low
    }

    cout << "[SLAVE] Address received: 0b" << bitset<7>(received_address) << endl;

    // ACK if address matches
    if (received_address == address) {
        lock_guard<mutex> lock(I2C.busLock);
        I2C.SDA = false; // ACK
        cout << "[SLAVE] Sending ACK" << endl;
    } else {
        lock_guard<mutex> lock(I2C.busLock);
        I2C.SDA = true;  // NACK
        cout << "[SLAVE] Sending NACK" << endl;
        return;          // ignore transaction
    }

    // Receive 8-bit data
    for (int i = 7; i >= 0; i--) {
        while (!I2C.SCL) this_thread::sleep_for(chrono::milliseconds(1));
        {
            lock_guard<mutex> lock(I2C.busLock);
            received_data |= ((I2C.SDA & 1) << i);
        }
        while (I2C.SCL) this_thread::sleep_for(chrono::milliseconds(1));
    }

    // Send ACK after data
    lock_guard<mutex> lock(I2C.busLock);
    I2C.SDA = false; // ACK
    cout << "[SLAVE] Data received: 0b" << bitset<8>(received_data)
         << " | ACK sent" << endl;

    // Wait for Stop condition (SDA goes high while SCL is high)
    while (!I2C.SDA || !I2C.SCL) this_thread::sleep_for(chrono::milliseconds(1));
    cout << "[SLAVE] Detected STOP condition" << endl;
}

// -----------------------------------------------------------------------------
// SECTION 3: I2C Master
// -----------------------------------------------------------------------------
void I2CMaster_Write(uint8_t address, uint8_t data) {
    /*
      Simulates master sending start, address, data,
      and stop conditions over I2C bus
    */

    // START condition: SDA goes low while SCL is high
    {
        lock_guard<mutex> lock(I2C.busLock);
        I2C.SDA = false;
        I2C.SCL = true;
    }
    cout << "[MASTER] START condition" << endl;
    this_thread::sleep_for(chrono::milliseconds(50));

    // Send 7-bit address (MSB first)
    for (int i = 7; i >= 1; i--) {
        lock_guard<mutex> lock(I2C.busLock);
        I2C.SDA = (address >> i) & 1;
        // Toggle clock high
        I2C.SCL = true;
        this_thread::sleep_for(chrono::milliseconds(50));
        I2C.SCL = false;
    }

    // Simulate wait for ACK/NACK
    this_thread::sleep_for(chrono::milliseconds(50));

    // Send 8-bit data (MSB first)
    for (int i = 7; i >= 0; i--) {
        lock_guard<mutex> lock(I2C.busLock);
        I2C.SDA = (data >> i) & 1;
        // Clock high → slave reads
        I2C.SCL = true;
        this_thread::sleep_for(chrono::milliseconds(50));
        I2C.SCL = false;
    }

    // STOP condition: SDA goes high while SCL is high
    {
        lock_guard<mutex> lock(I2C.busLock);
        I2C.SDA = true;
        I2C.SCL = true;
    }
    cout << "[MASTER] STOP condition" << endl;
}

// -----------------------------------------------------------------------------
// SECTION 4: MAIN
// -----------------------------------------------------------------------------
int main() {
    cout << "==== I2C Realistic Simulation ====" << endl;

    uint8_t slave_address = 0x50;
    uint8_t data_to_send = 0xA5;
    uint8_t slave_received = 0;

    // Start slave thread
    thread slave_thread(I2CSlave, slave_address, ref(slave_received));

    // Give slave time to be ready
    this_thread::sleep_for(chrono::milliseconds(10));

    // Master sends byte
    I2CMaster_Write(slave_address, data_to_send);

    // Wait for slave to finish
    slave_thread.join();

    cout << "[MAIN] Slave received final data: 0x" 
         << hex << (int)slave_received << endl;

    cout << "==== I2C Simulation Complete ====" << endl;
    return 0;
}

/*
===============================================================================
Concepts Demonstrated:

1. Start/Stop conditions (SDA changes while SCL high)
2. 7-bit address recognition and ACK/NACK
3. Bit-by-bit data transfer (MSB-first)
4. Master-Slave synchronization using clock (SCL)
5. Mutex simulates exclusive access to shared bus
6. Delay simulates real-world timing

Real-world relevance:
- Sensors, EEPROMs, RTCs use I2C.
- Master drives clock; slave must respond in sync.
- Proper ACK/NACK ensures reliable communication.
===============================================================================
*/
