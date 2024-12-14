#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>


std::vector<int> GenerateRandomArray(int size) {
  std::vector<int> array(size);
  for (int& num : array) {
    num = rand() % 100;
  }
  return array;
}

void PrintArray(const std::vector<int>& array, const std::string& label) {
  std::cout << label << ": ";
  for (int num : array) {
    std::cout << num << " ";
  }
  std::cout << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <array_size> <repetitions>" << std::endl;
    return 1;
  }

  int array_size = std::stoi(argv[1]);
  int repetitions = std::stoi(argv[2]);

  if (array_size <= 0 || repetitions <= 0) {
    std::cerr << "Both parameters must be positive integers." << std::endl;
    return 1;
  }

  srand(static_cast<unsigned>(time(nullptr)));

  for (int i = 0; i < repetitions; ++i) {
    std::vector<int> numbers = GenerateRandomArray(array_size);

    PrintArray(numbers, "Original array");

    clock_t start_time = clock();
    std::sort(numbers.begin(), numbers.end());
    clock_t end_time = clock();

    PrintArray(numbers, "Sorted array");

    double elapsed_time = double(end_time - start_time) / CLOCKS_PER_SEC;
    std::cout << "Iteration " << (i + 1) << ": Sorting completed in " << elapsed_time << " seconds."
              << std::endl;

    std::cout << "---------------------------------" << std::endl;
  }

  return 0;
}
