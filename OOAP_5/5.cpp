#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <chrono>
#include <mutex>

// Клас Logger реалізує шаблон "Одинак" для ведення логів.
// Він забезпечує єдиний екземпляр для запису в файл з підтримкою багатопоточності.
class Logger {
public:
    // Метод для отримання єдиного екземпляра Logger (лінива ініціалізація)
    static Logger& getInstance() {
        static Logger instance; // Єдиний екземпляр класу
        return instance;
    }

    // Метод для запису повідомлення у файл
    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mtx); // Блокуємо м'ютекс для потокобезпеки
        if (!file.is_open()) {
            file.open("sorting_times.txt", std::ios::app); // Відкриваємо файл для запису
        }
        file << message << std::endl; // Записуємо повідомлення у файл
    }

private:
    Logger() {} // Приватний конструктор для заборони створення екземплярів
    ~Logger() {
        if (file.is_open()) file.close(); // Закриваємо файл при знищенні екземпляра
    }
    //Заборона копіювання і присвоєння:
    Logger(const Logger&) = delete; // Забороняємо копіювання
    Logger& operator=(const Logger&) = delete; // Забороняємо присвоєння

    std::ofstream file; // Файл для запису часу сортування
    std::mutex mtx; // М'ютекс для потокобезпечного запису
};

// Функція для запису часу виконання сортування конкретного алгоритму
void logSortTime(const std::string& algorithmName, const std::chrono::milliseconds& duration) {
    Logger::getInstance().log(algorithmName + ": " + std::to_string(duration.count()) + " ms");
}

// Реалізація сортування бульбашкою
void bubbleSort(std::vector<int> arr) {
    auto start = std::chrono::high_resolution_clock::now(); // Починаємо вимір часу
    // Проходимо по всіх елементах масиву
    for (size_t i = 0; i < arr.size() - 1; i++) {
        for (size_t j = 0; j < arr.size() - i - 1; j++) {
            // Якщо поточний елемент більше наступного, міняємо їх місцями
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now(); // Завершуємо вимір часу
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    logSortTime("Bubble Sort", duration); // Записуємо час виконання в лог
}

// Реалізація сортування Шелла
void shellSort(std::vector<int> arr) {
    auto start = std::chrono::high_resolution_clock::now(); // Починаємо вимір часу
    // Поступове зменшення проміжку для сортування
    for (int gap = arr.size() / 2; gap > 0; gap /= 2) {
        // Сортування елементів на відстані gap
        for (size_t i = gap; i < arr.size(); i++) {
            int temp = arr[i];
            size_t j;
            // Переміщуємо елементи в межах від gap
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
    auto end = std::chrono::high_resolution_clock::now(); // Завершуємо вимір часу
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    logSortTime("Shell Sort", duration); // Записуємо час виконання в лог
}

// Функція для швидкого сортування (рекурсивна) 
void quickSort(std::vector<int>& arr, int left, int right) {
    int i = left, j = right;
    int pivot = arr[(left + right) / 2]; // Вибір опорного елемента
    while (i <= j) {
        // Знаходимо елементи, які не на своїх місцях відносно опорного
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        // Міняємо місцями елементи
        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }
    // Рекурсивно сортуємо підмасиви
    if (left < j) quickSort(arr, left, j);
    if (i < right) quickSort(arr, i, right);
}

// Обгортка для функції швидкого сортування, що вимірює час виконання
void quickSortWrapper(std::vector<int> arr) {
    auto start = std::chrono::high_resolution_clock::now(); // Починаємо вимір часу
    quickSort(arr, 0, arr.size() - 1); // Викликаємо швидке сортування
    auto end = std::chrono::high_resolution_clock::now(); // Завершуємо вимір часу
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    logSortTime("Quick Sort", duration); // Записуємо час виконання в лог
}

int main() {
    setlocale(LC_ALL, "ukr");
    // Ініціалізація масиву для сортування
    std::vector<int> arr = { 34, 7, 23, 32, 5, 62, 32, 45, 3, 1 };

    // Створення потоків для кожного алгоритму сортування
    std::thread t1(bubbleSort, arr); // Потік для сортування бульбашкою
    std::thread t2(shellSort, arr);  // Потік для сортування Шелла
    std::thread t3(quickSortWrapper, arr); // Потік для швидкого сортування

    // Очікуємо завершення всіх потоків
    t1.join();
    t2.join();
    t3.join();

    std::cout << "Сортування завершено. Час сортування записано у файл sorting_times.txt" << std::endl;
    return 0;
}
