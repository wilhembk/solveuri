#pragma once
#include "Graph.h"

/**
 * A cell for a linked list of graphs
 * This should not be created as is.
 */
typedef struct _CellGraph CellGraph;
struct _CellGraph {

    Graph* g;
    CellGraph* nxt;
};

/**
 * A linked list of graphs
 * This should not be created as is.
 */
typedef struct _GraphList GraphList;
struct _GraphList {

    CellGraph* head;
};

/**
 * Parse all graphs from a correctly formated file.
 * The [nauty program](https://pallini.di.uniroma1.it/) can generate such files.
 * First generate a graph6 formated file using the geng binary,
 * then use the showg binary on the previously generated file, withe the option
 * -e.
 * @param filename The path of the formated file
 * @return A linked list of graphs. The GraphList structure just stores the
 * head.
 */
GraphList* parseAllGraphs(char* filename);

/**
 * Frees the cell provided and all the next ones
 * @param cg The CellGraph from wich we start freeing memory
 */
void freeFromCell(CellGraph* cg);

/**
 * Generate a [Graphviz](https://graphviz.org/) dot file to visualize the graph.
 * The layout is always "circo"
 *
 * @param filename The path to save the file under
 * (This function does not add the dot extension itself)
 * @param g The graph to parse in the dot file
 * @param mode 0 shows all labels, 1 shows the graph structure without its
 * labelling, 2 shows only the total weights on each vertices.
 */
void generateDotFile(char* filename, Graph* g, int mode);

/**
 * Generate [Graphviz](https://graphviz.org/) dot files to visualize all the
 * graphs. The layout is always "circo"
 *
 * @param filename The path to save the file under
 * (This function does not add the dot extension itself)
 * @param gl The graphs to parse in the dot file
 * @param mode 0 shows all labels, 1 shows the graph structure without its
 * labelling, 2 shows only the total weights on each vertices.
 */
void generateAllDotFiles(char* filename, GraphList* gl, int mode);

/**
 * Frees the GRaphList from memory.
 * All graphs insinde are also freed.
 *
 * @param l The list to free
 */
void freeList(GraphList* l);

/**
 * Implement the graph in a dotfile into the Graph structure
 * @param filename the name of the dotfile where the graph is saved
 * @return the graph of the dotfile in the graph structure
 */
Graph* getFromDotFile(char* filename);
