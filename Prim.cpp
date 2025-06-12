#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <climits>

struct Edge {
    int start, end, weight;
    
    Edge() : start(0), end(0), weight(0) {}
    Edge(int s, int e, int w) : start(s), end(e), weight(w) {}
};

// Implementacja kopca binarnego (min-heap) zgodnie z wymaganiami projektu
class MinHeap {
private:
    struct HeapNode {
        int vertex;
        int key;
        HeapNode() : vertex(-1), key(INT_MAX) {}
        HeapNode(int v, int k) : vertex(v), key(k) {}
    };
    
    HeapNode* heap;
    int* position; // pozycja wierzcholka w kopcu
    int size;
    int capacity;
    
    void swapNodes(int i, int j) {
        position[heap[i].vertex] = j;
        position[heap[j].vertex] = i;
        
        HeapNode temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }
    
    void heapifyUp(int index) {
        while(index > 0) {
            int parent = (index - 1) / 2;
            if(heap[parent].key <= heap[index].key) break;
            swapNodes(parent, index);
            index = parent;
        }
    }
    
    void heapifyDown(int index) {
        while(true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int smallest = index;
            
            if(left < size && heap[left].key < heap[smallest].key)
                smallest = left;
            if(right < size && heap[right].key < heap[smallest].key)
                smallest = right;
                
            if(smallest == index) break;
            
            swapNodes(index, smallest);
            index = smallest;
        }
    }
    
public:
    MinHeap(int cap) : capacity(cap), size(0) {
        heap = new HeapNode[capacity];
        position = new int[capacity];
        for(int i = 0; i < capacity; i++) {
            position[i] = -1;
        }
    }
    
    ~MinHeap() {
        delete[] heap;
        delete[] position;
    }
    
    bool isEmpty() { return size == 0; }
    
    void insert(int vertex, int key) {
        if(size >= capacity) return;
        
        heap[size] = HeapNode(vertex, key);
        position[vertex] = size;
        size++;
        heapifyUp(size - 1);
    }
    
    HeapNode extractMin() {
        if(size == 0) return HeapNode();
        
        HeapNode min = heap[0];
        position[min.vertex] = -1;
        
        heap[0] = heap[size - 1];
        if(size > 1) position[heap[0].vertex] = 0;
        size--;
        
        if(size > 0) heapifyDown(0);
        
        return min;
    }
    
    void decreaseKey(int vertex, int newKey) {
        int pos = position[vertex];
        if(pos == -1 || pos >= size) return;
        
        heap[pos].key = newKey;
        heapifyUp(pos);
    }
    
    bool isInHeap(int vertex) {
        return position[vertex] != -1 && position[vertex] < size;
    }
};

class Graph {
private:
    int vertices;
    int edges_count;
    int** adjacency_matrix;
    Edge* edge_list;
    int** adjacency_list;
    int* list_sizes;
    
    void reallocateMemory(int new_vertices) {
        // Zwolnienie starej pamieci
        if(adjacency_matrix) {
            for(int i = 0; i < vertices; i++) {
                delete[] adjacency_matrix[i];
                delete[] adjacency_list[i];
            }
            delete[] adjacency_matrix;
            delete[] adjacency_list;
            delete[] list_sizes;
            delete[] edge_list;
        }
        
        // Alokacja nowej pamieci
        vertices = new_vertices;
        adjacency_matrix = new int*[vertices];
        for(int i = 0; i < vertices; i++) {
            adjacency_matrix[i] = new int[vertices];
            for(int j = 0; j < vertices; j++) {
                adjacency_matrix[i][j] = 0;
            }
        }
        
        adjacency_list = new int*[vertices];
        list_sizes = new int[vertices];
        for(int i = 0; i < vertices; i++) {
            adjacency_list[i] = new int[vertices];
            list_sizes[i] = 0;
        }
        
        edge_list = new Edge[vertices * vertices];
        edges_count = 0;
    }
    
public:
    Graph(int v) : vertices(v), edges_count(0), adjacency_matrix(nullptr), 
                   edge_list(nullptr), adjacency_list(nullptr), list_sizes(nullptr) {
        reallocateMemory(v);
    }
    
