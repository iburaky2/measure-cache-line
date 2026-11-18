#include <algorithm>
#include <cstddef>
#include <new>
#include <print>
#include <thread>
#include <vector>

#include <pthread.h>
#include <sched.h>


template <size_t N>
struct alignas(256) shared_data {
    uint8_t data1{0};
    alignas(N) uint8_t data2{0};
};

void modify_data(uint8_t& data, uint64_t core) {
    // Pin thread to core
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(core, &set);

    pthread_setaffinity_np(
        pthread_self(),
        sizeof(cpu_set_t),
        &set
    );

    // Modify data in the shared struct. If data1 and data2 are in the 
    // same cache line the other core's cache will be invalid forcing
    // a refetch
    for (size_t i{0}; i < 1'000'000'000; ++i) {
        ++data;
    }
}

template <size_t N>
void measure() {
    // Measure 10 times, report lowest one in order to reduce noise
    std::vector<std::chrono::microseconds> diffs;
    for (size_t i{0}; i < 10; ++i) {
        shared_data<N> SharedData;

        auto begin = std::chrono::steady_clock::now();
        std::thread t1(modify_data, std::ref(SharedData.data1), 2);
        std::thread t2(modify_data, std::ref(SharedData.data2), 3);

        t1.join();
        t2.join();
        auto end = std::chrono::steady_clock::now();

        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        diffs.push_back(diff);
    }

    // Report results
    std::sort(diffs.begin(), diffs.end());
    std::println("Second byte aligned to {:>3}, ticks: {:>10}", N, diffs[0].count());
}

int main() {
    // Print values from C++ implementation
    std::println("std::hardware_constructive_interference_size: {} bytes", std::hardware_constructive_interference_size);
    std::println("std::hardware_destructive_interference_size: {} bytes", std::hardware_destructive_interference_size);

    // Probably a better way to do this
    // I need to work on my template metaprogramming skills
    measure<1>();
    measure<2>();
    measure<4>();
    measure<8>();
    measure<16>();
    measure<32>();
    measure<64>();
    measure<128>();

    return 0;
}