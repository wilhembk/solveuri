#include "Solver.h"
#include "Graph.h"
#include "Rectangle.h"
#include "Utils.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int timeout = 200000;

int checksumLabel(Graph* g) {
    int n = g->nbv;
    int m = g->nbe;
    int goal = (n + m) * (n + m + 1) / 2;
    int res = 0;
    for (int i = 0; i < n; i++) {
        res += g->vertices[i].label;
    }
    for (int i = 0; i < m; i++) {
        res += g->edges[i].label;
    }
    if (res != goal) {
        printGraph(g);
        return 0;
    }
    return 1;
}

Solver* createSolver(Graph* g) {
    Solver* s = (Solver*)malloc(sizeof(Graph));
    s->g = g;
    s->tab = (IssuedEdges**)malloc(sizeof(IssuedEdges*) * g->nbv);
    for (int i = 0; i < g->nbv; i++) {
        s->tab[i] = getIssuedEdges(g, &(g->vertices[i]));
    }

    return s;
}

void freeSolver(Solver* s, int freeG) {
    if (freeG) freeGraph(s->g);
    free(s->tab);
    free(s);
}

void printIssuedEdgesList(Solver* s) {
    // Affiche tout les esges associées a chaque vertex dans un tableau
    printf("[\n");
    for (int i = 0; i < s->g->nbv; i++) {

        printIssuedEdges(s->tab[i]);
    }
    printf(" ]\n");
}

int* labelOfVertices(Graph* g) {
    int n = g->nbvMAX;
    int* tab = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        tab[i] = g->vertices[i].label;
    }
    qsort(tab, g->nbvMAX, sizeof(int), cmp);
    return tab;
}

int getMagicConst(Graph* g) {
    Solver* s = createSolver(g);
    int res = s->tab[0]->v->label;
    for (int i = 0; i < s->tab[0]->size; i++) {
        res += s->tab[0]->issuedEdges[i]->label;
    }
    return res;
}

/**
 * Permutes the two chosen issued edges of the selected vertex.
 * @param s The solver structure to modify
 * @param vertexIndex the index of the vertex inside the structure
 * @param x The index of the first edge to permute, 0 to permute the selected vertex
 * @param y The index of the second edge to permute, 0 to permute the selected vertex
 */
void permuteIssuedEdges(Solver* s, int vertexIndex, int x, int y) {
    // We permute to same elements, nothing happens
    if (x == y) return;

    // Retrieving issued edges of the selected vertex
    IssuedEdges* issuedEdges = s->tab[vertexIndex];
    if (x == 0) {
        // Permuting the vertex label with one of its issued edge label
        Vertex* v = issuedEdges->v;
        Edge* e = issuedEdges->issuedEdges[y - 1];
        int temp = v->label;
        v->label = e->label;
        e->label = temp;

        return;
    }

    if (y == 0) {
        Vertex* v = issuedEdges->v;
        Edge* e = issuedEdges->issuedEdges[x - 1];
        int temp = v->label;
        v->label = e->label;
        e->label = temp;
        return;
    }

    // Permuting 2 edges labels
    Edge* e1 = issuedEdges->issuedEdges[y - 1];
    Edge* e2 = issuedEdges->issuedEdges[x - 1];
    int temp = e1->label;
    e1->label = e2->label;
    e2->label = temp;
    return;
}

/**
 * Get the heuristic of the current graph with knowledge of the expected graph
 * @param s The solver to study
 * @param k The magic constant
 * @param k_d The antimagic step
 * @return 0 if the graph is (k, k_d)-antimagic
 */
int magicFormula(Solver* s, int k, int k_d) {
    int res = 0;
    int nb = s->g->nbv;

    int weights[nb];

    int temp;
    for (int i = 0; i < nb; i++) {
        temp = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            temp += s->tab[i]->issuedEdges[j]->label;
        }
        weights[i] = temp;
    }

    if (k_d < 0) qsort(weights, nb, sizeof(int), cmp_inv);
    if (k_d > 0) qsort(weights, nb, sizeof(int), cmp);
    for (int i = 0; i < nb; i++) {
        weights[i] = abs(weights[i] - k - i * k_d);
        res += weights[i];
    }

    return res;
}

/**
 * Get the heuristic of the current graph
 * @param s The solver to study
 * @return 0 if the graph is magic
 */
float magicFormulaUnknownConst(Solver* s) {
    float res = 0;
    int nb = s->g->nbv;

    int weights[nb];

    int temp;
    for (int i = 0; i < nb; i++) {
        temp = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            temp += s->tab[i]->issuedEdges[j]->label;
        }
        weights[i] = temp;
        res += weights[i];
    }

    float avg = res / nb;
    res = 0;
    for (int i = 0; i < nb; i++) {
        res += (weights[i] - avg) * (weights[i] - avg);
    }
    return res;
}

/**
 * Get the heuristic of the current graph with a wanted antimagic step
 * @param s The solver to study
 * @return 0 if the graph is antimagic of step d
 */
float antimagicFormulaKnownStep(Solver* s, int d) {
    // Renvoie un nombre, plus le nombre est proche de 0, plus le graphe est
    // proche d'etre magique
    float res = 0;
    int nb = s->g->nbv;

    int weights[nb];

    int temp;
    for (int i = 0; i < nb; i++) {
        temp = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            temp += s->tab[i]->issuedEdges[j]->label;
        }
        weights[i] = temp;
    }

    if (d < 0) qsort(weights, nb, sizeof(int), cmp_inv);
    if (d > 0) qsort(weights, nb, sizeof(int), cmp);

    for (int i = 0; i < nb - 1; i++) {
        res += (weights[i + 1] - weights[i] - d) * (weights[i + 1] - weights[i] - d);
    }
    return res;
}

