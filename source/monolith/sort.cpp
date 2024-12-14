#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

// Генерация случайного массива
std::vector<int> GenerateRandomArray(int size) {
  std::vector<int> array(size);
  for (int& num : array) {
    num = rand() % 100;  // Числа от 0 до 99
  }
  return array;
}

// Функция для вывода массива
void PrintArray(const std::vector<int>& array, const std::string& label) {
  std::cout << label << ": ";
  for (int num : array) {
    std::cout << num << " ";
  }
  std::cout << std::endl;
}

int main(int argc, char* argv[]) {
  // Проверяем количество аргументов
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <array_size> <repetitions>" << std::endl;
    return 1;
  }

  // Получаем параметры из командной строки
  int array_size = std::stoi(argv[1]);
  int repetitions = std::stoi(argv[2]);

  if (array_size <= 0 || repetitions <= 0) {
    std::cerr << "Both parameters must be positive integers." << std::endl;
    return 1;
  }

  // Настраиваем генератор случайных чисел
  srand(static_cast<unsigned>(time(nullptr)));

  for (int i = 0; i < repetitions; ++i) {
    // Генерируем случайный массив
    std::vector<int> numbers = GenerateRandomArray(array_size);

    // Выводим исходный массив (опционально, для отладки)
    PrintArray(numbers, "Original array");

    // Сортируем массив и измеряем время
    clock_t start_time = clock();
    std::sort(numbers.begin(), numbers.end());
    clock_t end_time = clock();

    // Выводим отсортированный массив (опционально, для отладки)
    PrintArray(numbers, "Sorted array");

    // Выводим время выполнения
    double elapsed_time = double(end_time - start_time) / CLOCKS_PER_SEC;
    std::cout << "Iteration " << (i + 1) << ": Sorting completed in " << elapsed_time << " seconds."
              << std::endl;

    std::cout << "---------------------------------" << std::endl;
  }

  return 0;
}
