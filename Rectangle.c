#include "Graph.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Rectangle.h"

#define BUFFER_SIZE 256

Rectangle* createRectangle(char* name, int width, int height) {
    int** newTable = malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        newTable[i] = malloc(width * sizeof(int));
    }
    char buff[BUFFER_SIZE];
    sprintf(buff, "RECTANGLE_%s", name);
    Graph* g = createGraph(buff, height + width);
    for (int i = 0; i < height; i++) {

        sprintf(buff, "v0,%d", i);
        Vertex vi = createVertex(buff, 0);

        addVertex(g, vi);
    }

    for (int j = 0; j < width; j++) {
        sprintf(buff, "v1,%d", j);
        Vertex vj = createVertex(buff, 0);
        addVertex(g, vj);
    }

    int cpt = 1;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            sprintf(buff, "e%d", cpt - 1);
            Vertex vi = g->vertices[i];
            Vertex vj = g->vertices[height + j];

            addEdge(g, createEdge(buff, cpt, vi, vj));
            newTable[i][j] = cpt;
            cpt++;
        }
    }

    Rectangle* res = malloc(sizeof(Rectangle));
    res->table = newTable;
    res->height = height;
    res->width = width;
    res->g = g;
    return res;
}

Rectangle* updateRectangle(Rectangle* r) {
    /* Updates the table if the graph has been modified */
    for (int i = 0; i < r->g->nbe; i++) {

        int x = -1;
        int y = -1;

        Edge e = r->g->edges[i];

        int xIsWidth = 2;
        int yIsWidth = 2;

        sscanf(e.vertices[0].name, "v%d,%d", &xIsWidth, &x);
        sscanf(e.vertices[1].name, "v%d,%d", &yIsWidth, &y);

        if (abs(xIsWidth) > 1 || abs(yIsWidth) > 1 || x == -1 || y == -1) {
            fprintf(stderr,
                    "Something went wrong when updating the rectangle %s, was "
                    "it correctly generated ?\n",
                    r->g->name);
            exit(EXIT_FAILURE);
        }

        if (xIsWidth) {

            if (yIsWidth || x >= r->width || y >= r->height) {
                fprintf(stderr, "Provided rectangle %s, is NOT a rectangle\n",
                        r->g->name);
                exit(EXIT_FAILURE);
            }

            r->table[y][x] = e.label;
        }

        else {
            if (!yIsWidth || x >= r->height || y >= r->width) {
                fprintf(stderr, "Provided rectangle %s, is NOT a rectangle\n",
                        r->g->name);
                exit(EXIT_FAILURE);
            }
            r->table[x][y] = e.label;
        }
    }
    return r;
}

void printRectangle(Rectangle* r, int update) {
    if (update) updateRectangle(r);
    for (int i = 0; i < r->height; i++) {
        printf("[");
        for (int j = 0; j < r->width - 1; j++) {
            printf("%3d, ", r->table[i][j]);
        }
        printf("%3d]\n", r->table[i][r->width - 1]);
    }
}

int getVal(Rectangle* r, int i, int j) {
    if (i >= r->height || j >= r->width) {
        fprintf(stderr, "Index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    return r->table[i][j];
}

int* getLine(Rectangle* r, int i) {

    int* res = malloc(r->width * sizeof(int));
    memcpy(res, r->table[i], r->width * sizeof(int));
    return res;
}

int* getColumn(Rectangle* r, int j) {
    int* res = malloc(r->height * sizeof(int));
    for (int i = 0; i < r->height; i++) {
        res[i] = r->table[i][j];
    }
    return res;
}