/**
 * Get the heuristic of the current graph with a wanted antimagic result
 * @param s The solver to study
 * @return 0 if the graph is antimagic
 */
float antimagicFormulaUnknownConst(Solver* s) {
    // Renvoie un nombre, plus le nombre est proche de 0, plus le graphe est
    // proche d'etre magique
    float res = 0;
    int nb = s->g->nbv;
    int moyenne;
    int weights[nb];

    int temp;
    for (int i = 0; i < nb; i++) {
        temp = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            temp += s->tab[i]->issuedEdges[j]->label;
        }
        weights[i] = temp;
    }

    qsort(weights, nb, sizeof(int), cmp);

    for (int i = 0; i < nb - 1; i++) {
        res += weights[i + 1] - weights[i];
    }
    moyenne = res / (nb - 1);
    res = 0;
    for (int i = 0; i < nb - 1; i++) {
        res += (weights[i + 1] - weights[i] - moyenne) * (weights[i + 1] - weights[i] - moyenne);
    }

    return res;
}
float antimagicFormulaKnownConst(Solver* s, int a) {
    // Renvoie un nombre, plus le nombre est proche de 0, plus le graphe est
    // proche d'etre magique
    float res = 0;
    int nb = s->g->nbv;
    int moyenne;
    int weights[nb];

    int temp;
    for (int i = 0; i < nb; i++) {
        temp = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            temp += s->tab[i]->issuedEdges[j]->label;
        }
        weights[i] = temp;
    }

    qsort(weights, nb, sizeof(int), cmp);

    for (int i = 0; i < nb - 1; i++) {
        res += weights[i + 1] - weights[i];
    }
    moyenne = res / (nb - 1);
    res = 0;
    for (int i = 0; i < nb; i++) {
        res += (weights[i] - (a + i * moyenne)) * (weights[i] - (a + i * moyenne));
    }
    return res;
}
float allDifferentFormula(Solver* s) {
    // Renvoie un nombre, plus le nombre est proche de 0, plus le graphe est
    // proche d'etre magique
    float res = 0;
    int nb = s->g->nbv;
    int weights[nb];

    int temp;
    for (int i = 0; i < nb; i++) {
        temp = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            temp += s->tab[i]->issuedEdges[j]->label;
        }
        weights[i] = temp;
    }

    qsort(weights, nb, sizeof(int), cmp);

    for (int i = 0; i < nb - 1; i++) {
        res += (weights[i + 1] == weights[i]);
    }
    return res;
}

/**
 * Get the (k, d_k)-antimagic constants
 * @param s The solver to study
 * @param k The pointer to where to store the value of k
 * @param d_k The pointer to where to store the value of d_k
 */
void getAntimagicConst(Solver* s, int* k, int* d_k) {

    int nb = s->g->nbv;

    int weights[nb];

    int temp;
    for (int i = 0; i < nb; i++) {
        temp = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            temp += s->tab[i]->issuedEdges[j]->label;
        }
        weights[i] = temp;
    }
    qsort(weights, nb, sizeof(int), cmp);
    int d = weights[1] - weights[0];
    for (int i = 1; i < nb - 1; i++) {
        if (weights[i + 1] - weights[i] != d) {
            weights[0] = 0;
            d = 0;
        }
    }
    *k = weights[0];
    *d_k = d;
}

/**
 * Checks if the graph is antimagic without the knowledge of the constant
 * @param s The solver to study
 * @param d The antimagic step
 * @return 1 if the graph is antimagic of step d, 0 if not
 */
int IsAntimagicStep(Solver* s, int d) {
    int nb = s->g->nbv;

    int weights[nb];

    int temp;
    for (int i = 0; i < nb; i++) {
        temp = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            temp += s->tab[i]->issuedEdges[j]->label;
        }
        weights[i] = temp;
    }
    qsort(weights, nb, sizeof(int), cmp);

    for (int i = 0; i < nb - 1; i++) {
        if (weights[i + 1] - weights[i] != d) {
            return 0;
        }
    }

    return weights[0];
}

/**
 * Checks if the graph is (a,d)-antimagic
 * @param s The solver to study
 * @param a The antimagic constant
 * @param d The antimagic step
 * @return 1 if the graph is (a,d)-antimagic, 0 if not
 */
int IsAntimagic(Solver* s, int a, int d) {

    int nb = s->g->nbv;

    int weights[nb];

    int temp;
    for (int i = 0; i < nb; i++) {
        temp = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            temp += s->tab[i]->issuedEdges[j]->label;
        }
        weights[i] = temp;
    }

    qsort(weights, nb, sizeof(int), cmp);
    if (weights[0] != a) {
        return 0;
    }
    for (int i = 0; i < nb - 1; i++) {
        if (weights[i + 1] - weights[i] != d) {
            return 0;
        }
    }

    return 1;
}

/**
 * Get the heuristic of the current rectangle
 * @param s The solver to study
 * @param u The magic constant of lines
 * @param v The magic constant of columns
 * @param d_u The antimagic step of lines
 * @param d_v The antimagic step of columns
 * @return 0 if the rectangle is (u, d_u),(v, d_v)-antimagic (see reference)
 */
