#pragma once

/**
 * A vertex has a name and a labeling
 * Use the createVertex function to properly create one
 */
typedef struct _vertex {
    char* name;
    int label;

} Vertex;

/**
 * An edge has a name, a labeling, and is defined by two vertices
 * Use the createEdge function to properly create one
 */
typedef struct _edge {
    char* name;
    int label;
    Vertex vertices[2];

} Edge;

/**
 * A graph has a name, a list of vertices, and a list of edges
 * The size of those list are stored in variables.
 *
 * The nbvMAX et nbeMAX are constant that corresponds to the max number
 * of vertices and edge a simple graph of order n can have
 *
 * Use the createGraph function to properly create one
 */
typedef struct _graph {
    char* name;
    int nbvMAX;
    int nbv;
    Vertex* vertices;
    int nbeMAX;
    int nbe;
    Edge* edges;

} Graph;

/**
 * Neighbours are adjacent edges of a vertex.
 */
typedef struct _issuedEdges {
    Vertex* v;
    Edge** issuedEdges;
    int size;

} IssuedEdges;

/*
float absolue(float x);

void permuteTab(int* tab, int i, int j);


void affichertab(int* i, int len);
int exposant10(int e);
double fact(int n);
double binom(int n, int k);
int indiceDeMagieOpti(int nbv, int nbe);
char str(int i);
char* stdName(char eouv, int i); */

/**
 * Creates a vertex structure and allocated memory for its name
 * @param name The name of the vertex to create
 * @param label The labeling of the vertex
 * @return The vertex structure
 */
Vertex createVertex(char* name, int label);

/**
 * Creates an edge structure and allocated memory for its name
 * @param name The name of the edge to create
 * @param label The labeling of the edge
 * @param v1 The first vertex it's connected from
 * @param v2 The second vertex it's connected from
 * @return The edge structure
 */
Edge createEdge(char* name, int label, Vertex v1, Vertex v2);

/**
 * Allocates memory for a graph.
 * It allocates the memory of the vertices and edges tables
 * @param name The name of the graph
 * @param nbvMAX The number of vertices the graph can have
 * @return The pointer to the allocated graph
 */
Graph* createGraph(char* name, int nbvMAX);

Vertex* getVertex(Graph* g, char* name);
Edge* getEdge(Graph* g, char* name);

/**
 * Frees the memory of the graph, including its name.
 * It also frees the name of the vertices and edges.
 * @param g The graph to free
 */
void freeGraph(Graph* g);

/**
 * Adds a vertex to the graph
 * @param g The graph on wich we add a vertex
 * @param v The vertex to add
 * @return 1 if operation was successful, 0 if not
 */
int addVertex(Graph* g, Vertex v);

/**
 * Adds an edge to the graph
 * @param g The graph on wich we add an edge
 * @param e The edge to add
 * @return 1 if operation was successful, 0 if not
 */
int addEdge(Graph* g, Edge e);

/**
 * Clones the graph
 * @param g The graph to clone
 * @return A newly allocated graph with the same structure as g
 */
Graph* cloneGraph(Graph* g);

/**
 * Removes an edge from the graph
 * @param g The graph we want to remove an edge from
 * @param e The edge to remove
 */
void deleteEdge(Graph* g, Edge e);

/**
 * Removes a vertex from the graph
 * @param g The graph we want to remove a vertex from
 * @param v The vertex to remove
 */
void deleteVertex(Graph* g, Vertex v);

/**
 * Prints the vertex data
 * @param v The vertex we want to know about
 */
void printVertex(Vertex v);

/**
 * Prints the edge data
 * @param e The edge we want to know about
 */
void printEdge(Edge e);

/**
 * Prints all the graph data, vertices and edges included
 * @param g The graph we want to know about
 */
void printGraph(Graph* g);

/**
 * Prints the neighbour tab
 * @param n the neighbours we want to know about
 */
void printIssuedEdges(IssuedEdges* n);

/**
 * @param g The graph we want to know about
 * @param v The vertex on wich we check the edge issued from
 * @return The table of all the edge's labels issued from v
 */
int* getIssuedEdgesLabels(Graph* g, Vertex* v);

/**
 * @param g The graph we want to know about
 * @return 1 if the graph is connected, 0 if not
 */
int isConnected(Graph* g);

/**
 * Correctly label the graph g, from 1 to n+m
 * @param g The graph to relabel
 */
void relabelGraph(Graph* g);

/**
 * Correctly relabels all edges of g, without touching the vertices
 * @param g The graph to relabel
 * @param verticesLabels The table of the vertices' labels (so the label we
 * skip)
 * @param size The size of the table
 */
void relabelEdgesGraph(Graph* g, int* verticesLabels, int size);

/**
 * Gets the vertices issued edges
 * @param g The graph we want to know about
 * @param v The vertex pointer on wich we want the issued edges
 * @return The pointer of the neighbour structure containing
 * the issued edges of v
 */
IssuedEdges* getIssuedEdges(Graph* g, Vertex* v);

/**
 * Frees the IssuedEdges structure. Does not delete the edges
 * @param n The pointer to the structure to free
 */
void freeIssuedEdges(IssuedEdges* n);

/**
 * Shows the degrees of all the vertices in the graph.
 * It is ordered the same way the vertices are ordered
 * @param g The graph we want to know about
 * @return The list of the degree of each vertex
 */
int* degreesTab(Graph* g);


/**
 * Return the list of the indexes of the vertices, sorted with the degenareted order algorithm.
 * Its similar to the degenerated order algorithm that is presented here : https://en.wikipedia.org/wiki/Degeneracy_(graph_theory)
 * The only difference is, when it has to choose between multiple vertices, it takes the one with the minimal degree in the original graph
 * @param G the graph
 * @return the table L in the algorithm
 */
int* degeneratedOrder(Graph* G);

/**
 * Counts the triangles of a graph
 * @param g the graph
 * @return the number of triangle in g
 */
int countTriangle(Graph* g);

/**
 * Creates the antiprismatic graph of order n
 * @param name The name of the graph
 * @param n The order of the graph we will generate
 * @return The pointer to the antiprismatic graph with labeling from 1 to 6n
 */
Graph* createAntiprism(char* name, int n);

/**
 * Applies the VMT solving algortihm for an antiprismatic graph of odd labeling.
 * See references.
 * @param name The name of the graph
 * @param n The order of the graph we will generate
 * @return The pointer to the antiprismatic graph of order n with a VMT labeling
 */
Graph* oddAntiprismVMT(char* name, int n);

/**
 * Checks if 2 graphs have the same labeling by isomorphism
 * @param g1 The first graph to compare
 * @param g2 The second graph to compare
 * @return 1 if the labeling is the same, 0 if not
 */
int sameLabeling(Graph* g1, Graph* g2);


/**
 * Create a complete graph
 * @param size The size of the wanted complete graph
 * @return g
 */
Graph* createCompleteGraph(int size);
