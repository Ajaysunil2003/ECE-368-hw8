#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "graph.h"

// Priority Queue implementation for Dijkstra's algorithm
typedef struct {
    PathState** array;
    int capacity;
    int size;
} PriorityQueue;

// Create a new priority queue
PriorityQueue* create_priority_queue(int capacity) {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pq->array = (PathState**)malloc(capacity * sizeof(PathState*));
    pq->capacity = capacity;
    pq->size = 0;
    return pq;
}

// Helper functions for priority queue
void swap_states(PathState** a, PathState** b) {
    PathState* temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(PriorityQueue* pq, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < pq->size && pq->array[left]->distance < pq->array[smallest]->distance)
        smallest = left;

    if (right < pq->size && pq->array[right]->distance < pq->array[smallest]->distance)
        smallest = right;

    if (smallest != idx) {
        swap_states(&pq->array[idx], &pq->array[smallest]);
        heapify(pq, smallest);
    }
}

// Insert into priority queue
void pq_insert(PriorityQueue* pq, PathState* state) {
    if (pq->size == pq->capacity) {
        pq->capacity *= 2;
        pq->array = (PathState**)realloc(pq->array, pq->capacity * sizeof(PathState*));
    }

    int i = pq->size;
    pq->array[i] = state;
    pq->size++;

    while (i > 0 && pq->array[(i - 1) / 2]->distance > pq->array[i]->distance) {
        swap_states(&pq->array[i], &pq->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Extract minimum from priority queue
PathState* pq_extract_min(PriorityQueue* pq) {
    if (pq->size == 0) return NULL;

    PathState* min = pq->array[0];
    pq->array[0] = pq->array[pq->size - 1];
    pq->size--;
    heapify(pq, 0);

    return min;
}

// Create a new graph
Graph* create_graph(int vertices, int period) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->num_vertices = vertices;
    graph->period = period;
    graph->adjacency_list = (Edge**)calloc(vertices, sizeof(Edge*));
    return graph;
}

// Add edge to graph
void add_edge(Graph* graph, int source, int target, int* weights) {
    Edge* edge = (Edge*)malloc(sizeof(Edge));
    edge->target = target;
    edge->weights = (int*)malloc(graph->period * sizeof(int));
    memcpy(edge->weights, weights, graph->period * sizeof(int));
    
    edge->next = graph->adjacency_list[source];
    graph->adjacency_list[source] = edge;
}

// Create new path state
PathState* create_path_state(int vertex, int step, long long distance, int* prev_path, int prev_length) {
    PathState* state = (PathState*)malloc(sizeof(PathState));
    state->vertex = vertex;
    state->step = step;
    state->distance = distance;
    state->path_length = prev_length + 1;
    state->path = (int*)malloc(state->path_length * sizeof(int));
    
    if (prev_path && prev_length > 0) {
        memcpy(state->path, prev_path, prev_length * sizeof(int));
    }
    state->path[state->path_length - 1] = vertex;
    
    return state;
}

// Find shortest path implementation
int* find_shortest_path(Graph* graph, int start, int end, int* path_length) {
    // Create distance table
    long long** distances = (long long**)malloc(graph->num_vertices * sizeof(long long*));
    for (int i = 0; i < graph->num_vertices; i++) {
        distances[i] = (long long*)malloc(graph->period * sizeof(long long));
        for (int j = 0; j < graph->period; j++) {
            distances[i][j] = LLONG_MAX;
        }
    }

    // Initialize priority queue
    PriorityQueue* pq = create_priority_queue(graph->num_vertices * graph->period);
    
    // Add starting vertex
    PathState* initial = create_path_state(start, 0, 0, NULL, 0);
    pq_insert(pq, initial);
    distances[start][0] = 0;

    PathState* result = NULL;

    while (pq->size > 0) {
        PathState* current = pq_extract_min(pq);
        
        if (current->vertex == end) {
            result = current;
            break;
        }

        int current_step = current->step % graph->period;
        
        // Process all neighbors
        for (Edge* edge = graph->adjacency_list[current->vertex]; edge; edge = edge->next) {
            int next_step = (current->step + 1) % graph->period;
            long long new_distance = current->distance + edge->weights[current_step];

            if (distances[edge->target][next_step] > new_distance) {
                distances[edge->target][next_step] = new_distance;
                PathState* next = create_path_state(edge->target, current->step + 1, 
                                                  new_distance, current->path, current->path_length);
                pq_insert(pq, next);
            }
        }

        if (current != result) {
            free(current->path);
            free(current);
        }
    }

    // Clean up
    for (int i = 0; i < graph->num_vertices; i++) {
        free(distances[i]);
    }
    free(distances);
    
    while (pq->size > 0) {
        PathState* state = pq_extract_min(pq);
        if (state != result) {
            free(state->path);
            free(state);
        }
    }
    free(pq->array);
    free(pq);

    if (result) {
        *path_length = result->path_length;
        int* final_path = result->path;
        free(result);
        return final_path;
    }
    
    *path_length = 0;
    return NULL;
}

// Free graph memory
void free_graph(Graph* graph) {
    for (int i = 0; i < graph->num_vertices; i++) {
        Edge* current = graph->adjacency_list[i];
        while (current) {
            Edge* next = current->next;
            free(current->weights);
            free(current);
            current = next;
        }
    }
    free(graph->adjacency_list);
    free(graph);
}