#include <iostream>
#include <chrono>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <fstream>
#include <sstream>

using namespace std;


template<typename T>
class InsertionSort {
public:
    void sort(T* arr, int n) {
        for (int i = 1; i < n; i++) {
            T key = arr[i];
            int j = i - 1;
            while (j >= 0 && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }
};


template<typename T>
void testInsertionSort(const string& typeName) {
    InsertionSort<T> sorter;
    const int tab[] = {1000000, 2000000, 5000000,8000000, 10000000, 15000000,20000000};
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

    cout << "\nTESTY DLA TYPU: " << typeName << "\n";

    // LOSOWA TABLICA 
    cout << "\nLosowa tablica" << endl;
    for (int n : tab) {
        double sumTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; i++) {
            T* arr = generate(n);

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, n);
            auto end = chrono::high_resolution_clock::now();

            sumTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << sumTime / k << " s" << endl;
    }

    // POSORTOWANA ROSNĄCO
    cout << "\nPosortowana rosnąca tablica" << endl;
    for (int n : tab) {
        double sumTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; i++) {
            T* arr = generate(n);
            sort(arr, arr + n);

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, n);
            auto end = chrono::high_resolution_clock::now();

            sumTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << sumTime / k << " s" << endl;
    }

    //POSORTOWANA MALEJĄCO
    cout << "\nPosortowana malejąca tablica" << endl;
    for (int n : tab) {
        double sumTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; i++) {
            T* arr = generate(n);
            sort(arr, arr + n, greater<T>());

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, n);
            auto end = chrono::high_resolution_clock::now();

            sumTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << sumTime / k << " s" << endl;
    }

    //POSORTOWANA W 33% 
    cout << "\nPosortowana w 33%" << endl;
    for (int n : tab) {
        double sumTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; i++) {
            T* arr = generate(n);
            int sortedPortion = static_cast<int>(n * 0.33);
            sort(arr, arr + sortedPortion);

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, n);
            auto end = chrono::high_resolution_clock::now();

            sumTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << sumTime / k << " s" << endl;
    }

    //POSORTOWANA W 66% 
    cout << "\nPosortowana w 66%" << endl;
    for (int n : tab) {
        double sumTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; i++) {
            T* arr = generate(n);
            int sortedPortion = static_cast<int>(n * 0.66);
            sort(arr, arr + sortedPortion);

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, n);
            auto end = chrono::high_resolution_clock::now();

            sumTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << sumTime / k << " s" << endl;
    }
}


void demoSortUI() {
    InsertionSort<int> sorter;
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
    while (iss >> num)
        count++;

    if (count == 0) {
        cout << "Plik nie zawiera żadnych liczb.\n";
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
        cout << "2. Testy InsertionSort (int, float)\n";
        cout << "0. Wyjście\n";
        cout << "Wybierz opcję: ";
        cin >> option;

        switch (option) {
            case 1:
                demoSortUI();
                break;
            case 2:
                testInsertionSort<int>("int");
                testInsertionSort<float>("float");
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
