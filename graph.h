#ifndef GRAPH_H
#define GRAPH_H

// Structure to represent an edge with time-dependent weights
typedef struct Edge {
    int target;
    int* weights;
    struct Edge* next;
} Edge;

// Structure to represent a graph
typedef struct Graph {
    int num_vertices;
    int period;
    Edge** adjacency_list;
} Graph;

// Structure to store state for path finding
typedef struct PathState {
    int vertex;
    int step;
    long long distance;
    int* path;
    int path_length;
} PathState;

// Function declarations
Graph* create_graph(int vertices, int period);
void add_edge(Graph* graph, int source, int target, int* weights);
int* find_shortest_path(Graph* graph, int start, int end, int* path_length);
void free_graph(Graph* graph);

#endif