#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <chrono>
#include <mutex>

// ���� Logger ������ ������ "������" ��� ������� ����.
// ³� ��������� ������ ��������� ��� ������ � ���� � ��������� ���������������.
class Logger {
public:
    // ����� ��� ��������� ������� ���������� Logger (����� �����������)
    static Logger& getInstance() {
        static Logger instance; // ������ ��������� �����
        return instance;
    }

    // ����� ��� ������ ����������� � ����
    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mtx); // ������� �'����� ��� �������������
        if (!file.is_open()) {
            file.open("sorting_times.txt", std::ios::app); // ³�������� ���� ��� ������
        }
        file << message << std::endl; // �������� ����������� � ����
    }

private:
    Logger() {} // ��������� ����������� ��� �������� ��������� ����������
    ~Logger() {
        if (file.is_open()) file.close(); // ��������� ���� ��� ������� ����������
    }
    //�������� ��������� � ���������:
    Logger(const Logger&) = delete; // ����������� ���������
    Logger& operator=(const Logger&) = delete; // ����������� ���������

    std::ofstream file; // ���� ��� ������ ���� ����������
    std::mutex mtx; // �'����� ��� ���������������� ������
};

// ������� ��� ������ ���� ��������� ���������� ����������� ���������
void logSortTime(const std::string& algorithmName, const std::chrono::milliseconds& duration) {
    Logger::getInstance().log(algorithmName + ": " + std::to_string(duration.count()) + " ms");
}

// ��������� ���������� ����������
void bubbleSort(std::vector<int> arr) {
    auto start = std::chrono::high_resolution_clock::now(); // �������� ���� ����
    // ��������� �� ��� ��������� ������
    for (size_t i = 0; i < arr.size() - 1; i++) {
        for (size_t j = 0; j < arr.size() - i - 1; j++) {
            // ���� �������� ������� ����� ����������, ������ �� ������
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now(); // ��������� ���� ����
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    logSortTime("Bubble Sort", duration); // �������� ��� ��������� � ���
}

// ��������� ���������� �����
void shellSort(std::vector<int> arr) {
    auto start = std::chrono::high_resolution_clock::now(); // �������� ���� ����
    // ��������� ��������� ������� ��� ����������
    for (int gap = arr.size() / 2; gap > 0; gap /= 2) {
        // ���������� �������� �� ������ gap
        for (size_t i = gap; i < arr.size(); i++) {
            int temp = arr[i];
            size_t j;
            // ��������� �������� � ����� �� gap
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
    auto end = std::chrono::high_resolution_clock::now(); // ��������� ���� ����
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    logSortTime("Shell Sort", duration); // �������� ��� ��������� � ���
}

// ������� ��� �������� ���������� (����������) 
void quickSort(std::vector<int>& arr, int left, int right) {
    int i = left, j = right;
    int pivot = arr[(left + right) / 2]; // ���� �������� ��������
    while (i <= j) {
        // ��������� ��������, �� �� �� ���� ����� ������� ��������
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        // ̳����� ������ ��������
        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }
    // ���������� ������� ��������
    if (left < j) quickSort(arr, left, j);
    if (i < right) quickSort(arr, i, right);
}

// �������� ��� ������� �������� ����������, �� ������ ��� ���������
void quickSortWrapper(std::vector<int> arr) {
    auto start = std::chrono::high_resolution_clock::now(); // �������� ���� ����
    quickSort(arr, 0, arr.size() - 1); // ��������� ������ ����������
    auto end = std::chrono::high_resolution_clock::now(); // ��������� ���� ����
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    logSortTime("Quick Sort", duration); // �������� ��� ��������� � ���
}

int main() {
    setlocale(LC_ALL, "ukr");
    // ����������� ������ ��� ����������
    std::vector<int> arr = { 34, 7, 23, 32, 5, 62, 32, 45, 3, 1 };

    // ��������� ������ ��� ������� ��������� ����������
    std::thread t1(bubbleSort, arr); // ���� ��� ���������� ����������
    std::thread t2(shellSort, arr);  // ���� ��� ���������� �����
    std::thread t3(quickSortWrapper, arr); // ���� ��� �������� ����������

    // ������� ���������� ��� ������
    t1.join();
    t2.join();
    t3.join();

    std::cout << "���������� ���������. ��� ���������� �������� � ���� sorting_times.txt" << std::endl;
    return 0;
}