int magicFormulaRectangle(Solver* s, int u, int v, int d_u, int d_v) {

    int res = 0;
    int nb = s->g->nbv;

    int weightsColumn[nb];
    int weightsLign[nb];

    int temp;
    int height = 0;
    int width = 0;

    for (int i = 0; i < nb; i++) {
        temp = 0;
        for (int j = 0; j < s->tab[i]->size; j++) {
            temp += s->tab[i]->issuedEdges[j]->label;
        }

        if (s->tab[i]->v->name[1] == '0') {
            weightsColumn[height] = temp;
            height++;
        } else if (s->tab[i]->v->name[1] == '1') {
            weightsLign[width] = temp;
            width++;
        }
    }

    if (d_u > 0) qsort(weightsColumn, height, sizeof(int), cmp);
    if (d_u < 0) qsort(weightsColumn, height, sizeof(int), cmp_inv);
    if (d_v > 0) qsort(weightsLign, width, sizeof(int), cmp);
    if (d_v < 0) qsort(weightsLign, width, sizeof(int), cmp_inv);

    for (int i = 0; i < height; i++) {
        res += abs(weightsColumn[i] - u - i * d_u);
    }

    for (int i = 0; i < width; i++) {
        res += abs(weightsLign[i] - v - i * d_v);
    }

    return res;
}

/**
 * Checks if the graph is magic knowing its constant
 * @param s The solver to study
 * @param k The magic constant
 * @return 1 if the graph is k-magic, 0 if not
 */
int isMagic(Solver* s, int k) {

    int res;
    for (int i = 0; i < s->g->nbv; i++) {
        res = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            res += s->tab[i]->issuedEdges[j]->label;
        }
        if (res != k) { return 0; }
    }
    return 1;
}

/**
 * Checks if the graph is magic
 * @param s The solver to study
 * @return 1 if the graph is magic, 0 if not
 */
int isMagicUnknwonConst(Solver* s) {
    // Renvoit 1 si le graphe est magique
    int res;
    int k;
    for (int i = 0; i < s->g->nbv; i++) {
        res = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            res += s->tab[i]->issuedEdges[j]->label;
        }
        if (i == 0) { k = res; }
        if (res != k) { return 0; }
    }
    return 1;
}

/**
 * Finds the best graph by checking permutation between vertices.
 * @param method 0 to solve an antimagic graph (k, d_k),
 * 1 to solve an antimagic (k, d_k), (l, d_l) rectangle,
 * 2 to find a VMT
 * 3 to find a VAT knowing a step
 *
 * @param s The solver to study
 * @param k Depends on the method, can be ignored
 * @param l Depends on the method, can be ignored
 * @param d_k Depends on the method, can be ignored
 * @param d_l Depends on the method, can be ignored
 * @param tabooPermutations List of label that will not be permuted, NULL if ignored
 * @param sizePT The size of the provided list, 0 if ignored
 * @return 1 if a new graph was discovered, 0 if not
 */
int bestGraphByVertex(int method, Solver* s, int k, int l, int d_k, int d_l,
                      int* tabooPermutations, int sizePT) {

    float magicVal;
    if (method == 0) { magicVal = magicFormula(s, k, d_k); }
    if (method == 1) { magicVal = magicFormulaRectangle(s, k, l, d_k, d_l); }
    if (method == 2) { magicVal = magicFormulaUnknownConst(s); }
    if (method == 3) { magicVal = antimagicFormulaKnownStep(s, d_k); }
    if (method == 4) { magicVal = antimagicFormulaUnknownConst(s); }
    if (method == 5) { magicVal = antimagicFormulaKnownConst(s, k); }
    if (method == 6) { magicVal = allDifferentFormula(s); }

    float currMagicVal;

    int indexI = 0;
    int indexJ = 0;

    int temp;

    for (int j = 1; j < s->g->nbv; j++) {
        for (int i = 0; i < j; i++) {
            // We check all issued edges not yet studied from the current label
            if (inTab(s->g->vertices[i].label, tabooPermutations, sizePT) ||
                inTab(s->g->vertices[j].label, tabooPermutations, sizePT)) {
                continue;
            }
            temp = s->g->vertices[i].label;
            s->g->vertices[i].label = s->g->vertices[j].label;
            s->g->vertices[j].label = temp;

            // Permuting current couple

            if (method == 0) { currMagicVal = magicFormula(s, k, d_k); }
            if (method == 1) {
                currMagicVal = magicFormulaRectangle(s, k, l, d_k, d_l);
            }
            if (method == 2) { currMagicVal = magicFormulaUnknownConst(s); }
            if (method == 3) { currMagicVal = antimagicFormulaKnownStep(s, d_k); }
            if (method == 4) { currMagicVal = antimagicFormulaUnknownConst(s); }
            if (method == 5) { currMagicVal = antimagicFormulaKnownConst(s, k); }
            if (method == 6) { currMagicVal = allDifferentFormula(s); }
            if (currMagicVal < magicVal) {
                // This is a better graph, we save the permutation
                indexI = i;
                indexJ = j;
                magicVal = currMagicVal;
            }

            // We go back to the initial graph
            temp = s->g->vertices[i].label;
            s->g->vertices[i].label = s->g->vertices[j].label;
            s->g->vertices[j].label = temp;
        }
    }

    if (indexI == 0 && indexJ == 0) {
        // The best graph is already the current one
        return 0;
    }

    // We permute to get the best graph
    temp = s->g->vertices[indexI].label;
    s->g->vertices[indexI].label = s->g->vertices[indexJ].label;
    s->g->vertices[indexJ].label = temp;

    return 1;
}

/**
 * Finds the best graph by checking permutation between a vertex and its issued edges.
 * @param method 0 to solve an antimagic graph (k, d_k),
 * 1 to solve an antimagic (k, d_k), (l, d_l) rectangle,
 * 2 to find a VMT
 * 3 to find a VAT knowing a step
 *
 * @param s The solver to study
 * @param k Depends on the method, can be ignored
 * @param l Depends on the method, can be ignored
 * @param d_k Depends on the method, can be ignored
 * @param d_l Depends on the method, can be ignored
 * @param vertexIndex The vertex on which we will do the permutation from
 * @param tabooPermutations List of label that will not be permuted, NULL if ignored
 * @param sizePT The size of the provided list, 0 if ignored
 * @return 1 if a new graph was discovered, 0 if not
 */