    ~Graph() {
        if(adjacency_matrix) {
            for(int i = 0; i < vertices; i++) {
                delete[] adjacency_matrix[i];
                delete[] adjacency_list[i];
            }
            delete[] adjacency_matrix;
            delete[] adjacency_list;
            delete[] list_sizes;
            delete[] edge_list;
        }
    }
    
    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if(!file.is_open()) {
            std::cout << "Blad: Nie mozna otworzyc pliku " << filename << std::endl;
            return;
        }
        
        int file_edges, file_vertices;
        file >> file_edges >> file_vertices;
        
        reallocateMemory(file_vertices);
        
        for(int i = 0; i < file_edges; i++) {
            int start, end, weight;
            file >> start >> end >> weight;
            addEdge(start, end, weight);
            edge_list[i] = Edge(start, end, weight);
        }
        edges_count = file_edges;
        
        file.close();
        
        std::cout << "Graf wczytany z pliku. Reprezentacje:" << std::endl;
        displayMatrix();
        displayList();
    }
    
    void generateRandom(int v, int density) {
        reallocateMemory(v);
        
        // Generowanie drzewa rozpinajacego dla spojnosci (zgodnie z wymaganiami)
        for(int i = 1; i < vertices; i++) {
            int parent = rand() % i;
            int weight = rand() % 100 + 1;
            addEdge(parent, i, weight);
            edge_list[edges_count++] = Edge(parent, i, weight);
        }
        
        int max_edges = (vertices * (vertices - 1)) / 2;
        int target_edges = (max_edges * density) / 100;
        
        // Dodawanie pozostalych krawedzi
        while(edges_count < target_edges) {
            int start = rand() % vertices;
            int end = rand() % vertices;
            
            if(start != end && adjacency_matrix[start][end] == 0) {
                int weight = rand() % 100 + 1;
                addEdge(start, end, weight);
                edge_list[edges_count++] = Edge(start, end, weight);
            }
        }
        
        std::cout << "Graf wygenerowany losowo (" << vertices << " wierzcholkow, " 
                  << density << "% gestosci, " << edges_count << " krawedzi)" << std::endl;
        displayMatrix();
        displayList();
    }
    
    void generateRandomForTesting(int v, int density) {
        reallocateMemory(v);
        
        for(int i = 1; i < vertices; i++) {
            int parent = rand() % i;
            int weight = rand() % 100 + 1;
            addEdge(parent, i, weight);
            edge_list[edges_count++] = Edge(parent, i, weight);
        }
        
        int max_edges = (vertices * (vertices - 1)) / 2;
        int target_edges = (max_edges * density) / 100;
        
        while(edges_count < target_edges) {
            int start = rand() % vertices;
            int end = rand() % vertices;
            
            if(start != end && adjacency_matrix[start][end] == 0) {
                int weight = rand() % 100 + 1;
                addEdge(start, end, weight);
                edge_list[edges_count++] = Edge(start, end, weight);
            }
        }
    }
    
    void addEdge(int start, int end, int weight) {
        // Graf nieskierowany dla MST
        adjacency_matrix[start][end] = weight;
        adjacency_matrix[end][start] = weight;
        
        adjacency_list[start][list_sizes[start]++] = end;
        adjacency_list[end][list_sizes[end]++] = start;
    }
    
    void displayMatrix() {
        if(vertices > 20) {
            std::cout << "\nGraf zbyt duzy do wyswietlenia macierzy (>20 wierzcholkow)" << std::endl;
            return;
        }
        
        std::cout << "\nReprezentacja macierzowa:" << std::endl;
        std::cout << "   ";
        for(int i = 0; i < vertices; i++) {
            std::cout << i << "  ";
        }
        std::cout << std::endl;
        
        for(int i = 0; i < vertices; i++) {
            std::cout << i << ": ";
            for(int j = 0; j < vertices; j++) {
                std::cout << adjacency_matrix[i][j] << "  ";
            }
            std::cout << std::endl;
        }
    }
    
    void displayList() {
        if(vertices > 20) {
            std::cout << "\nGraf zbyt duzy do wyswietlenia listy (>20 wierzcholkow)" << std::endl;
            return;
        }
        
        std::cout << "\nReprezentacja listowa:" << std::endl;
        for(int i = 0; i < vertices; i++) {
            std::cout << i << ": ";
            for(int j = 0; j < list_sizes[i]; j++) {
                std::cout << adjacency_list[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
    
    // Algorytm Prima dla reprezentacji macierzowej z kopcem
    void primMatrix() {
        std::cout << "\nAlgorytm Prima - reprezentacja macierzowa:" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        int* parent = new int[vertices];
        int* key = new int[vertices];
        bool* inMST = new bool[vertices];
        
        MinHeap minHeap(vertices);
        
        // Inicjalizacja
        for(int i = 0; i < vertices; i++) {
            key[i] = INT_MAX;
            parent[i] = -1;
            inMST[i] = false;
        }
        
        // Rozpoczecie od wierzcholka 0
        key[0] = 0;
        minHeap.insert(0, 0);
        
        Edge* result = new Edge[vertices - 1];
        int result_count = 0;
        int total_weight = 0;
        
        // Glowna petla algorytmu Prima
        while(!minHeap.isEmpty()) {
            auto minNode = minHeap.extractMin();
            int u = minNode.vertex;
            inMST[u] = true;
            
            // Sprawdzenie wszystkich sasiadow u w macierzy
            for(int v = 0; v < vertices; v++) {
                if(adjacency_matrix[u][v] && !inMST[v] && adjacency_matrix[u][v] < key[v]) {
                    key[v] = adjacency_matrix[u][v];
                    parent[v] = u;
                    
                    if(minHeap.isInHeap(v)) {
                        minHeap.decreaseKey(v, key[v]);
                    } else {
                        minHeap.insert(v, key[v]);
                    }
                }
            }
        }
        
        // Budowanie wyniku
        for(int i = 1; i < vertices; i++) {
            if(parent[i] != -1) {
                result[result_count++] = Edge(parent[i], i, adjacency_matrix[i][parent[i]]);
                total_weight += adjacency_matrix[i][parent[i]];
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double seconds = duration.count() / 1000000000.0;
        
        // Wyswietlenie wynikow zgodnie z wymaganiami
        std::cout << "Krawedzie minimalnego drzewa rozpinajacego:" << std::endl;
        for(int i = 0; i < result_count; i++) {
            std::cout << result[i].start << " - " << result[i].end 
                      << " (waga: " << result[i].weight << ")" << std::endl;
        }
        std::cout << "Sumaryczna waga MST: " << total_weight << std::endl;
        std::cout << "Czas wykonania (reprezentacja macierzowa): " << std::fixed << std::setprecision(6) << seconds << " sekund" << std::endl;
        
        delete[] parent;
        delete[] key;
        delete[] inMST;
        delete[] result;
    }
    
    // Algorytm Prima dla reprezentacji listowej z kopcem
    void primList() {
        std::cout << "\nAlgorytm Prima - reprezentacja listowa:" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        int* parent = new int[vertices];
        int* key = new int[vertices];
        bool* inMST = new bool[vertices];
        
        MinHeap minHeap(vertices);
        
        // Inicjalizacja
        for(int i = 0; i < vertices; i++) {
            key[i] = INT_MAX;
            parent[i] = -1;
            inMST[i] = false;
        }
        
        // Rozpoczecie od wierzcholka 0
        key[0] = 0;
        minHeap.insert(0, 0);
        
        Edge* result = new Edge[vertices - 1];
        int result_count = 0;
        int total_weight = 0;
        
        // Glowna petla algorytmu Prima
        while(!minHeap.isEmpty()) {
            auto minNode = minHeap.extractMin();
            int u = minNode.vertex;
            inMST[u] = true;
            
            // Sprawdzenie sasiadow u w liscie sasiedztwa
            for(int i = 0; i < list_sizes[u]; i++) {
                int v = adjacency_list[u][i];
                int weight = adjacency_matrix[u][v];
                
                if(!inMST[v] && weight < key[v]) {
                    key[v] = weight;
                    parent[v] = u;
                    
                    if(minHeap.isInHeap(v)) {
                        minHeap.decreaseKey(v, key[v]);
                    } else {
                        minHeap.insert(v, key[v]);
                    }
                }
            }
        }
        
        // Budowanie wyniku
        for(int i = 1; i < vertices; i++) {
            if(parent[i] != -1) {
                result[result_count++] = Edge(parent[i], i, adjacency_matrix[i][parent[i]]);
                total_weight += adjacency_matrix[i][parent[i]];
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double seconds = duration.count() / 1000000000.0;
        
        // Wyswietlenie wynikow zgodnie z wymaganiami
        std::cout << "Krawedzie minimalnego drzewa rozpinajacego:" << std::endl;
        for(int i = 0; i < result_count; i++) {
            std::cout << result[i].start << " - " << result[i].end 
                      << " (waga: " << result[i].weight << ")" << std::endl;
        }
        std::cout << "Sumaryczna waga MST: " << total_weight << std::endl;
        std::cout << "Czas wykonania (reprezentacja listowa): " << std::fixed << std::setprecision(6) << seconds << " sekund" << std::endl;
        
        delete[] parent;
        delete[] key;
        delete[] inMST;
        delete[] result;
    }
    
    // Funkcje pomocnicze dla testow wydajnosci
    double executePrimMatrix() {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        int* parent = new int[vertices];
        int* key = new int[vertices];
        bool* inMST = new bool[vertices];
        
        MinHeap minHeap(vertices);
        
        for(int i = 0; i < vertices; i++) {
            key[i] = INT_MAX;
            parent[i] = -1;
            inMST[i] = false;
        }
        
        key[0] = 0;
        minHeap.insert(0, 0);
        
        while(!minHeap.isEmpty()) {
            auto minNode = minHeap.extractMin();
            int u = minNode.vertex;
            inMST[u] = true;
            
            for(int v = 0; v < vertices; v++) {
                if(adjacency_matrix[u][v] && !inMST[v] && adjacency_matrix[u][v] < key[v]) {
                    key[v] = adjacency_matrix[u][v];
                    parent[v] = u;
                    
                    if(minHeap.isInHeap(v)) {
                        minHeap.decreaseKey(v, key[v]);
                    } else {
                        minHeap.insert(v, key[v]);
                    }
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double seconds = duration.count() / 1000000000.0;
        
        delete[] parent;
        delete[] key;
        delete[] inMST;
        
        return seconds;
    }
    
    double executePrimList() {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        int* parent = new int[vertices];
        int* key = new int[vertices];
        bool* inMST = new bool[vertices];
        
        MinHeap minHeap(vertices);
        
        for(int i = 0; i < vertices; i++) {
            key[i] = INT_MAX;
            parent[i] = -1;
            inMST[i] = false;
        }
        
        key[0] = 0;
        minHeap.insert(0, 0);
        
        while(!minHeap.isEmpty()) {
            auto minNode = minHeap.extractMin();
            int u = minNode.vertex;
            inMST[u] = true;
            
            for(int i = 0; i < list_sizes[u]; i++) {
                int v = adjacency_list[u][i];
                int weight = adjacency_matrix[u][v];
                
                if(!inMST[v] && weight < key[v]) {
                    key[v] = weight;
                    parent[v] = u;
                    
                    if(minHeap.isInHeap(v)) {
                        minHeap.decreaseKey(v, key[v]);
                    } else {
                        minHeap.insert(v, key[v]);
                    }
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double seconds = duration.count() / 1000000000.0;
        
        delete[] parent;
        delete[] key;
        delete[] inMST;
        
        return seconds;
    }
    
    // Testy wydajnosci zgodnie z wymaganiami projektu
    void performPerformanceTests() {
        std::cout << "\n=== TESTY WYDAJNOSCI ALGORYTMU PRIMA ===" << std::endl;
        std::cout << "Zgodnie z wymaganiami projektu:" << std::endl;
        std::cout << "- 7 rozmiarow grafow" << std::endl;
        std::cout << "- 3 gestosci: 20%, 60%, 99%" << std::endl;
        std::cout << "- 50 instancji dla kazdego zestawu" << std::endl;
        std::cout << "- Wyniki usrednione\n" << std::endl;
        
        int test_sizes[] = {500,1000, 2000, 3000,4000,5000,6000};
        int densities[] = {20, 60, 99};
        int num_tests = 3;
        
        for(int size_idx = 0; size_idx < 7; size_idx++) {
            int v = test_sizes[size_idx];
            
            for(int dens_idx = 0; dens_idx < 3; dens_idx++) {
                int density = densities[dens_idx];
                
                double total_time_matrix = 0.0;
                double total_time_list = 0.0;
                
                std::cout << "Testowanie " << v << " wierzcholkow, gestosc " << density << "%... ";
                std::cout.flush();
                
                for(int test = 0; test < num_tests; test++) {
                    generateRandomForTesting(v, density);
                    
                    double time_matrix = executePrimMatrix();
                    total_time_matrix += time_matrix;
                    
                    double time_list = executePrimList();
                    total_time_list += time_list;
                }
                
                double avg_time_matrix = total_time_matrix / num_tests;
                double avg_time_list = total_time_list / num_tests;
                
                std::cout << "GOTOWE" << std::endl;
                
                std::cout << "ilosc wierzcholkow = " << v << " gestosc = " << density << " reprezentacja = macierzowa" << std::endl;
                std::cout << "czas w sekundach = " << std::fixed << std::setprecision(6) << avg_time_matrix << std::endl;
                std::cout << std::endl;
                
                std::cout << "ilosc wierzcholkow = " << v << " gestosc = " << density << " reprezentacja = listowa" << std::endl;
                std::cout << "czas w sekundach = " << std::fixed << std::setprecision(6) << avg_time_list << std::endl;
                std::cout << std::endl;
            }
        }
        
        std::cout << "=== KONIEC TESTOW WYDAJNOSCI ===" << std::endl;
    }
};

int main() {
    srand(time(nullptr));
    Graph* graph = nullptr;
    int choice;
    
    std::cout << "=== ALGORYTM PRIMA - MINIMALNE DRZEWO ROZPINAJACE ===" << std::endl;
    
    do {
        std::cout << "\n--- MENU ---" << std::endl;
        std::cout << "1. Wczytaj z pliku" << std::endl;
        std::cout << "2. Wygeneruj graf losowo" << std::endl;
        std::cout << "3. Wyswietl listowo i macierzowo" << std::endl;
        std::cout << "4. Algorytm Prima - macierzowo" << std::endl;
        std::cout << "5. Algorytm Prima - listowo" << std::endl;
        std::cout << "6. Testy wydajnosci (zgodnie z wymaganiami)" << std::endl;
        std::cout << "0. Wyjscie" << std::endl;
        std::cout << "Wybierz opcje: ";
        std::cin >> choice;
        
        switch(choice) {
            case 1: {
                std::string filename;
                std::cout << "Podaj nazwe pliku: ";
                std::cin >> filename;
                
                if(graph) delete graph;
                graph = new Graph(10);
                graph->loadFromFile(filename);
                break;
            }
            case 2: {
                int vertices, density;
                std::cout << "Podaj liczbe wierzcholkow: ";
                std::cin >> vertices;
                std::cout << "Podaj gestosc grafu (w %): ";
                std::cin >> density;
                
                if(graph) delete graph;
                graph = new Graph(vertices);
                graph->generateRandom(vertices, density);
                break;
            }
            case 3: {
                if(graph) {
                    graph->displayMatrix();
                    graph->displayList();
                } else {
                    std::cout << "Brak grafu! Wczytaj lub wygeneruj graf." << std::endl;
                }
                break;
            }
            case 4: {
                if(graph) {
                    graph->primMatrix();
                } else {
                    std::cout << "Brak grafu! Wczytaj lub wygeneruj graf." << std::endl;
                }
                break;
            }
            case 5: {
                if(graph) {
                    graph->primList();
                } else {
                    std::cout << "Brak grafu! Wczytaj lub wygeneruj graf." << std::endl;
                }
                break;
            }
            case 6: {
                std::cout << "Uwaga: Testy wydajnosci moga potrwac kilka minut..." << std::endl;
                std::cout << "Czy kontynuowac? (t/n): ";
                char confirm;
                std::cin >> confirm;
                if(confirm == 't' || confirm == 'T') {
                    if(graph) delete graph;
                    graph = new Graph(10);
                    graph->performPerformanceTests();
                }
                break;
            }
            case 0:
                std::cout << "Koniec programu." << std::endl;
                break;
            default:
                std::cout << "Nieprawidlowa opcja!" << std::endl;
        }
    } while(choice != 0);
    
    if(graph) delete graph;
    return 0;
}
