#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <climits>  

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


void printArray(const std::vector<int>& data) {
    for (int num : data) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
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

  
    std::ofstream outFile(filename);
    for (int num : data) {
        outFile << num << std::endl;
    }
    outFile.close();
}


void mergeData(const std::string& outputFile, const std::vector<std::string>& inputFiles) {
    std::vector<std::ifstream> fileStreams;
    for (const auto& file : inputFiles) {
        fileStreams.push_back(std::ifstream(file));
    }

    std::ofstream outFile(outputFile);
    std::vector<int> currentNumbers(inputFiles.size());

    for (size_t i = 0; i < currentNumbers.size(); ++i) {
        fileStreams[i] >> currentNumbers[i];
    }

    while (true) {
        int minValue = INT_MAX;
        int minIndex = -1;

       
        for (size_t i = 0; i < currentNumbers.size(); ++i) {
            if (fileStreams[i] && currentNumbers[i] < minValue) {
                minValue = currentNumbers[i];
                minIndex = i;
            }
        }

        if (minIndex == -1) {
            break; 
                    }

        outFile << minValue << std::endl;
        fileStreams[minIndex] >> currentNumbers[minIndex]; 
    }

    outFile.close();
    for (auto& fs : fileStreams) {
        fs.close();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <number of elements> <number of repetitions>" << std::endl;
        return 1;
    }

    int numElements = std::stoi(argv[1]);
    int repetitions = std::stoi(argv[2]);

    srand(time(0)); 

    std::string inputFile = "input.txt";
    std::string outputFile = "output.txt";
    std::vector<std::string> tempFiles;

    clock_t startTime = clock();

    for (int i = 0; i < repetitions; ++i) {
        
        generateData(inputFile, numElements);

        std::ifstream inFile(inputFile);
        std::vector<int> data;
        int num;
        while (inFile >> num) {
            data.push_back(num);
        }

       
        std::cout << "Initial array: ";
        printArray(data);

        
        int numParts = 4; 
        int chunkSize = numElements / numParts;

        tempFiles.clear();
        for (int j = 0; j < numParts; ++j) {
            std::string tempFile = "temp" + std::to_string(j) + ".txt";
            tempFiles.push_back(tempFile);

            if (fork() == 0) {
                
                std::ifstream inFile(inputFile);
                std::ofstream outFile(tempFile);
                int count = 0;
                while (inFile >> num && count < chunkSize) {
                    outFile << num << std::endl;
                    count++;
                }
                inFile.close();
                outFile.close();
                sortData(tempFile);
                exit(0); 
            }
        }

       
        for (int j = 0; j < numParts; ++j) {
            wait(NULL);
        }

        
        mergeData(outputFile, tempFiles);

        
        inFile.open(outputFile);
        data.clear();
        while (inFile >> num) {
            data.push_back(num);
        }

        
        std::cout << "Sorted array: ";
        printArray(data);
    }

    clock_t endTime = clock();
    double duration = double(endTime - startTime) / CLOCKS_PER_SEC;

    std::cout << "Execution time: " << duration << " seconds." << std::endl;

    return 0;
}