int bestGraphByVertexAndIssuedEdges(int method, Solver* s,
                                    int k, int l, int d_k, int d_l, int vertexIndex, int* tabooPermutations, int sizePT) {

    float magicVal;
    if (method == 0) { magicVal = magicFormula(s, k, d_k); }
    if (method == 1) { magicVal = magicFormulaRectangle(s, k, l, d_k, d_l); }
    if (method == 2) { magicVal = magicFormulaUnknownConst(s); }
    if (method == 3) { magicVal = antimagicFormulaKnownStep(s, d_k); }
    if (method == 4) { magicVal = antimagicFormulaUnknownConst(s); }
    if (method == 5) { magicVal = antimagicFormulaKnownConst(s, k); }
    if (method == 6) { magicVal = allDifferentFormula(s); }
    float currMagicVal;
    int indexI = 0;
    int indexJ = 0;
    int check;
    for (int j = 1; j < s->tab[vertexIndex]->size + 1; j++) {
        for (int i = 0; i < j; i++) { // On parcours tout les couples possibles
                                      // de voisins par le vertex correspondant

            check = 0;

            if (i == 0) {
                check += inTab(s->tab[vertexIndex]->v->label,
                               tabooPermutations, sizePT);
            } else {
                check += inTab(s->tab[vertexIndex]->issuedEdges[i - 1]->label,
                               tabooPermutations, sizePT);
            }
            check += inTab(s->tab[vertexIndex]->issuedEdges[j - 1]->label,
                           tabooPermutations, sizePT);

            if (check > 0) { continue; }
            permuteIssuedEdges(s, vertexIndex, i, j); // On permute le couple actuel

            if (method == 0) { currMagicVal = magicFormula(s, k, d_k); }
            if (method == 1) {
                currMagicVal = magicFormulaRectangle(s, k, l, d_k, d_l);
            }
            if (method == 2) { currMagicVal = magicFormulaUnknownConst(s); }
            if (method == 3) { currMagicVal = antimagicFormulaKnownStep(s, d_k); }
            if (method == 4) { currMagicVal = antimagicFormulaUnknownConst(s); }
            if (method == 5) { currMagicVal = antimagicFormulaKnownConst(s, k); }
            if (method == 6) { currMagicVal = allDifferentFormula(s); }

            if (currMagicVal < magicVal) {
                // New better permutation found
                indexI = i;
                indexJ = j;
                magicVal = currMagicVal;
            }

            // Going back to the initial graph
            permuteIssuedEdges(s, vertexIndex, i, j);
        }
    }
    if ((indexI == 0) && (indexJ == 0)) {
        return 0;
    }
    permuteIssuedEdges(s, vertexIndex, indexI, indexJ);
    return 1;
}

/**
 * Finds the best graph by checking permutation between edges
 * @param method 0 to solve an antimagic graph (k, d_k),
 * 1 to solve an antimagic (k, d_k), (l, d_l) rectangle,
 * 2 to find a VMT
 * 3 to find a VAT knowing a step
 *
 * @param s The solver to study
 * @param k Depends on the method, can be ignored
 * @param l Depends on the method, can be ignored
 * @param d_k Depends on the method, can be ignored
 * @param d_l Depends on the method, can be ignored
 * @param tabooPermutations List of label that will not be permuted, NULL if ignored
 * @param sizePT The size of the provided list, 0 if ignored
 * @return 1 if a new graph was discovered, 0 if not
 */
int bestGraphByEdges(int method, Solver* s, int k, int l, int d_k, int d_l,
                     int* tabooPermutations, int sizePT) {

    float magicValue;
    if (method == 0) { magicValue = magicFormula(s, k, d_k); }
    if (method == 1) { magicValue = magicFormulaRectangle(s, k, l, d_k, d_l); }
    if (method == 2) { magicValue = magicFormulaUnknownConst(s); }
    if (method == 3) { magicValue = antimagicFormulaKnownStep(s, d_k); }
    if (method == 4) { magicValue = antimagicFormulaUnknownConst(s); }
    if (method == 5) { magicValue = antimagicFormulaKnownConst(s, k); }
    if (method == 6) { magicValue = allDifferentFormula(s); }
    float currMagicValue;
    int indexI = 0;
    int indexJ = 0;

    int temp;

    for (int j = 1; j < s->g->nbe; j++) {
        for (int i = 0; i < j; i++) {
            if (inTab(s->g->edges[i].label, tabooPermutations, sizePT) ||
                inTab(s->g->edges[j].label, tabooPermutations, sizePT)) {
                continue;
            }
            temp = s->g->edges[i].label;
            s->g->edges[i].label = s->g->edges[j].label;
            s->g->edges[j].label = temp;

            if (method == 0) { currMagicValue = magicFormula(s, k, d_k); }
            if (method == 1) {
                currMagicValue = magicFormulaRectangle(s, k, l, d_k, d_l);
            }
            if (method == 2) { currMagicValue = magicFormulaUnknownConst(s); }
            if (method == 3) { currMagicValue = antimagicFormulaKnownStep(s, d_k); }
            if (method == 4) { currMagicValue = antimagicFormulaUnknownConst(s); }
            if (method == 5) { currMagicValue = antimagicFormulaKnownConst(s, k); }
            if (method == 6) { currMagicValue = allDifferentFormula(s); }
            if (currMagicValue < magicValue) {
                indexI = i;
                indexJ = j;
                magicValue = currMagicValue;
            }

            temp = s->g->edges[i].label;
            s->g->edges[i].label = s->g->edges[j].label;
            s->g->edges[j].label = temp;
        }
    }
    if ((indexI == 0) && (indexJ == 0)) {

        return 0;
    }

    temp = s->g->edges[indexI].label;
    s->g->edges[indexI].label = s->g->edges[indexJ].label;
    s->g->edges[indexJ].label = temp;

    return 1;
}

