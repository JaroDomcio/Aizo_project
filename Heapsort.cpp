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
class HeapSorter {
public:
    void sort(T* arr, int n) {
        buildHeap(arr, n);
        for (int i = n - 1; i > 0; i--) {
            swap(arr[0], arr[i]);
            heapify(arr, i, 0);
        }
    }

private:
    void buildHeap(T* arr, int n) {
        for (int i = n / 2 - 1; i >= 0; i--)
            heapify(arr, n, i);
    }

    void heapify(T* arr, int n, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && arr[left] > arr[largest])
            largest = left;
        if (right < n && arr[right] > arr[largest])
            largest = right;
        if (largest != i) {
            swap(arr[i], arr[largest]);
            heapify(arr, n, largest);
        }
    }
};


template<typename T>
void testHeapSort(const string& typeName) {
    HeapSorter<T> sorter;
    const int tab[] = {1000000, 2000000, 5000000, 8000000, 10000000, 15000000, 20000000};
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

    cout << "\n=== TESTY HEAP SORT DLA TYPU: " << typeName << " ===\n";

    //LOSOWA TABLICA
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

    //ROSNĄCO
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

    //MALEJĄCO 
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

    // POSORTOWANA W 66% 
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
    HeapSorter<int> sorter;
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
        cout << "2. Test sortowania dla typu int i float\n";
        cout << "0. Wyjście\n";
        cout << "Wybierz opcję: ";
        cin >> option;

        switch (option) {
            case 1:
                demoSortUI();
                break;
            case 2:
                testHeapSort<int>("int");
                testHeapSort<float>("float");
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
