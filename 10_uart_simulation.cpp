/*
===============================================================================
Purpose: Demonstrate UART (Universal Asynchronous Receiver/Transmitter)
         fundamentals using a C++ simulation.
         - Shows TX/RX buffers
         - Simulates wire transfer delay
         - Uses mutex for safe register access (as if hardware registers)

How to compile & run:
  g++ 10_uart_simulation.cpp -o uart_demo -std=c++11 -pthread
  ./uart_demo
===============================================================================
*/

#include <iostream>   // For printing to console
#include <queue>      // To simulate hardware transmit/receive FIFOs
#include <thread>     // To simulate concurrent hardware behavior
#include <chrono>     // To add timing delays
#include <mutex>      // To safely access shared "registers"
using namespace std;

// -----------------------------------------------------------------------------
// SECTION 1: Simulated UART Registers
// -----------------------------------------------------------------------------
/*
In real hardware, UART has memory-mapped registers such as:
  - TXDATA  (data to transmit)
  - RXDATA  (data received)
  - STATUS  (flags)
  - CTRL    (configuration)
We simulate those here with C++ variables.
*/
struct UART_Registers {
    queue<char> txBuffer;   // Simulates transmit FIFO (TX register)
    queue<char> rxBuffer;   // Simulates receive FIFO (RX register)
    bool txReady = true;    // True means hardware is ready to send next byte
    bool rxReady = false;   // True means new data has arrived
    mutex uartLock;         // Protects access to buffers, like disabling IRQs
} UART;                     // Create global UART object (one peripheral)

// -----------------------------------------------------------------------------
// SECTION 2: UART Transmit Function
// -----------------------------------------------------------------------------
/*
UART_SendChar() imitates writing a byte into TX register.
When firmware calls this, data is enqueued and hardware
would normally start shifting bits out.
*/
void UART_SendChar(char data) {
    lock_guard<mutex> lock(UART.uartLock);     // ① ensure exclusive access
    if(UART.txReady) {                         // ② if transmitter idle
        UART.txBuffer.push(data);              // ③ put byte into TX FIFO
        UART.txReady = false;                  // ④ mark busy
        cout << "[UART] Sending char: " << data << endl;
        this_thread::sleep_for(chrono::milliseconds(100)); // ⑤ simulate TX time
        UART.txReady = true;                   // ⑥ mark ready again
    }
}

// -----------------------------------------------------------------------------
// SECTION 3: UART Receive Function
// -----------------------------------------------------------------------------
/*
UART_ReadChar() imitates reading from RX register.
Returns next received character or 0 if no data.
*/
char UART_ReadChar() {
    lock_guard<mutex> lock(UART.uartLock);     // ① lock RX access
    if(!UART.rxBuffer.empty()) {               // ② data available?
        char data = UART.rxBuffer.front();     // ③ get next byte
        UART.rxBuffer.pop();                   // ④ remove it from FIFO
        UART.rxReady = !UART.rxBuffer.empty(); // ⑤ update status flag
        cout << "[UART] Received char: " << data << endl;
        return data;
    }
    return 0; // no data present (like polling RXREADY=0)
}

// -----------------------------------------------------------------------------
// SECTION 4: Simulated Physical Wire
// -----------------------------------------------------------------------------
/*
In reality, UART hardware shifts bits from TX line to RX line.
Here we model that as a background thread that periodically
moves bytes from TX buffer to RX buffer.
*/
void UART_TransferWire() {
    while(true) {                                       // runs "forever"
        this_thread::sleep_for(chrono::milliseconds(150)); // simulate baud delay
        lock_guard<mutex> lock(UART.uartLock);          // protect both buffers
        if(!UART.txBuffer.empty()) {                    // any data to send?
            char data = UART.txBuffer.front();          // read byte from TX FIFO
            UART.txBuffer.pop();                        // remove from TX FIFO
            UART.rxBuffer.push(data);                   // deliver to RX FIFO
            UART.rxReady = true;                        // set RX flag
            cout << "[WIRE] Transferred: " << data << endl;
        }
    }
}

// -----------------------------------------------------------------------------
// SECTION 5: MAIN FUNCTION (Firmware Entry Point)
// -----------------------------------------------------------------------------
int main() {
    cout << "==== UART Firmware Simulation ====" << endl;

    // Start "hardware wire" thread — runs concurrently
    thread wire(UART_TransferWire);

    // Firmware sends three characters (like printf over UART)
    UART_SendChar('H');
    UART_SendChar('i');
    UART_SendChar('!');

    // Give time for the wire to transfer them
    this_thread::sleep_for(chrono::milliseconds(500));

    // Firmware checks RX register for incoming data
    UART_ReadChar();
    UART_ReadChar();
    UART_ReadChar();

    // Detach wire thread (end simulation)
    wire.detach();
    cout << "==== UART Demo Complete ====" << endl;
    return 0;
}

/*
===============================================================================
SUMMARY / LEARNING POINTS
===============================================================================
1. UART is asynchronous → no clock line; both sides agree on baud rate.
2. TX buffer = data to send; RX buffer = data received.
3. Firmware writes TX register (here: txBuffer.push()).
4. A background process (hardware or ISR) moves bits to RX.
5. Firmware reads RX register when RXREADY flag is set.
6. Mutex here acts like interrupt disable/enable around critical sections.

Real-world analogy:
   MCU TX pin → Serial cable → Peripheral RX pin.
   Each transfer consists of start bit, 8 data bits, optional parity, stop bit.

Firmware relevance:
   Understanding TX/RX timing and flag polling is key for writing
   low-level drivers for UART peripherals in embedded systems.
===============================================================================
*/