/**
 * Finds the best graph by checking permutation between edges and vertices
 * @param method 0 to solve an antimagic graph (k, d_k),
 * 1 to solve an antimagic (k, d_k), (l, d_l) rectangle,
 * 2 to find a VMT
 * 3 to find a VAT knowing a step
 *
 * @param s The solver to study
 * @param k Depends on the method, can be ignored
 * @param l Depends on the method, can be ignored
 * @param d_k Depends on the method, can be ignored
 * @param d_l Depends on the method, can be ignored
 * @param tabooPermutations List of label that will not be permuted, NULL if ignored
 * @param sizePT The size of the provided list, 0 if ignored
 * @return 1 if a new graph was discovered, 0 if not
 */
int bestGraphByVerticesAndEdges(int method, Solver* s, int k, int l, int d_k,
                                int d_l, int* tabooPermutations, int sizePT) {

    float magicValue;
    if (method == 0) { magicValue = magicFormula(s, k, d_k); }
    if (method == 1) { magicValue = magicFormulaRectangle(s, k, l, d_k, d_l); }
    if (method == 2) { magicValue = magicFormulaUnknownConst(s); }
    if (method == 3) { magicValue = antimagicFormulaKnownStep(s, d_k); }
    if (method == 4) { magicValue = antimagicFormulaUnknownConst(s); }
    if (method == 5) { magicValue = antimagicFormulaKnownConst(s, k); }
    if (method == 6) { magicValue = allDifferentFormula(s); }
    float currMagicValue;
    int indexI = 0;
    int indexJ = 0;

    int temp;

    for (int j = 0; j < s->g->nbe; j++) {
        for (int i = 0; i < s->g->nbv; i++) {
            if (inTab(s->g->vertices[i].label, tabooPermutations, sizePT) ||
                inTab(s->g->edges[j].label, tabooPermutations, sizePT)) {
                continue;
            }
            temp = s->g->vertices[i].label;
            s->g->vertices[i].label = s->g->edges[j].label;
            s->g->edges[j].label = temp;

            if (method == 0) { currMagicValue = magicFormula(s, k, d_k); }
            if (method == 1) {
                currMagicValue = magicFormulaRectangle(s, k, l, d_k, d_l);
            }
            if (method == 2) { currMagicValue = magicFormulaUnknownConst(s); }
            if (method == 3) { currMagicValue = antimagicFormulaKnownStep(s, d_k); }
            if (method == 4) { currMagicValue = antimagicFormulaUnknownConst(s); }
            if (method == 5) { currMagicValue = antimagicFormulaKnownConst(s, k); }
            if (method == 6) { currMagicValue = allDifferentFormula(s); }
            if (currMagicValue < magicValue) {
                indexI = i;
                indexJ = j;
                magicValue = currMagicValue;
            }

            temp = s->g->vertices[i].label;
            s->g->vertices[i].label = s->g->edges[j].label;
            s->g->edges[j].label = temp;
        }
    }
    if ((indexI == 0) &&
        (indexJ == 0)) { // Si le graphe de base est le plus magique

        return 0;
    }

    temp = s->g->vertices[indexI].label;
    s->g->vertices[indexI].label = s->g->edges[indexJ].label;
    s->g->edges[indexJ].label = temp;

    return 1;
}

/**
 * Tries to solve the given graph, only by permuting the vertices and issued edges together
 * @param method 0 to solve an antimagic graph (k, d_k),
 * 1 to solve an antimagic (k, d_k), (l, d_l) rectangle,
 * 2 to find a VMT
 * 3 to find a VAT knowing a step
 *
 * @param s The solver to study
 * @param k Depends on the method, can be ignored
 * @param l Depends on the method, can be ignored
 * @param d_k Depends on the method, can be ignored
 * @param d_l Depends on the method, can be ignored
 * @param precision Magic value threshold to satisfy, 0 if we want to solve the graph
 * @param tabooPermutations List of label that will not be permuted, NULL if ignored
 * @param sizePT The size of the provided list, 0 if ignored
 */
// TODO test
void attemptSolveVertexAndIssuedEdges(int method, Solver* s, int k, int l, int d_k,
                                      int d_l, float precision, int* tabooPermutations,
                                      int sizePT) {

    // We found out that randomness was less reliable that trying for all vertices
    // In the future, we would like to understand why and optimize this variable
    int randomTimeout = 3;
    int randomIndex;
    int nbv = s->g->nbv;
    int iterCount = 0;
    int control;
    float magicValue;
    if (method == 0) { magicValue = magicFormula(s, k, d_k); }
    if (method == 1) { magicValue = magicFormulaRectangle(s, k, l, d_k, d_l); }
    if (method == 2) { magicValue = magicFormulaUnknownConst(s); }
    if (method == 3) { magicValue = antimagicFormulaKnownStep(s, d_k); }
    if (method == 4) { magicValue = antimagicFormulaUnknownConst(s); }
    if (method == 5) { magicValue = antimagicFormulaKnownConst(s, k); }
    if (method == 6) { magicValue = allDifferentFormula(s); }
    while (magicValue > precision) {
        if (iterCount > randomTimeout) {
            control = 0;
            for (int alerte = 0; alerte < nbv; alerte++) {
                control += bestGraphByVertexAndIssuedEdges(
                    method, s, k, l, d_k, d_l, alerte, tabooPermutations,
                    sizePT);
            }

            if (control == 0) {
                // Nothing was found, no better solution exists
                return;
            } else {
                iterCount = 0;
            }
        }

        // We select a random vertex and check if a better graph exists
        randomIndex = rand() % (nbv);

        control = bestGraphByVertexAndIssuedEdges(
            method, s, k, l, d_k, d_l, randomIndex, tabooPermutations,
            sizePT);

        if (control == 0) {
            // Randomness did not give a better graph
            iterCount++;
        }
        if (method == 0) {
            magicValue = magicFormula(s, k, d_k);
        } else if (method == 1) {
            magicValue = magicFormulaRectangle(s, k, l, d_k, d_l);
        } else if (method == 2) {
            magicValue = magicFormulaUnknownConst(s);
        } else if (method == 3) {
            magicValue = antimagicFormulaKnownStep(s, d_k);
        } else if (method == 4) {
            magicValue = antimagicFormulaUnknownConst(s);
        } else if (method == 5) {
            magicValue = antimagicFormulaKnownConst(s, k);
        } else if (method == 6) {
            magicValue = allDifferentFormula(s);
        }
    }
    return;
}

