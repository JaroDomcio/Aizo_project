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

class Graph {
private:
    int vertices;
    int edges_count;
    int start_vertex;
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
            adjacency_list[i] = new int[vertices * 2]; // [sasiad, waga, sasiad, waga, ...]
            list_sizes[i] = 0;
        }
        
        edge_list = new Edge[vertices * vertices];
        edges_count = 0;
    }
    
public:
    Graph(int v) : vertices(v), edges_count(0), start_vertex(0), adjacency_matrix(nullptr), 
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
        file >> file_edges >> file_vertices >> start_vertex;
        
        reallocateMemory(file_vertices);
        
        for(int i = 0; i < file_edges; i++) {
            int start, end, weight;
            file >> start >> end >> weight;
            addEdge(start, end, weight);
            edge_list[i] = Edge(start, end, weight);
        }
        edges_count = file_edges;
        
        file.close();
        
        std::cout << "Graf wczytany z pliku (wierzcholek startowy: " << start_vertex << "). Reprezentacje:" << std::endl;
        displayMatrix();
        displayList();
    }
    
    void generateRandom(int v, int density) {
        reallocateMemory(v);
        start_vertex = 0; // Domyslny wierzcholek startowy
        
        // Generowanie cyklu dla spojnosci (graf skierowany z ujemnymi wagami)
        for(int i = 0; i < vertices; i++) {
            int next = (i + 1) % vertices;
            int weight = (rand() % 200) - 100; // Wagi od -100 do 99
            addEdge(i, next, weight);
            edge_list[edges_count++] = Edge(i, next, weight);
        }
        
        int max_edges = vertices * (vertices - 1); // Graf skierowany
        int target_edges = (max_edges * density) / 100;
        
        while(edges_count < target_edges) {
            int start = rand() % vertices;
            int end = rand() % vertices;
            
            if(start != end && adjacency_matrix[start][end] == 0) {
                int weight = (rand() % 200) - 100; // Wagi od -100 do 99
                addEdge(start, end, weight);
                edge_list[edges_count++] = Edge(start, end, weight);
            }
        }
        
        std::cout << "Graf wygenerowany losowo (" << vertices << " wierzcholkow, " 
                  << density << "% gestosci, " << edges_count << " krawedzi, start: " << start_vertex << ")" << std::endl;
        displayMatrix();
        displayList();
    }
    
    void generateRandomForTesting(int v, int density) {
        reallocateMemory(v);
        start_vertex = 0;
        
        // Generowanie cyklu dla spojnosci
        for(int i = 0; i < vertices; i++) {
            int next = (i + 1) % vertices;
            int weight = (rand() % 200) - 100; // Wagi od -100 do 99
            addEdge(i, next, weight);
            edge_list[edges_count++] = Edge(i, next, weight);
        }
        
        int max_edges = vertices * (vertices - 1);
        int target_edges = (max_edges * density) / 100;
        
        while(edges_count < target_edges) {
            int start = rand() % vertices;
            int end = rand() % vertices;
            
            if(start != end && adjacency_matrix[start][end] == 0) {
                int weight = (rand() % 200) - 100; // Wagi od -100 do 99
                addEdge(start, end, weight);
                edge_list[edges_count++] = Edge(start, end, weight);
            }
        }
    }
    
    void addEdge(int start, int end, int weight) {
        // Graf skierowany - tylko jedna krawedz
        adjacency_matrix[start][end] = weight;
        
        // Lista sasiedztwa: [sasiad, waga, sasiad, waga, ...]
        adjacency_list[start][list_sizes[start] * 2] = end;
        adjacency_list[start][list_sizes[start] * 2 + 1] = weight;
        list_sizes[start]++;
    }
    
    void displayMatrix() {
        if(vertices > 20) {
            std::cout << "\nGraf zbyt duzy do wyswietlenia macierzy (>20 wierzcholkow)" << std::endl;
            return;
        }
        
        std::cout << "\nReprezentacja macierzowa (graf skierowany z ujemnymi wagami):" << std::endl;
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
        
        std::cout << "\nReprezentacja listowa (graf skierowany z ujemnymi wagami):" << std::endl;
        for(int i = 0; i < vertices; i++) {
            std::cout << i << ": ";
            for(int j = 0; j < list_sizes[i]; j++) {
                int neighbor = adjacency_list[i][j * 2];
                int weight = adjacency_list[i][j * 2 + 1];
                std::cout << "(" << neighbor << "," << weight << ") ";
            }
            std::cout << std::endl;
        }
    }
    
    void printPath(int parent[], int vertex, int* path, int& path_length) {
        if(parent[vertex] == -1) {
            path[path_length++] = vertex;
            return;
        }
        
        printPath(parent, parent[vertex], path, path_length);
        path[path_length++] = vertex;
    }
    
    void bellmanFordMatrix() {
        std::cout << "\nAlgorytm Bellmana-Forda - reprezentacja macierzowa:" << std::endl;
        std::cout << "Wierzcholek startowy: " << start_vertex << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        int* dist = new int[vertices];
        int* parent = new int[vertices];
        
        // Inicjalizacja
        for(int i = 0; i < vertices; i++) {
            dist[i] = INT_MAX;
            parent[i] = -1;
        }
        dist[start_vertex] = 0;
        
        // Relaksacja krawedzi (V-1) razy
        for(int i = 0; i < vertices - 1; i++) {
            for(int u = 0; u < vertices; u++) {
                for(int v = 0; v < vertices; v++) {
                    if(adjacency_matrix[u][v] != 0 && dist[u] != INT_MAX && 
                       dist[u] + adjacency_matrix[u][v] < dist[v]) {
                        dist[v] = dist[u] + adjacency_matrix[u][v];
                        parent[v] = u;
                    }
                }
            }
        }
        
        // Sprawdzenie cykli ujemnych
        bool has_negative_cycle = false;
        for(int u = 0; u < vertices; u++) {
            for(int v = 0; v < vertices; v++) {
                if(adjacency_matrix[u][v] != 0 && dist[u] != INT_MAX && 
                   dist[u] + adjacency_matrix[u][v] < dist[v]) {
                    has_negative_cycle = true;
                    break;
                }
            }
            if(has_negative_cycle) break;
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double seconds = duration.count() / 1000000000.0;
        
        if(has_negative_cycle) {
            std::cout << "WYKRYTO CYKL UJEMNY W GRAFIE!" << std::endl;
        } else {
            std::cout << "Najkrotsze sciezki od wierzcholka " << start_vertex << ":" << std::endl;
            for(int i = 0; i < vertices; i++) {
                if(i != start_vertex) {
                    std::cout << "Do wierzcholka " << i << ": ";
                    if(dist[i] == INT_MAX) {
                        std::cout << "NIEOSIAGALNY" << std::endl;
                    } else {
                        std::cout << "koszt = " << dist[i] << ", sciezka: ";
                        int* path = new int[vertices];
                        int path_length = 0;
                        printPath(parent, i, path, path_length);
                        
                        for(int j = 0; j < path_length; j++) {
                            std::cout << path[j];
                            if(j < path_length - 1) std::cout << " -> ";
                        }
                        std::cout << std::endl;
                        delete[] path;
                    }
                }
            }
        }
        
        std::cout << "Czas wykonania (reprezentacja macierzowa): " << std::fixed << std::setprecision(6) << seconds << " sekund" << std::endl;
        
        delete[] dist;
        delete[] parent;
    }
    
    void bellmanFordList() {
        std::cout << "\nAlgorytm Bellmana-Forda - reprezentacja listowa:" << std::endl;
        std::cout << "Wierzcholek startowy: " << start_vertex << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        int* dist = new int[vertices];
        int* parent = new int[vertices];
        
        // Inicjalizacja
        for(int i = 0; i < vertices; i++) {
            dist[i] = INT_MAX;
            parent[i] = -1;
        }
        dist[start_vertex] = 0;
        
        // Relaksacja krawedzi (V-1) razy
        for(int i = 0; i < vertices - 1; i++) {
            for(int u = 0; u < vertices; u++) {
                if(dist[u] != INT_MAX) {
                    for(int j = 0; j < list_sizes[u]; j++) {
                        int v = adjacency_list[u][j * 2];
                        int weight = adjacency_list[u][j * 2 + 1];
                        
                        if(dist[u] + weight < dist[v]) {
                            dist[v] = dist[u] + weight;
                            parent[v] = u;
                        }
                    }
                }
            }
        }
        
        // Sprawdzenie cykli ujemnych
        bool has_negative_cycle = false;
        for(int u = 0; u < vertices; u++) {
            if(dist[u] != INT_MAX) {
                for(int j = 0; j < list_sizes[u]; j++) {
                    int v = adjacency_list[u][j * 2];
                    int weight = adjacency_list[u][j * 2 + 1];
                    
                    if(dist[u] + weight < dist[v]) {
                        has_negative_cycle = true;
                        break;
                    }
                }
                if(has_negative_cycle) break;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double seconds = duration.count() / 1000000000.0;
        
        if(has_negative_cycle) {
            std::cout << "WYKRYTO CYKL UJEMNY W GRAFIE!" << std::endl;
        } else {
            std::cout << "Najkrotsze sciezki od wierzcholka " << start_vertex << ":" << std::endl;
            for(int i = 0; i < vertices; i++) {
                if(i != start_vertex) {
                    std::cout << "Do wierzcholka " << i << ": ";
                    if(dist[i] == INT_MAX) {
                        std::cout << "NIEOSIAGALNY" << std::endl;
                    } else {
                        std::cout << "koszt = " << dist[i] << ", sciezka: ";
                        int* path = new int[vertices];
                        int path_length = 0;
                        printPath(parent, i, path, path_length);
                        
                        for(int j = 0; j < path_length; j++) {
                            std::cout << path[j];
                            if(j < path_length - 1) std::cout << " -> ";
                        }
                        std::cout << std::endl;
                        delete[] path;
                    }
                }
            }
        }
        
        std::cout << "Czas wykonania (reprezentacja listowa): " << std::fixed << std::setprecision(6) << seconds << " sekund" << std::endl;
        
        delete[] dist;
        delete[] parent;
    }
    
    double executeBellmanFordMatrix() {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        int* dist = new int[vertices];
        int* parent = new int[vertices];
        
        for(int i = 0; i < vertices; i++) {
            dist[i] = INT_MAX;
            parent[i] = -1;
        }
        dist[start_vertex] = 0;
        
        // Relaksacja krawedzi (V-1) razy
        for(int i = 0; i < vertices - 1; i++) {
            for(int u = 0; u < vertices; u++) {
                for(int v = 0; v < vertices; v++) {
                    if(adjacency_matrix[u][v] != 0 && dist[u] != INT_MAX && 
                       dist[u] + adjacency_matrix[u][v] < dist[v]) {
                        dist[v] = dist[u] + adjacency_matrix[u][v];
                        parent[v] = u;
                    }
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double seconds = duration.count() / 1000000000.0;
        
        delete[] dist;
        delete[] parent;
        
        return seconds;
    }
    
    double executeBellmanFordList() {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        int* dist = new int[vertices];
        int* parent = new int[vertices];
        
        for(int i = 0; i < vertices; i++) {
            dist[i] = INT_MAX;
            parent[i] = -1;
        }
        dist[start_vertex] = 0;
        
        // Relaksacja krawedzi (V-1) razy
        for(int i = 0; i < vertices - 1; i++) {
            for(int u = 0; u < vertices; u++) {
                if(dist[u] != INT_MAX) {
                    for(int j = 0; j < list_sizes[u]; j++) {
                        int v = adjacency_list[u][j * 2];
                        int weight = adjacency_list[u][j * 2 + 1];
                        
                        if(dist[u] + weight < dist[v]) {
                            dist[v] = dist[u] + weight;
                            parent[v] = u;
                        }
                    }
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        double seconds = duration.count() / 1000000000.0;
        
        delete[] dist;
        delete[] parent;
        
        return seconds;
    }
    
    void performPerformanceTests() {
        std::cout << "\n=== TESTY WYDAJNOSCI ALGORYTMU BELLMANA-FORDA ===" << std::endl;
        std::cout << "Zgodnie z wymaganiami projektu:" << std::endl;
        std::cout << "- 7 rozmiarow grafow" << std::endl;
        std::cout << "- 3 gestosci: 20%, 60%, 99%" << std::endl;
        std::cout << "- 50 instancji dla kazdego zestawu" << std::endl;
        std::cout << "- Wyniki usrednione\n" << std::endl;
        
        int test_sizes[] = {100, 200, 300, 400, 500, 600, 700};
        int densities[] = {20, 60, 99};
        int num_tests = 50;
        
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
                    
                    double time_matrix = executeBellmanFordMatrix();
                    total_time_matrix += time_matrix;
                    
                    double time_list = executeBellmanFordList();
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
    
    std::cout << "=== ALGORYTM BELLMANA-FORDA - NAJKROTSZE SCIEZKI ===" << std::endl;
    
    do {
        std::cout << "\n--- MENU ---" << std::endl;
        std::cout << "1. Wczytaj z pliku" << std::endl;
        std::cout << "2. Wygeneruj graf losowo" << std::endl;
        std::cout << "3. Wyswietl listowo i macierzowo" << std::endl;
        std::cout << "4. Algorytm Bellmana-Forda - macierzowo" << std::endl;
        std::cout << "5. Algorytm Bellmana-Forda - listowo" << std::endl;
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
                    graph->bellmanFordMatrix();
                } else {
                    std::cout << "Brak grafu! Wczytaj lub wygeneruj graf." << std::endl;
                }
                break;
            }
            case 5: {
                if(graph) {
                    graph->bellmanFordList();
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
