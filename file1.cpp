#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

const int BUFFER_CAPACITY = 2; // Capacity of the shared buffer

std::vector<int> buffer; // Shared buffer
std::mutex mtx;          // Mutex for critical sections
std::condition_variable producer_cv, consumer_cv; // Condition variables for synchronization

void producer() {
    for (int i = 1; i <= 5; ++i) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait if the buffer is full
        producer_cv.wait(lock, [] { return buffer.size() < BUFFER_CAPACITY; });

        buffer.push_back(i);
        std::cout << "Produced: " << i << ", Buffer Size: " << buffer.size() << std::endl;

        lock.unlock();
        consumer_cv.notify_one(); // Notify a waiting consumer
    }
}

void consumer(int id) {
    for (int i = 0; i < 5; ++i) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait if the buffer is empty
        consumer_cv.wait(lock, [] { return !buffer.empty(); });

        int item = buffer.back();
        buffer.pop_back();
        std::cout << "Consumer " << id << " Consumed: " << item << ", Buffer Size: " << buffer.size() << std::endl;

        lock.unlock();
        producer_cv.notify_one(); // Notify a waiting producer
    }
}

int main() {
    std::thread producerThread(producer);
    std::thread consumerThread1(consumer, 1);
    std::thread consumerThread2(consumer, 2);

    producerThread.join();
    consumerThread1.join();
    consumerThread2.join();

    return 0;
}