/**
 * Permute 2 labels regardless the heuristic
 * @param s The solver to study
 * @param tabooPermutations List of label that will not be permuted, NULL if ignored
 * @param sizePT The size of the provided list, 0 if ignored
 */
void newCloseGraph(Solver* s, int* tabooPermutations, int sizePT) {
    // Permute deux labels dans le graphe

    int nbv = s->g->nbv;
    int nbe = s->g->nbe;
    int label1;
    int label2;
    int numv1;
    do {
        numv1 = rand() % (nbe + nbv);
        if (numv1 < nbe) {
            label1 = s->g->edges[numv1].label;
        } else {
            label1 = s->g->vertices[numv1 - nbe].label;
        }
    } while (inTab(label1, tabooPermutations, sizePT));
    int numv2;
    int temp;
    do {
        numv2 = rand() % (nbe + nbv);
        if (numv2 < nbe) {
            label2 = s->g->edges[numv2].label;
        } else {
            label2 = s->g->vertices[numv2 - nbe].label;
        }
    } while (numv1 == numv2 || inTab(label2, tabooPermutations, sizePT));

    if (numv1 < nbe) {
        if (numv2 < nbe) {
            temp = s->g->edges[numv1].label;
            s->g->edges[numv1].label = s->g->edges[numv2].label;
            s->g->edges[numv2].label = temp;
            return;
        }
        numv2 -= nbe;
        temp = s->g->edges[numv1].label;
        s->g->edges[numv1].label = s->g->vertices[numv2].label;
        s->g->vertices[numv2].label = temp;
        return;
    }
    numv1 -= nbe;
    if (numv2 < nbe) {
        temp = s->g->vertices[numv1].label;
        s->g->vertices[numv1].label = s->g->edges[numv2].label;
        s->g->edges[numv2].label = temp;
        return;
    }
    numv2 -= nbe;
    temp = s->g->vertices[numv1].label;
    s->g->vertices[numv1].label = s->g->vertices[numv2].label;
    s->g->vertices[numv2].label = temp;
    return;
}

/**
 * Permute 2 consecutive labels regardless the heuristic
 * @param s The solver to study
 * @param enableVertexPermutations 1 to allow permutations with the label of vertices
 * @return 1 if operation was successful, 0 if not
 */
int newVeryCloseGraph(Solver* s, int enableVertexPermutations) {
    // Permute deux labels consecutifs du graphe
    int nbv = s->g->nbv;
    int nbe = s->g->nbe;
    int indice;
    int label;

    if (enableVertexPermutations) {
        int eouv = rand() % (nbe + nbv);

        if (eouv < nbv) {
            indice = rand() % nbv;
            label = s->g->vertices[indice].label;
            for (int i = 0; i < nbv; i++) {
                if (abs(s->g->vertices[i].label - label) == 1) {
                    s->g->vertices[indice].label = s->g->vertices[i].label;
                    s->g->vertices[i].label = label;
                    return 1;
                }
            }
            for (int i = 0; i < nbe; i++) {
                if (abs(s->g->edges[i].label - label) == 1) {
                    s->g->vertices[indice].label = s->g->edges[i].label;
                    s->g->edges[i].label = label;
                    return 1;
                }
            }
            return 0;
        }
        indice = rand() % nbe;
        label = s->g->edges[indice].label;
        for (int i = 0; i < nbv; i++) {
            if (abs(s->g->vertices[i].label - label) == 1) {
                s->g->edges[indice].label = s->g->vertices[i].label;
                s->g->vertices[i].label = label;
                return 1;
            }
        }
        for (int i = 0; i < nbe; i++) {
            if (abs(s->g->edges[i].label - label) == 1) {
                s->g->edges[indice].label = s->g->edges[i].label;
                s->g->edges[i].label = label;
                return 1;
            }
        }

    } else {
        for (int i = 1; i < nbe; i++) {
            label = s->g->edges[i].label;
            for (int j = 0; j < i; j++) {
                if (abs(s->g->edges[j].label - label) == 1) {
                    s->g->edges[i].label = s->g->edges[j].label;
                    s->g->edges[j].label = label;
                    return 1;
                }
            }
        }
    }
    return 0;
}

/**
 * Logs the result of the solve attempt
 * @param failed 1 to log a failed operation
 * @param method 0 to log a graph operation, 1 to log a rectangle operation
 * @param g The graph on wich we log operation
 * @param k The goal constant, in method 1 for the columns
 * @param l The line constant, in method 0 this is ignored
 * @param d_k The antimagic step, in method 1 for the columns
 * @param d_l The antimagic step for lines, in method 0 this is ignored
 * @param precision Target precision
 */
