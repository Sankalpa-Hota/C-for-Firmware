/******************************************************
 * Filename: 14_rtos_advanced.cpp
 * Purpose: Demonstrates RTOS advanced concepts:
 * - Multi-threading
 * - Mutex and semaphore usage
 * - Pipelining tasks
 * - Deadlock avoidance
 ******************************************************/

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <condition_variable>
using namespace std;

// --------------------------- Global Resources -----------------------------
mutex uart_mutex;          // Protect UART
mutex sensor_mutex;        // Protect sensor access

// Semaphore simulation (counting)
class Semaphore {
    int count;
    mutex mtx;
    condition_variable cv;
public:
    Semaphore(int init = 1) : count(init) {}
    void wait() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [&]() { return count > 0; });
        count--;
    }
    void signal() {
        unique_lock<mutex> lock(mtx);
        count++;
        cv.notify_one();
    }
};
Semaphore pipeline_slots(2); // only 2 slots can process at a time

// --------------------------- Tasks -----------------------------
void task_sensor_read(queue<int> &dataQueue) {
    int sensor_value = 0;
    while(sensor_value < 5) { // simulate 5 readings
        {
            lock_guard<mutex> lock(sensor_mutex); // protect sensor
            sensor_value++;
            cout << "[SENSOR] Read value: " << sensor_value << endl;
        }
        pipeline_slots.wait(); // wait for pipeline slot
        dataQueue.push(sensor_value);
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void task_data_process(queue<int> &dataQueue) {
    while(true) {
        if(!dataQueue.empty()) {
            int val = dataQueue.front();
            dataQueue.pop();
            int processed = val * 2; // dummy processing
            cout << "[PROCESS] Processed data: " << processed << endl;
            pipeline_slots.signal(); // free slot
        } else {
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
}

void task_uart_send(queue<int> &dataQueue) {
    while(true) {
        if(!dataQueue.empty()) {
            int val = dataQueue.front();
            dataQueue.pop();
            lock_guard<mutex> lock(uart_mutex); // protect UART
            cout << "[UART] Sending data: " << val << endl;
            this_thread::sleep_for(chrono::milliseconds(50));
        } else {
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
}

// --------------------------- Main -----------------------------
int main() {
    cout << "=== RTOS Advanced Simulation ===" << endl;

    queue<int> pipelineQueue;

    // Multi-threaded pipeline
    thread sensor(task_sensor_read, ref(pipelineQueue));
    thread process(task_data_process, ref(pipelineQueue));
    thread uart(task_uart_send, ref(pipelineQueue));

    // Wait for sensor to finish
    sensor.join();

    // Wait some time for remaining data to be processed and sent
    this_thread::sleep_for(chrono::seconds(2));

    // Detach other threads for demo
    process.detach();
    uart.detach();

    cout << "=== RTOS Advanced Demo Complete ===" << endl;
    return 0;
}
