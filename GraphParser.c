#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Graph.h"
#include "GraphParser.h"
#include "Utils.h"

#define BUFF_SIZE 150

GraphList* parseAllGraphs(char* filename) {

    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Could not open %s when trying to parse graphs\n",
                filename);
        return NULL;
    }

    GraphList* res = malloc(sizeof(GraphList));
    res->head = NULL;

    char buff[BUFF_SIZE];
    CellGraph* currCell = res->head;
    fgets(buff, BUFF_SIZE, f); // \n in normal condition

    while (!feof(f)) {

        int graphNum = 0;
        int graphOrder = 0;
        fscanf(f, "Graph %d, order %d.\n", &graphNum, &graphOrder);
        // Graph %d, order %d in normal condition
        Graph* g = malloc(sizeof(Graph));
        g->name = malloc(100 * sizeof(char));
        sprintf(buff, "%d-%d", graphNum, graphOrder);
        strcpy(g->name, buff);

        int nbv = 0;
        int nbe = 0;
        fscanf(f, "%d %d\n", &nbv, &nbe);
        g->nbv = nbv;
        g->nbvMAX = nbv;
        g->nbe = nbe;
        g->nbeMAX = (nbv * (nbv - 1)) / 2;

        int cpt = 1;
        g->vertices = (Vertex*)malloc(nbv * sizeof(Vertex));
        for (int i = 0; i < nbv; i++) {
            // Convert i value to char*
            char* str = malloc(20 * sizeof(char));
            sprintf(str, "v%d", i);
            g->vertices[i] = createVertex(str, cpt);
            cpt++;
        }

        g->edges = (Edge*)malloc(nbe * sizeof(Edge));

        int v1;
        int v2;
        for (int i = 0; i < nbe; i++) {

            fscanf(f, "%d %d  ", &v1, &v2);
            // Convert v1 v2 value to char*
            char* str = malloc(20 * sizeof(char));
            sprintf(str, "e%d", i);

            Vertex eff_v1 = g->vertices[v1];
            Vertex eff_v2 = g->vertices[v2];
            g->edges[i] = createEdge(str, cpt, eff_v1, eff_v2);
            cpt++;
        }

        if (!res->head) {
            res->head = (CellGraph*)malloc(sizeof(CellGraph));
            res->head->g = g;
            res->head->nxt = NULL;
            currCell = res->head;
        } else {

            currCell->nxt = (CellGraph*)malloc(sizeof(CellGraph));
            currCell->nxt->g = g;
            currCell->nxt->nxt = NULL;
            currCell = currCell->nxt;
        }
    }
    fclose(f);
    return res;
}

void freeFromCell(CellGraph* cg) {
    CellGraph* currCell = cg;
    while (currCell) {
        freeGraph(currCell->g);
        CellGraph* tmp = currCell->nxt;
        free(currCell);
        currCell = tmp;
    }
}

void generateDotFile(char* filename, Graph* g, int mode) {

    FILE* f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr,
                "Error while trying to write in %s to create a dot file for graph %s\n",
                filename, g->name);
    }

    fprintf(f,
            "graph \"%s\" {\n"
            "\tlayout=circo;\n",
            g->name);

    for (int i = 0; i < g->nbv; i++) {
        Vertex currVertex = g->vertices[i];
        if (mode == 2) {
            int weight = currVertex.label;
            IssuedEdges* v = getIssuedEdges(g, g->vertices + i);
            for (int i = 0; i < v->size; i++) {
                weight += v->issuedEdges[i]->label;
            }
            fprintf(f, "\t%s [label=%d, shape=circle];\n", currVertex.name,
                    weight);
        } else if (mode == 1)
            fprintf(f, "\t%s [label=\"%s\", shape=circle];\n", currVertex.name,
                    currVertex.name);
        else
            fprintf(f, "\t%s [label=%d, shape=circle];\n", currVertex.name,
                    currVertex.label);
    }

    fprintf(f, "\n");

    for (int i = 0; i < g->nbe; i++) {
        Edge currEdge = g->edges[i];
        fprintf(f, "\t%s -- %s", currEdge.vertices[0].name,
                currEdge.vertices[1].name);
        (mode == 1 || mode == 2) ? fprintf(f, ";\n")
                                 : fprintf(f, " [label=%d];\n", currEdge.label);
    }
    fprintf(f, "}\n");

    printf("Generated %s dotfile for graph %s\n", filename, g->name);
    fclose(f);
}

Graph* getFromDotFile(char* filename) {
    FILE* f = fopen(filename, "r");

    // Dummy read to determine the number of vertices
    char buff[BUFFER_SIZE];
    int cnt = 0;
    while (1) {
        fgets(buff, BUFFER_SIZE, f);
        if (strcmp(buff, "\n") == 0) { break; }
        cnt++;
    }

    // The two first line are dotfile related
    int nbv = cnt - 2;

    // Starting read
    fseek(f, 0, SEEK_SET);

    fgets(buff, BUFFER_SIZE, f);

    char graphName[BUFFER_SIZE];

    sscanf(buff, "graph \"%[^\"]\" {\n", graphName);

    Graph* res = createGraph(graphName, nbv);

    fgets(buff, BUFFER_SIZE, f);

    int label;
    for (int i = 0; i < nbv; i++) {
        fscanf(f, "\t%s [label=%d, shape=circle];\n", buff, &label);
        addVertex(res, createVertex(buff, label));
    }

    fscanf(f, "\n");

    char v1S[BUFFER_SIZE];
    char v2S[BUFFER_SIZE];
    int i = 0;
    while (1) {
        fgets(buff, BUFFER_SIZE, f);
        if (strcmp(buff, "}\n") == 0) break;
        sscanf(buff, "\t%s -- %s [label=%d];\n", v1S, v2S, &label);

        Vertex* v1 = getVertex(res, v1S);
        Vertex* v2 = getVertex(res, v2S);

        if (!v1 || !v2) {
            printf(ANSI_COLOR_RED "Error while parsing %s" ANSI_RESET_ALL "\n",
                   filename);
            exit(EXIT_FAILURE);
        }

        sprintf(buff, "e%d", i);
        addEdge(res, createEdge(buff, label, *v1, *v2));
        i++;
    }
    fclose(f);
    return res;
}

void generateAllDotFiles(char* filename, GraphList* g,
                         int labeliseVerticesByName) {
    CellGraph* curr = g->head;

    char name[150];
    while (curr) {
        sprintf(name, "%s-%s", curr->g->name, filename);
        generateDotFile(name, curr->g, labeliseVerticesByName);
        curr = curr->nxt;
    }
}
