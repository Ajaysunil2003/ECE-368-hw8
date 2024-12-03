#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <graph_file>\n", argv[0]);
        return 1;
    }

    // Open input file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    // Read first line containing vertices and period
    int vertices, period;
    if (fscanf(file, "%d %d", &vertices, &period) != 2) {
        fprintf(stderr, "Error: Invalid file format\n");
        fclose(file);
        return 1;
    }

    // Create graph
    Graph* graph = create_graph(vertices, period);

    // Read edges
    int source, target;
    int* weights = (int*)malloc(period * sizeof(int));
    
    while (fscanf(file, "%d %d", &source, &target) == 2) {
        for (int i = 0; i < period; i++) {
            if (fscanf(file, "%d", &weights[i]) != 1) {
                fprintf(stderr, "Error: Invalid file format\n");
                free(weights);
                free_graph(graph);
                fclose(file);
                return 1;
            }
        }
        add_edge(graph, source, target, weights);
    }
    
    free(weights);
    fclose(file);

    // Process queries from stdin
    int start, end;
    while (scanf("%d %d", &start, &end) == 2) {
        int path_length;
        int* path = find_shortest_path(graph, start, end, &path_length);
        
        if (path) {
            for (int i = 0; i < path_length; i++) {
                printf("%d%c", path[i], (i < path_length - 1) ? ' ' : '\n');
            }
            free(path);
        } else {
            printf("No path found\n");
        }
    }

    free_graph(graph);
    return 0;
}