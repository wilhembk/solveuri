#pragma once

#include "Graph.h"

typedef struct _Rectangle Rectangle;

/**
 * This structure transforms a rectangle into a graph (see references)
 * That way the solver can attempt a solve on rectangle by direclty solving the graph.
 */
struct _Rectangle {
    int height;
    int width;
    int** table;
    Graph* g;
};

/**
 * Creates a rectangle.
 * @param name The name of the rectangle to create
 * @param width The width of the rectangle
 * @param height THe height of the rectangle
 * @return The corresponding rectangle structure with the related graph 
 */
Rectangle* createRectangle(char* name, int width, int height);

/**
 * Updates the rectangle according to the labeling of the related graph
 * @param r The rectangle to update
 * @return The pointer to the rectangle r if it succeed, NULL if it fails.
 */
Rectangle* updateRectangle(Rectangle* r);

/**
 * Prints out the rectangle
 * @param r The rectangle to print
 * @param update 1 if we update the rectangle accoring to the label of the related graph, 0 if not
 */
void printRectangle(Rectangle* r, int update);

/**
 * Retrieve the wanted value. Make sure to update the rectangle before with updateRectangle
 * @param r The rectangle we want to retrieve the value from
 * @param i The line coordinate, starting from 0
 * @param j The column coordinate, starting from 0
 * @return The value is r->tab[i][j], exits program if out of bounds.
 */
int getVal(Rectangle* r, int i, int j);

/**
 * Gets a new table containing the values of line i
 * @param r The rectangle we want to retrieve the values from
 * @param i The line coordinate, starting from 0.
 * @return A newly allocated list containing the values of the line i
 */
int* getLine(Rectangle* r, int i);

/**
 * Gets a new table containing the values of column j
 * @param r The rectangle we want to retrieve the values from
 * @param j The column coordinate, starting from 0.
 * @return A newly allocated list containing the values of the column j
 */
int* getColumn(Rectangle* r, int j);

