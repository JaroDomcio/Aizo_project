#include <iostream>
#include <chrono>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <fstream>
#include <sstream>

using namespace std;

enum PivotType {
    LEFT,
    RIGHT,
    MIDDLE,
    RANDOM
};


template<typename T>
class QuickSort {
public:
    QuickSort(PivotType pivot) : pivotType(pivot) {
        srand(static_cast<unsigned int>(time(0)));
    }

    void sort(T* A, int p, int r) {
        if (p < r) {
            int q = partition(A, p, r);
            sort(A, p, q - 1);
            sort(A, q + 1, r);
        }
    }

private:
    PivotType pivotType;

    int partition(T* A, int p, int r) {
        int pivotIndex = selectPivot(p, r);
        T pivotValue = A[pivotIndex];

        swap(A[pivotIndex], A[r]);
        T x = A[r];
        int i = p - 1;

        for (int j = p; j < r; j++) {
            if (A[j] <= x) {
                i++;
                swap(A[i], A[j]);
            }
        }
        swap(A[i + 1], A[r]);
        return i + 1;
    }

    int selectPivot(int p, int r) {
        switch (pivotType) {
            case LEFT: return p;
            case RIGHT: return r;
            case MIDDLE: return p + (r - p) / 2;
            case RANDOM: return p + rand() % (r - p + 1);
        }
        return r;
    }
};


template<typename T>
void testQuickSort(const string& typeName, PivotType pivotType) {
    QuickSort<T> sorter(pivotType);
    const int tab[] = {1000000, 2000000, 5000000, 8000000, 10000000, 15000000, 20000000};
    const int k = 20;

    auto generate = [](int size) {
        T* arr = new T[size];
        for (int i = 0; i < size; i++) {
            if constexpr (is_same<T, int>::value)
                arr[i] = ((rand() << 15) | rand()) % 2147483647;
            else{
                int r = (rand() << 15) | rand(); // 30-bitowa liczba losowa
                arr[i] = static_cast<float>(r) / static_cast<float>((1 << 30)) * 4294967296.0f;
            }
        }
        return arr;
    };

    cout << "\nTESTY QUICK SORT DLA TYPU: " << typeName << "\n";

    // //LOSOWA TABLICA
    // cout << "\nLosowa tablica" << endl;
    // for (int n : tab) {
    //     double sumTime = 0;
    //     cout << "n = " << n << endl;
    //     for (int i = 0; i < k; i++) {
    //         T* arr = generate(n);

    //         auto start = chrono::high_resolution_clock::now();
    //         sorter.sort(arr, 0, n - 1);
    //         auto end = chrono::high_resolution_clock::now();

    //         sumTime += chrono::duration<double>(end - start).count();
    //         delete[] arr;
    //     }
    //     cout << "Average time: " << sumTime / k << " s" << endl;
    // }

    //ROSNĄCO 
    cout << "\nPosortowana rosnąca tablica" << endl;
    for (int n : tab) {
        double sumTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; i++) {
            T* arr = generate(n);
            sort(arr, arr + n);

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, 0, n - 1);
            auto end = chrono::high_resolution_clock::now();

            sumTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << sumTime / k << " s" << endl;
    }

    //MALEJĄCO
    cout << "\nPosortowana malejąca tablica" << endl;
    for (int n : tab) {
        double sumTime = 0;
        cout << "n = " << n << endl;
        for (int i = 0; i < k; i++) {
            T* arr = generate(n);
            sort(arr, arr + n, greater<T>());

            auto start = chrono::high_resolution_clock::now();
            sorter.sort(arr, 0, n - 1);
            auto end = chrono::high_resolution_clock::now();

            sumTime += chrono::duration<double>(end - start).count();
            delete[] arr;
        }
        cout << "Average time: " << sumTime / k << " s" << endl;
    }

    // //POSORTOWANA W 33%
    // cout << "\nPosortowana w 33%" << endl;
    // for (int n : tab) {
    //     double sumTime = 0;
    //     cout << "n = " << n << endl;
    //     for (int i = 0; i < k; i++) {
    //         T* arr = generate(n);
    //         int sortedPortion = static_cast<int>(n * 0.33);
    //         sort(arr, arr + sortedPortion);

    //         auto start = chrono::high_resolution_clock::now();
    //         sorter.sort(arr, 0, n - 1);
    //         auto end = chrono::high_resolution_clock::now();

    //         sumTime += chrono::duration<double>(end - start).count();
    //         delete[] arr;
    //     }
    //     cout << "Average time: " << sumTime / k << " s" << endl;
    // }

    // //POSORTOWANA W 66%
    // cout << "\nPosortowana w 66%" << endl;
    // for (int n : tab) {
    //     double sumTime = 0;
    //     cout << "n = " << n << endl;
    //     for (int i = 0; i < k; i++) {
    //         T* arr = generate(n);
    //         int sortedPortion = static_cast<int>(n * 0.66);
    //         sort(arr, arr + sortedPortion);

    //         auto start = chrono::high_resolution_clock::now();
    //         sorter.sort(arr, 0, n - 1);
    //         auto end = chrono::high_resolution_clock::now();

    //         sumTime += chrono::duration<double>(end - start).count();
    //         delete[] arr;
    //     }
    //     cout << "Average time: " << sumTime / k << " s" << endl;
    // }
}


void demoSortUI() {
    const string filename = "dane.txt";
    QuickSort<int> sorter(MIDDLE);

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

    sorter.sort(arr, 0, count - 1);

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
        cout << "2. Testy QuickSort (int, float, pivot = R)\n";
        cout << "0. Wyjście\n";
        cout << "Wybierz opcję: ";
        cin >> option;

        switch (option) {
            case 1:
                demoSortUI();
                break;
            case 2:
                testQuickSort<int>("int", RANDOM);
                testQuickSort<float>("float", RANDOM);
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
