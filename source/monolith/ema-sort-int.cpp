#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>
#include <chrono>

void generateData(const std::string& filename, int size) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error opening file for writing!" << std::endl;
        exit(1);
    }

    for (int i = 0; i < size; ++i) {
        outFile << rand() % 1000000 << std::endl; 
    }

    outFile.close();
}

void sortData(const std::string& filename) {
    std::ifstream inFile(filename);
    std::vector<int> data;
    int number;

    while (inFile >> number) {
        data.push_back(number);
    }

    inFile.close();

    std::sort(data.begin(), data.end());

    // тут отсортированные данные обратно в файл
    std::ofstream outFile(filename);
    for (int num : data) {
        outFile << num << std::endl;
    }
    outFile.close();
}

void printArray(const std::string& filename) {
    std::ifstream inFile(filename);
    int number;

    while (inFile >> number) {
        std::cout << number << " ";
    }

    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <number of elements> <number of repetitions>" << std::endl;
        return 1;
    }

    int n = std::stoi(argv[1]);  
    int repetitions = std::stoi(argv[2]);  

    std::string filename = "input.txt";

    generateData(filename, n);

    std::cout << "Initial array: ";
    printArray(filename);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < repetitions; ++i) {
        pid_t pid = fork();  // ВОТ ТУТ дочерний процесс начинается 

        if (pid == -1) { 
            std::cerr << "Fork failed!" << std::endl;
            return 1;
        }

        if (pid == 0) {  // Дочерний 
            std::cout << "Initial array (iteration " << i + 1 << "): ";
            printArray(filename);

            sortData(filename);  

            std::cout << "Sorted array (iteration " << i + 1 << "): ";
            printArray(filename);

            exit(0);  // завершается
        } else {  // А ТУТ родительский
            wait(NULL);  // чето ждем
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Execution time: " << duration.count() << " seconds." << std::endl;

    return 0;
}
