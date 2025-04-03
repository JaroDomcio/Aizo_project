#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <type_traits>
#include <algorithm>
#include <functional>

using namespace std;

template<typename T>
class ShellSort {
public:
    void sort(T arr[], int n) {
        for (int gap = n / 2; gap > 0; gap /= 2) {
            for (int i = gap; i < n; i++) {
                T temp = arr[i];
                int j;
                for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                    arr[j] = arr[j - gap];
                }
                arr[j] = temp;
            }
        }
    }
};


template<typename T>
void testShellSort(const string& typeName) {
    ShellSort<T> sorter;
    const int tab[] = {50000, 100000, 200000, 500000, 750000, 1000000, 1500000};
    const int k = 20;

    auto generate = [](int size) {
        T* arr = new T[size];
        for (int i = 0; i < size; i++) {
            if constexpr (is_same<T, int>::value)
                arr[i] = rand() % 2147483647;
            else
                arr[i] = static_cast<T>(rand()) / RAND_MAX * 4294967296.0f;
        }
        return arr;
    };

    cout << "\nTESTY SHELL SORT DLA TYPU: " << typeName << "\n";

    // LOSOWA
    cout << "\nLosowa tablica" << endl;
    for (int n : tab) {
        double totalTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; ++i) {
            T* arr = generate(n);

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, n);
            auto end = chrono::high_resolution_clock::now();

            totalTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << totalTime / k << " s\n";
    }

    // ROSNĄCA
    cout << "\nPosortowana rosnąca tablica" << endl;
    for (int n : tab) {
        double totalTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; ++i) {
            T* arr = generate(n);
            sort(arr, arr + n);

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, n);
            auto end = chrono::high_resolution_clock::now();

            totalTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << totalTime / k << " s\n";
    }

    // MALEJĄCA
    cout << "\nPosortowana malejąca tablica" << endl;
    for (int n : tab) {
        double totalTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; ++i) {
            T* arr = generate(n);
            sort(arr, arr + n, greater<T>());

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, n);
            auto end = chrono::high_resolution_clock::now();

            totalTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << totalTime / k << " s\n";
    }

    // POSORTOWANA W 33%
    cout << "\nPosortowana w 33%" << endl;
    for (int n : tab) {
        double totalTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; ++i) {
            T* arr = generate(n);
            int portion = static_cast<int>(n * 0.33);
            sort(arr, arr + portion);

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, n);
            auto end = chrono::high_resolution_clock::now();

            totalTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << totalTime / k << " s\n";
    }

    // POSORTOWANA W 66%
    cout << "\nPosortowana w 66%" << endl;
    for (int n : tab) {
        double totalTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; ++i) {
            T* arr = generate(n);
            int portion = static_cast<int>(n * 0.66);
            sort(arr, arr + portion);

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, n);
            auto end = chrono::high_resolution_clock::now();

            totalTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << totalTime / k << " s\n";
    }
}


void demoSortUI() {
    ShellSort<int> sorter;
    const string filename = "dane.txt";

    ifstream file(filename);
    if (!file) {
        cout << "Nie można otworzyć pliku: " << filename << endl;
        return;
    }

    string line;
    getline(file, line);
    file.close();

    istringstream iss(line);
    int num, count = 0;
    while (iss >> num) count++;

    if (count == 0) {
        cout << "Plik nie zawiera liczb.\n";
        return;
    }

    istringstream iss2(line);
    int* arr = new int[count];
    for (int i = 0; i < count; ++i)
        iss2 >> arr[i];

    cout << "\nTablica przed sortowaniem:\n";
    for (int i = 0; i < count; ++i)
        cout << arr[i] << " ";
    cout << "\n";

    sorter.sort(arr, count);

    cout << "\nTablica po sortowaniu:\n";
    for (int i = 0; i < count; ++i)
        cout << arr[i] << " ";
    cout << "\n";

    delete[] arr;
}


int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    int option;
    do {
        cout << "\n========== MENU ==========\n";
        cout << "1. Sortowanie tablicy z pliku 'dane.txt'\n";
        cout << "2. Testy ShellSort dla int i float\n";
        cout << "0. Wyjście\n";
        cout << "Wybierz opcję: ";
        cin >> option;

        switch (option) {
            case 1:
                demoSortUI();
                break;
            case 2:
                testShellSort<int>("int");
                testShellSort<float>("float");
                break;
            case 0:
                cout << "Zamykanie programu...\n";
                break;
            default:
                cout << "Nieznana opcja.\n";
        }
    } while (option != 0);

    return 0;
}

