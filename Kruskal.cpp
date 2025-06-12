#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>

struct Edge {
    int start, end, weight;
    
    Edge() : start(0), end(0), weight(0) {}
    Edge(int s, int e, int w) : start(s), end(e), weight(w) {}
};

// Implementacja kopca dla sortowania krawędzi (heap sort)
class EdgeHeap {
private:
    Edge* heap;
    int size;
    
    void heapify(int i, int n) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        
        if(left < n && heap[left].weight > heap[largest].weight)
            largest = left;
        if(right < n && heap[right].weight > heap[largest].weight)
            largest = right;
            
        if(largest != i) {
            Edge temp = heap[i];
            heap[i] = heap[largest];
            heap[largest] = temp;
            heapify(largest, n);
        }
    }
    
public:
    EdgeHeap(Edge* edges, int count) : heap(edges), size(count) {}
    
    void heapSort() {
        // Budowanie kopca
        for(int i = size / 2 - 1; i >= 0; i--) {
            heapify(i, size);
        }
        
        // Sortowanie
        for(int i = size - 1; i > 0; i--) {
            Edge temp = heap[0];
            heap[0] = heap[i];
            heap[i] = temp;
            heapify(0, i);
        }
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
        // Graf nieskierowany dla MST (zgodnie z wymaganiami)
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
    
    // Union-Find z kompresja sciezki i union by rank (zgodnie z wymaganiami)
    int find(int parent[], int i) {
        if(parent[i] != i) {
            parent[i] = find(parent, parent[i]); // Kompresja sciezki
        }
        return parent[i];
    }
    
    void unionSets(int parent[], int rank[], int x, int y) {
        int rootX = find(parent, x);
        int rootY = find(parent, y);
        
        // Union by rank
        if(rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if(rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
    }
    
    // Algorytm Kruskala dla reprezentacji macierzowej z heap sort
    void kruskalMatrix() {
        std::cout << "\nAlgorytm Kruskala - reprezentacja macierzowa:" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        Edge* matrix_edges = new Edge[vertices * vertices];
        int edge_count = 0;
        
        // Budowanie listy krawedzi z macierzy sasiedztwa
        for(int i = 0; i < vertices; i++) {
            for(int j = i + 1; j < vertices; j++) {
                if(adjacency_matrix[i][j] != 0) {
                    matrix_edges[edge_count++] = Edge(i, j, adjacency_matrix[i][j]);
                }
            }
        }
        
        // Sortowanie krawedzi za pomoca heap sort (O(E log E))
        EdgeHeap edgeHeap(matrix_edges, edge_count);
        edgeHeap.heapSort();
        
        // Struktury Union-Find
        int* parent = new int[vertices];
        int* rank = new int[vertices];
        
        for(int i = 0; i < vertices; i++) {
            parent[i] = i;
            rank[i] = 0;
        }
        
        Edge* result = new Edge[vertices - 1];
        int result_count = 0;
        int total_weight = 0;
        
        // Glowna petla algorytmu Kruskala
        for(int i = 0; i < edge_count && result_count < vertices - 1; i++) {
            int x = find(parent, matrix_edges[i].start);
            int y = find(parent, matrix_edges[i].end);
            
            if(x != y) {
                result[result_count++] = matrix_edges[i];
                total_weight += matrix_edges[i].weight;
                unionSets(parent, rank, x, y);
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
        
        delete[] matrix_edges;
        delete[] parent;
        delete[] rank;
        delete[] result;
    }
    
    // Algorytm Kruskala dla reprezentacji listowej z heap sort
    void kruskalList() {
        std::cout << "\nAlgorytm Kruskala - reprezentacja listowa:" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        Edge* list_edges = new Edge[vertices * vertices];
        int edge_count = 0;
        
        // Budowanie listy krawedzi z list sasiedztwa
        for(int i = 0; i < vertices; i++) {
            for(int j = 0; j < list_sizes[i]; j++) {
                int neighbor = adjacency_list[i][j];
                if(i < neighbor) { // Unikanie duplikatow w grafie nieskierowanym
                    int weight = adjacency_matrix[i][neighbor];
                    list_edges[edge_count++] = Edge(i, neighbor, weight);
                }
            }
        }
        
        // Sortowanie krawedzi za pomoca heap sort (O(E log E))
        EdgeHeap edgeHeap(list_edges, edge_count);
        edgeHeap.heapSort();
        
        // Struktury Union-Find
        int* parent = new int[vertices];
        int* rank = new int[vertices];
        
        for(int i = 0; i < vertices; i++) {
            parent[i] = i;
            rank[i] = 0;
        }
        
        Edge* result = new Edge[vertices - 1];
        int result_count = 0;
        int total_weight = 0;
        
        // Glowna petla algorytmu Kruskala
        for(int i = 0; i < edge_count && result_count < vertices - 1; i++) {
            int x = find(parent, list_edges[i].start);
            int y = find(parent, list_edges[i].end);
            
            if(x != y) {
                result[result_count++] = list_edges[i];
                total_weight += list_edges[i].weight;
                unionSets(parent, rank, x, y);
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
        
        delete[] list_edges;
        delete[] parent;
        delete[] rank;
        delete[] result;
    }
    
    // Funkcje pomocnicze dla testow wydajnosci
    double executeKruskalMatrix() {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        Edge* matrix_edges = new Edge[vertices * vertices];
        int edge_count = 0;
        
        for(int i = 0; i < vertices; i++) {
            for(int j = i + 1; j < vertices; j++) {
                if(adjacency_matrix[i][j] != 0) {
                    matrix_edges[edge_count++] = Edge(i, j, adjacency_matrix[i][j]);
                }
            }
        }
        
        EdgeHeap edgeHeap(matrix_edges, edge_count);
        edgeHeap.heapSort();
        
        int* parent = new int[vertices];
        int* rank = new int[vertices];
        
        for(int i = 0; i < vertices; i++) {
            parent[i] = i;
            rank[i] = 0;
        }
        
        Edge* result = new Edge[vertices - 1];
        int result_count = 0;
        
        for(int i = 0; i < edge_count && result_count < vertices - 1; i++) {
            int x = find(parent, matrix_edges[i].start);
            int y = find(parent, matrix_edges[i].end);
            
            if(x != y) {
                result[result_count++] = matrix_edges[i];
                unionSets(parent, rank, x, y);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double seconds = duration.count() / 1000000000.0;
        
        delete[] matrix_edges;
        delete[] parent;
        delete[] rank;
        delete[] result;
        
        return seconds;
    }
    
    double executeKruskalList() {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        Edge* list_edges = new Edge[vertices * vertices];
        int edge_count = 0;
        
        for(int i = 0; i < vertices; i++) {
            for(int j = 0; j < list_sizes[i]; j++) {
                int neighbor = adjacency_list[i][j];
                if(i < neighbor) {
                    int weight = adjacency_matrix[i][neighbor];
                    list_edges[edge_count++] = Edge(i, neighbor, weight);
                }
            }
        }
        
        EdgeHeap edgeHeap(list_edges, edge_count);
        edgeHeap.heapSort();
        
        int* parent = new int[vertices];
        int* rank = new int[vertices];
        
        for(int i = 0; i < vertices; i++) {
            parent[i] = i;
            rank[i] = 0;
        }
        
        Edge* result = new Edge[vertices - 1];
        int result_count = 0;
        
        for(int i = 0; i < edge_count && result_count < vertices - 1; i++) {
            int x = find(parent, list_edges[i].start);
            int y = find(parent, list_edges[i].end);
            
            if(x != y) {
                result[result_count++] = list_edges[i];
                unionSets(parent, rank, x, y);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double seconds = duration.count() / 1000000000.0;
        
        delete[] list_edges;
        delete[] parent;
        delete[] rank;
        delete[] result;
        
        return seconds;
    }
    
    // Testy wydajnosci zgodnie z wymaganiami projektu
    void performPerformanceTests() {
        std::cout << "\n=== TESTY WYDAJNOSCI ALGORYTMU KRUSKALA ===" << std::endl;
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
                    
                    double time_matrix = executeKruskalMatrix();
                    total_time_matrix += time_matrix;
                    
                    double time_list = executeKruskalList();
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
    
    std::cout << "=== ALGORYTM KRUSKALA - MINIMALNE DRZEWO ROZPINAJACE ===" << std::endl;
    
    do {
        std::cout << "\n--- MENU ---" << std::endl;
        std::cout << "1. Wczytaj z pliku" << std::endl;
        std::cout << "2. Wygeneruj graf losowo" << std::endl;
        std::cout << "3. Wyswietl listowo i macierzowo" << std::endl;
        std::cout << "4. Algorytm Kruskala - macierzowo" << std::endl;
        std::cout << "5. Algorytm Kruskala - listowo" << std::endl;
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
                    graph->kruskalMatrix();
                } else {
                    std::cout << "Brak grafu! Wczytaj lub wygeneruj graf." << std::endl;
                }
                break;
            }
            case 5: {
                if(graph) {
                    graph->kruskalList();
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
