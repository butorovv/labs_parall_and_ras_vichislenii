#include <iostream>
#include <vector>
#include <boost/thread.hpp>
#include <chrono>


const int N = 1000;
std::vector<std::vector<int>> A(N, std::vector<int>(N, 1));
std::vector<std::vector<int>> B(N, std::vector<int>(N, 1));
std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));


void mult_row(int row) {
    for (int col = 0; col < N; ++col) {
        for (int k = 0; k < N; ++k) {
            C[row][col] += A[row][k] * B[k][col];
        }
    }
}


void parallel_mult(int num_threads) {
    boost::thread_group threads;
    for (int i = 0; i < N; i += N / num_threads) {
        for (int row = i; row < i + N / num_threads; ++row) {
            threads.create_thread(boost::bind(mult_row, row));
        }
    }
    threads.join_all();
}


void single_thread_mult() {
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            for (int k = 0; k < N; ++k) {
                C[row][col] += A[row][k] * B[k][col];
            }
        }
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    single_thread_mult();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_single = end - start;
    std::vector<int> thread_counts = { 2, 4, 8 };
    std::vector<double> elapsed_times;

    for (int num_threads : thread_counts) {
        C = std::vector<std::vector<int>>(N, std::vector<int>(N, 0));
        auto start = std::chrono::high_resolution_clock::now();
        parallel_mult(num_threads);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        elapsed_times.push_back(elapsed.count());
    }


    std::cout << "Для N = " << N << ":\n";
    std::cout << "Однопоточное выполнение: " << elapsed_single.count() << " секунд\n";
    for (size_t i = 0; i < thread_counts.size(); ++i) {
        std::cout << "Многопоточное выполнение (" << thread_counts[i] << " потока): " << elapsed_times[i] << " секунд\n";

    }

    return 0;
}