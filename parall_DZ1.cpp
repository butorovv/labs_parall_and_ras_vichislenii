#include <boost/thread.hpp>
#include <queue>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <future>

// Определение типов задач
enum class TaskType {
    Factorial,
    Fibonacci,
    SumOfDigits,
    IsPrime,
    GCD,
    ReverseNumber
};

// Структура задачи
struct Task {
    TaskType type;
    int n;
    int m; // Для задачи НОД
};

// Очередь задач
std::queue<Task> taskQueue;
std::mutex queueMutex;
std::condition_variable queueCondVar;
bool done = false;

// Функции для выполнения задач
unsigned long long factorial(int n) {
    if (n == 0) return 1;
    unsigned long long result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

unsigned long long fibonacci(int n) {
    if (n <= 1) return n;
    unsigned long long a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        unsigned long long c = a + b;
        a = b;
        b = c;
    }
    return b;
}

int sumOfDigits(int n) {
    int sum = 0;
    while (n != 0) {
        sum += n % 10;
        n /= 10;
    }
    return sum;
}

bool isPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int reverseNumber(int n) {
    int reversed = 0;
    while (n != 0) {
        reversed = reversed * 10 + n % 10;
        n /= 10;
    }
    return reversed;
}

// Функция рабочего потока
void workerThread() {
    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondVar.wait(lock, [] { return !taskQueue.empty() || done; });
            if (done && taskQueue.empty()) {
                break;
            }
            task = taskQueue.front();
            taskQueue.pop();
        }

        switch (task.type) {
            case TaskType::Factorial:
                std::cout << "Factorial of " << task.n << " is " << factorial(task.n) << std::endl;
                break;
            case TaskType::Fibonacci:
                std::cout << "Fibonacci number at position " << task.n << " is " << fibonacci(task.n) << std::endl;
                break;
            case TaskType::SumOfDigits:
                std::cout << "Sum of digits of " << task.n << " is " << sumOfDigits(task.n) << std::endl;
                break;
            case TaskType::IsPrime:
                std::cout << task.n << " is " << (isPrime(task.n) ? "prime" : "not prime") << std::endl;
                break;
            case TaskType::GCD:
                std::cout << "GCD of " << task.n << " and " << task.m << " is " << gcd(task.n, task.m) << std::endl;
                break;
            case TaskType::ReverseNumber:
                std::cout << "Reverse of " << task.n << " is " << reverseNumber(task.n) << std::endl;
                break;
        }
    }
}

int main() {
    // Создание задач
    taskQueue.push({TaskType::Factorial, 10});
    taskQueue.push({TaskType::Fibonacci, 20});
    taskQueue.push({TaskType::SumOfDigits, 12345});
    taskQueue.push({TaskType::IsPrime, 29});
    taskQueue.push({TaskType::GCD, 56, 98});
    taskQueue.push({TaskType::ReverseNumber, 123456});

    // Создание потоков
    unsigned int numThreads = boost::thread::hardware_concurrency();
    std::vector<boost::thread> threads;
    for (unsigned int i = 0; i < numThreads; ++i) {
        threads.emplace_back(workerThread);
    }

    // Замер времени выполнения
    auto start = std::chrono::high_resolution_clock::now();

    // Ожидание завершения всех задач
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        done = true;
    }
    queueCondVar.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Total time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}