void printDetails(int failed, int method, Graph* g, int k, int l, int d_k,
                  int d_l, int precision) {
    failed ? printf(ANSI_COLOR_RED "Failed to solve %s ", g->name)
           : printf(ANSI_COLOR_GREEN "\nSuccessfully solved %s ", g->name);
    if (method == 0) {
        (d_k) ? printf("for a (%d, %d)-VAT with goal precision of %d", k, d_k,
                       precision)
              : printf("for a %d-VMT with goal precision of %d", k, precision);
    }
    if (method == 1) {
        (d_k) ? printf("for a (%d, %d)-antimagic columns ", k, d_k)
              : printf("for a %d-magic columns ", k);
        (d_l) ? printf("and (%d, %d)-antimagic ligns", l, d_l)
              : printf("and %d-magic ligns", l);
    }
    if (method == 2) {
        printf("for a %d-VMT with goal precision of %d", getMagicConst(g), precision);
    }
    printf(ANSI_RESET_ALL "\n");
}

/**
 * Attemps to solve the antimagic graph given a set threshold, at constant k and step d_k
 * The function modifies the labels of the graph provided in the params,
 * so that it matches with the wanted antimagic graph
 *
 * @param 	method 0 if solving any graphs, 1 if solving a rectangle
 * @param 	g the graph to solver
 * @param 	k the magic constant, if method 1 is used, for the columns
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
Solver* attemptSolveWithThreshold(int method, Graph* g, int k, int l, int d_k,
                                  int d_l, int precision, int* tabooPermutations,
                                  int sizePT) {

    Solver* s = createSolver(g);
    printf("Attempting solve for %s with goal precision: %d\n", g->name, precision);

    int i = 0;
    float magicValue;
    if (method == 0) {
        magicValue = magicFormula(s, k, d_k);
    } else if (method == 1) {
        magicValue = magicFormulaRectangle(s, k, l, d_k, d_l);
    } else if (method == 2) {
        magicValue = magicFormulaUnknownConst(s);
    } else if (method == 3) {
        magicValue = antimagicFormulaKnownStep(s, d_k);
    } else if (method == 4) {
        magicValue = antimagicFormulaUnknownConst(s);
    } else if (method == 5) {
        magicValue = antimagicFormulaKnownConst(s, k);
    } else if (method == 6) {
        magicValue = allDifferentFormula(s);
    }

    while (magicValue > precision) {

        showProgress(i, timeout);

        if (i > timeout) {
            printDetails(1, method, g, k, l, d_k, d_l, precision);
            return NULL;
        }

        if (!bestGraphByVertex(method, s, k, l, d_k, d_l,
                               tabooPermutations, sizePT)) {
            if (!bestGraphByEdges(method, s, k, l, d_k, d_l,
                                  tabooPermutations, sizePT)) {
                if (!bestGraphByVerticesAndEdges(method, s, k, l, d_k, d_l,
                                                 tabooPermutations, sizePT)) {

                    newCloseGraph(s, tabooPermutations, sizePT);
                }
            }
        }

        attemptSolveVertexAndIssuedEdges(method, s, k, l, d_k, d_l, precision,
                                         tabooPermutations, sizePT);

        i++;

        if (method == 0) {
            magicValue = magicFormula(s, k, d_k);
        } else if (method == 1) {
            magicValue = magicFormulaRectangle(s, k, l, d_k, d_l);
        } else if (method == 2) {
            magicValue = magicFormulaUnknownConst(s);
        } else if (method == 3) {
            magicValue = antimagicFormulaKnownStep(s, d_k);
        } else if (method == 4) {
            magicValue = antimagicFormulaUnknownConst(s);
        } else if (method == 5) {
            magicValue = antimagicFormulaKnownConst(s, k);
        } else if (method == 6) {
            magicValue = allDifferentFormula(s);
        }
    }
    printDetails(0, method, g, k, l, d_k, d_l, precision);

    return s;
}

/**
 * Attemps to solve the antimagic graph at constant k and step d_k
 * The function modifies the labels of the graph provided in the params,
 * so that it matches with the wanted antimagic graph
 *
 * @param 	method 0 if solving any graphs, 1 if solving a rectangle
 * @param 	g the graph to solver
 * @param 	k the magic constant, if method 1 is used, for the columns
 * @param 	l if method 1 is used, the magic constant for the lines
 * @param 	d_k the step of the magic constant k, 0 if we solve a magic
 * graph
 * @param 	d_l the step of the magic constant l, used if needed
 *
 * @return 	The solver pointer used to find the graph, or NULL, if the
 * solution was not found
 *
 * */
Solver* attemptSolve(int method, Graph* g, int k, int l, int d_k,
                     int d_l, int* permutationTaboues, int sizePT) {
    return attemptSolveWithThreshold(method, g, k, l, d_k, d_l, 0,
                                     permutationTaboues, sizePT);
}

/**
 * Checks if the rectangle is magic
 * @param s The solver containing the rectangle
 * @param u The column constant
 * @param v The line constant
 * @return 1 if the rectangle is magic, 0 if not
 */
int isMagic_Rect(Solver* s, int u, int v) {
    // Renvoit 1 si le graphe est magique
    int res;
    for (int i = 0; i < s->g->nbv; i++) {
        res = (s->tab[i]->v->label);
        for (int j = 0; j < s->tab[i]->size; j++) {
            res += s->tab[i]->issuedEdges[j]->label;
        }
        if (s->tab[i]->v->name[1] == '0' && res != u) { return 0; }
        if (s->tab[i]->v->name[1] == '1' && res != v) { return 0; }
    }
    return 1;
}

// -------- Public func ---------------

