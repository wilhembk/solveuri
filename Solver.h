#pragma once

#include "Graph.h"
#include "Rectangle.h"
#include <stdio.h>

typedef struct _solver {
    Graph* g;
    IssuedEdges** tab;

} Solver;

/**
 * Free the allocated memory of a solver
 *
 * @param 	s the solver
 * @param 	freeG an int: 1 if you want to free the graph in s and 0 otherwise
 */

void freeSolver(Solver* s, int freeG);

/**
 * Finds the antimagic graph at constant k and step d_k
 * The function modifies the labels of the graph provided in the params,
 * so that it matches with a antimagic graph
 *
 * @param 	method 0 if solving any graphs, 1 if solving a rectangle
 * @param 	g the graph to solver
 * @param 	k the magic constant, if methode 1 is used, for the columns
 * @param 	l if method 1 is used, the magic constant for the lines
 * @param 	d_k the step of the magic constant k, 0 if we solve a magic
 * graph
 * @param 	d_l the step of the magic constant l, used if needed
 * @param 	precision the value the heuristic needs to reach to stop
 * 			(0 if we want a stricly anti-magic graph)
 *
 * @return 	The solver pointer used to find the graph, or NULL, if the
 * solution was not found
 *
 * */
Solver* attemptSolve(int method, Graph* g, int k, int l, int d_k, int d_l, int* permutationsTaboues,int sizePT);

// Solveur* GenererGrapheMagiqueCompletP(int nbVertex, int k, float precision);
// Solveur* GenererGrapheMagiqueComplet(int nbVertex, int k);

/**
 * According to W.D Wallis Proof
 * @param g A graph g
 * @return The smallest possible magic constant on the graph g
 * */
int getMinMagicConst(Graph* g);
/**
 * According to W.D Wallis Proof
 * @param g A graph g
 * @return The largest possible magic constant on the graph g
 * */
int getMaxMagicConst(Graph* g);


/**
 * Calculates the magic constant of a VMT
 * @param g A graph
 * @return The magic coanstant of g if g is a VMT
 * */
int getMagicConst(Graph* g);
/**
 * Solve the k-VMT of said graph with the given starting labelisation.
 * Modifies the give labelisation of the given graph in the process,
 * except when k is out of bounds (according to W.D Wallis Proof).
 *
 * @param g The graph to solve
 * @param k The magic constant
 * @return The pointer of the graph g if solved, NULL if it was not
 * */
Graph* solveVmtWithConst(Graph* g, int k);

/**
 * Solve a VMT of said graph with the given starting labelisation.
 * Modifies the give labelisation of the given graph in the process.
 *
 * @param g The graph to solve g
 * @return The pointer of the graph g if solved, NULL if it was not
 * */
Graph* solveVmt(Graph* g);

/**
 * Get the minimum value of (a,d) for a (a,d)-VAT.
 * Those values are copied in the pointers provided.
 *
 * @param g The graph
 * @param a The pointer to store the value of
 * @param d The pointer to store the value of
 * */
void getAntiMagicMinConstAndMaxStep(Graph* g, float* a, float* d);



/**
 * Get the (k, d_k)-antimagic constants
 * @param s The solver to study
 * @param k The pointer to where to store the value of k
 * @param d_k The pointer to where to store the value of d_k
 */
void getAntimagicConst(Solver* s, int* k, int* d_k);
/**
 * Solve the (a,d)-VAT of said graph with the given starting labelisation.
 * Modifies the give labelisation of the given graph in the process,
 * except when a or d are out of bounds
 *
 * @param g The graph to solve
 * @param a The antimagic constant
 * @param d The antimagic step
 * @return The pointer of the graph g if solved, NULL if it was not
 * */

Graph* solveVatWithConstAndStep(Graph* g, int a, int d);

/**
 * Solve the magic rectangle given
 *
 * @param r The rectangle to solve
 * @param cst_height, if not NULL, stores the magic width constant
 * @param cst_height, if not NULL, stores the magic height constant
 * @return The pointer of the given rectangle if solved, NULL if not
 * */
Rectangle* solveMagicRect(Rectangle* r, int* cst_width, int* cst_height);

/**
 * Solve the anti-magic rectangle given
 *
 * @param r The rectangle to solve
 * @param width_step, the step of the column
 * @param height_step, the step of the lines
 * @param cst_height, if not NULL, stores the magic width constant
 * @param cst_height, if not NULL, stores the magic height constant
 * @return The pointer of the given rectangle if solved, NULL if not
 * */
Rectangle* solveAntiMagicRect(Rectangle* r, int width_step, int height_step,
                              int* cst_width, int* cst_height);

