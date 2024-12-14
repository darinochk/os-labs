#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <string>
#include <sstream>


void runEmaSort(int arg1, int arg2) {
    std::string command = "./ema-sort-int " + std::to_string(arg1) + " " + std::to_string(arg2);
    int result = system(command.c_str());

    if (result != 0) {
        std::cerr << "Ошибка при выполнении команды: " << command << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " <количество_потоков>" << std::endl;
        return 1;
    }

    int threadCount;
    std::istringstream iss(argv[1]);
    if (!(iss >> threadCount) || threadCount <= 0) {
        std::cerr << "Неверное количество потоков. Укажите положительное целое число." << std::endl;
        return 1;
    }

    const int arg1 = 10;
    const int arg2 = 1;

    std::vector<std::thread> threads;

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(runEmaSort, arg1, arg2);
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    std::cout << "Все потоки завершили выполнение." << std::endl;
    return 0;
}