int getMinMagicConst(Graph* g) {
    int nbv = g->nbv;
    int nbe = g->nbe;
    double min = ceil((binom(nbv + nbe + 1, 2) + binom(nbe + 1, 2)) / nbv);

    return (int)min;
}

int getMaxMagicConst(Graph* g) {
    int nbv = g->nbv;
    int nbe = g->nbe;
    double max =
        floor(((2 * binom(nbv + nbe + 1, 2)) - binom(nbv + 1, 2)) / nbv);

    return (int)max;
}

Graph* solveVmtWithConst(Graph* g, int k) {
    int min = getMinMagicConst(g);
    int max = getMaxMagicConst(g);
    if (k < min || k > max) {
        fprintf(stderr,
                "Graph %s has no solution for a %d-VMT (Constant not in [%d, "
                "%d])\n",
                g->name, k, min, max);
        return NULL;
    }

    Solver* s = attemptSolveWithThreshold(0, g, k, 0, 0, 0, 0, NULL, 0);
    if (!s) return NULL;
    assert(isMagic(s, k));
    freeSolver(s, 0);
    return s->g;
}

Graph* solveVmtWithConstTabu(Graph* g, int k, int* ignoredLabels) {
    if (!ignoredLabels) {
        printf(ANSI_COLOR_YELLOW
               "WARNING: Using tabu VMT solve without a tabu list. Use "
               "solveVmtWithConst(...) instead" ANSI_RESET_ALL);
    }
    int min = getMinMagicConst(g);
    int max = getMaxMagicConst(g);
    if (k < min || k > max) {
        fprintf(stderr,
                "Graph %s has no solution for a %d-VMT (Constant not in [%d, "
                "%d])\n",
                g->name, k, min, max);
        return NULL;
    }

    Solver* s = attemptSolveWithThreshold(0, g, k, 0, 0, 0, 0, ignoredLabels, 0);
    if (!s) return NULL;
    assert(isMagic(s, k));
    freeSolver(s, 0);
    return s->g;
}

Graph* solveVmt(Graph* g) {
    Solver* s = attemptSolveWithThreshold(2, g, 0, 0, 0, 0, 0, NULL, 0);
    if (!s) return NULL;
    assert(isMagicUnknwonConst(s));
    freeSolver(s, 0);
    return s->g;
}

void getAntiMagicMinConstAndMaxStep(Graph* g, float* a, float* d) {
    int min_deg = -1;
    int max_deg = -1;
    for (int i = 0; i < g->nbv; i++) {
        IssuedEdges* v = getIssuedEdges(g, g->vertices + i);
        if (v->size < min_deg) min_deg = v->size;
        if (v->size > max_deg) max_deg = v->size;
        if (min_deg == -1) min_deg = v->size;
    }
    *a = ((min_deg + 1) * (min_deg + 2)) / 2.f;
    int n = g->nbv;
    int m = g->nbe;
    *d = ((2 * n + 2 * m - max_deg) * (max_deg + 1) -
          (min_deg + 1) * (min_deg + 2)) /
         2.f * (n - 1);
    return;
}

Graph* solveVatWithConstAndStep(Graph* g, int a, int step) {

    float min_a = 0;
    float max_d = 0;

    getAntiMagicMinConstAndMaxStep(g, &min_a, &max_d);
    if (a < min_a || step > max_d) {
        fprintf(stderr,
                "Graph %s has no solution for a (%d, %d)-VAT (%d should be "
                "higher than %.1f"
                " and %d should be lower than %.1f)\n",
                g->name, a, step, a, min_a, step, max_d);
        return NULL;
    }

    Solver* s = attemptSolveWithThreshold(0, g, a, 0, step, 0, 0, NULL, 0);
    if (!s) return NULL;
    freeSolver(s, 0);
    return s->g;
}

Graph* solveVatWithstep(Graph* g, int a) {
    Solver* s = attemptSolveWithThreshold(3, g, 0, 0, a, 0, 0, NULL, 0);
    if (!s) return NULL;
    freeSolver(s, 0);
    return s->g;
}
Graph* solveVat(Graph* g) {
    Solver* s = attemptSolveWithThreshold(4, g, 0, 0, 0, 0, 0, NULL, 0);
    if (!s) return NULL;
    freeSolver(s, 0);
    return s->g;
}

Rectangle* solveMagicRect(Rectangle* r, int* cst_width, int* cst_height) {

    int width = r->width;
    int height = r->height;
    int width_c = ((width * height) * (width * height + 1)) / (2 * width);
    int height_c = ((width * height) * (width * height + 1)) / (2 * height);
    if (cst_width) *cst_width = width_c;
    if (cst_height) *cst_height = height_c;

    Solver* s =
        attemptSolveWithThreshold(1, r->g, height_c, width_c, 0, 0, 0, NULL, 0);
    if (!s) return NULL;
    assert(isMagic_Rect(s, width_c, height_c));
    freeSolver(s, 0);
    updateRectangle(r);
    return r;
}

Rectangle* solveAntiMagicRect(Rectangle* r, int width_step, int height_step,
                              int* cst_width, int* cst_height) {
    int width = r->width;
    int height = r->height;
    int width_c = ((width * height) * (width * height + 1) -
                   width_step * (width * (width - 1))) /
                  (2 * width);
    int height_c = ((width * height) * (width * height + 1) -
                    height_step * (height * (height - 1))) /
                   (2 * height);
    if (cst_width) *cst_width = width_c;
    if (cst_height) *cst_height = height_c;

    Solver* s = attemptSolveWithThreshold(1, r->g, height_c, width_c, height_step,
                                          width_step, 0, NULL, 0);
    if (!s) return NULL;
    freeSolver(s, 0);
    updateRectangle(r);
    return r;
}
