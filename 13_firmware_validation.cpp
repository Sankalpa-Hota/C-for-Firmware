/******************************************************
 * Filename: 13_firmware_validation.cpp
 * Purpose: Demonstrates firmware debugging & validation
 *          using a simulated UART peripheral.
 *          Shows unit testing, flag checks, and logging.
 ******************************************************/

#include <iostream>
#include <queue>
#include <cassert>
using namespace std;

// Simulated UART registers
struct UART {
    queue<char> txBuffer;
    bool txReady = true;
} UART1;

// Function to send a character
void UART_SendChar(char data) {
    if (UART1.txReady) {
        UART1.txBuffer.push(data);
        UART1.txReady = false; // busy
        cout << "[DEBUG] UART queued char: " << data << endl;
        UART1.txReady = true;  // mark ready again
    }
}

// Unit test for UART_SendChar
void test_UART_SendChar() {
    UART1.txBuffer = queue<char>();  // clear buffer
    UART1.txReady = true;
    
    UART_SendChar('A');
    assert(!UART1.txBuffer.empty());            // buffer should have data
    assert(UART1.txBuffer.front() == 'A');     // data should match
    cout << "[TEST PASSED] UART_SendChar works correctly\n";
}

int main() {
    cout << "=== Firmware Validation Demo ===\n";
    
    test_UART_SendChar();   // run unit test
    
    cout << "=== Demo Complete ===\n";
    return 0;
}
