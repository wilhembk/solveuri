#include "Graph.h"

#include "Utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vertex createVertex(char* name, int label) {
    Vertex* v = (Vertex*)malloc(sizeof(Vertex));
    v->label = label;
    char* buff = malloc(BUFFER_SIZE * sizeof(char));
    sprintf(buff, "%s", name);
    v->name = buff;
    return *v;
}
Edge createEdge(char* name, int label, Vertex v1, Vertex v2) {
    Edge* e = (Edge*)malloc(sizeof(Edge));
    e->label = label;
    char* buff = malloc(BUFFER_SIZE * sizeof(char));
    sprintf(buff, "%s", name);
    e->name = buff;
    e->vertices[0] = v1;
    e->vertices[1] = v2;
    return *e;
}

Graph* createGraph(char* name, int nbvMAX) {
    int nbeMAX = (nbvMAX * (nbvMAX - 1)) / 2;
    Graph* g = (Graph*)malloc(sizeof(Graph));
    char* buff = malloc(BUFFER_SIZE * sizeof(char));
    sprintf(buff, "%s", name);
    g->name = buff;
    g->nbeMAX = nbeMAX;
    g->nbvMAX = nbvMAX;
    g->nbe = 0;
    g->nbv = 0;
    g->vertices = malloc(nbvMAX * sizeof(Vertex));
    g->edges = malloc(nbeMAX * sizeof(Edge));
    return g;
}

Vertex* getVertex(Graph* g, char* name) {
    for (int i = 0; i < g->nbv; i++) {
        if (strcmp((g->vertices + i)->name, name) == 0) {
            return g->vertices + i;
        }
    }
    return NULL;
}

Edge* getEdge(Graph* g, char* name) {
    for (int i = 0; i < g->nbe; i++) {
        if (strcmp((g->edges + i)->name, name) == 0) { return g->edges + i; }
    }
    return NULL;
}

void freeGraph(Graph* g) {
    for (int i = 0; i < g->nbe; i++) {
        free((g->edges + i)->name);
    }
    free(g->edges);
    for (int i = 0; i < g->nbv; i++) {
        free((g->vertices + i)->name);
    }
    free(g->vertices);

    free(g->name);
    free(g);
}

int addVertex(Graph* g, Vertex v) {
    if (g->nbv == g->nbvMAX) {
        printf(ANSI_COLOR_YELLOW
               "WARNING: Ignored the vertex %s "
               "when trying to add it to the graph %s "
               "because it has too many vertices" ANSI_RESET_ALL "\n",
               v.name, g->name);
        return 0;
    }
    if (g->nbv > g->nbvMAX) { g->nbv = 0; }
    g->vertices[g->nbv] = v;
    g->nbv++;
    return 1;
}

int addEdge(Graph* g, Edge e) {
    if (g->nbe == g->nbeMAX) {
        printf(ANSI_COLOR_YELLOW "WARNING: Ignored the edge %s "
                                 "when trying to add it to the graph %s "
                                 "because it has too many edges" ANSI_RESET_ALL
                                 "\n",
               e.name, g->name);
        return 0;
    }

    if (g->nbe > g->nbeMAX) { g->nbe = 0; }
    g->edges[g->nbe] = e;
    g->nbe++;
    return 1;
}
Graph* cloneGraph(Graph* g) {
    char buff[100];
    sprintf(buff, "%s_clone", g->name);

    Graph* clone = createGraph(buff, g->nbvMAX);

    for (int i = 0; i < g->nbv; i++) {
        addVertex(clone, g->vertices[i]);
    }
    for (int i = 0; i < g->nbe; i++) {
        addEdge(clone, g->edges[i]);
    }
    return clone;
}

void deleteEdge(Graph* g, Edge e) {
    int ok = 1;
    for (int i = 0; i < g->nbe; i++) {
        if (e.name == g->edges[i].name) { ok = 0; }
    }
    if (ok) { return; }
    Edge* new = malloc(sizeof(Edge) * (g->nbe - 1));
    int check = 0;
    for (int i = 0; i < g->nbe; i++) {
        if (e.name == g->edges[i].name) {
            check = 1;

        } else {
            new[i - check] = g->edges[i];
            sprintf(new[i - check].name, "e%d", i - check);
        }
    }
    g->nbe--;
    g->edges = new;
}
void deleteVertex(Graph* g, Vertex v) {
    int ok = 1;

    for (int i = 0; i < g->nbv; i++) {
        if (v.name == g->vertices[i].name) { ok = 0; }
    }
    if (ok) { return; }
    for (int i = 0; i < g->nbe; i++) {
        if (g->edges[i].vertices[0].name == v.name ||
            g->edges[i].vertices[1].name == v.name) {

            deleteEdge(g, g->edges[i]);
            i--;
        }
    }

    Vertex* new = malloc(sizeof(Vertex) * (g->nbv - 1));
    int check = 0;
    for (int i = 0; i < g->nbv; i++) {

        if (v.name == g->vertices[i].name) {
            check = 1;

        } else {

            new[i - check] = g->vertices[i];
        }
    }

    g->nbv--;

    g->vertices = new;
    return;
}

void printVertex(Vertex v) { printf("%s, label=%d\n", v.name, v.label); }

void printEdge(Edge e) {
    printf("%s, label=%d. Vertices : %s,%s\n", e.name, e.label,
           e.vertices[0].name, e.vertices[1].name);
}

int* logicalOrTab(int* t1, int* t2, int size) {
    for (int i = 0; i < size; i++) {
        t1[i] = t1[i] || t2[i];
    }
    return t1;
}

int* getIssuedEdgesLabels(Graph* g, Vertex* v) {
    IssuedEdges* voisins = getIssuedEdges(g, v);
    int* res = calloc(g->nbv, sizeof(int));
    Vertex temp;
    int indice = 0;
    for (int i = 0; i < voisins->size; i++) {
        if (strcmp(voisins->issuedEdges[i]->vertices[0].name, v->name) == 0) {
            temp = voisins->issuedEdges[i]->vertices[1];

        } else {
            temp = voisins->issuedEdges[i]->vertices[0];
        }
        for (int j = 0; j < g->nbv; j++) {
            if (strcmp(g->vertices[j].name, temp.name) == 0) { indice = j; }
        }
        res[indice] = 1;
    }
    return res;
}

int isConnected(Graph* g) {
    int* curr = calloc(g->nbv, sizeof(int));
    curr[0] = 1;
    int* next = calloc(g->nbv, sizeof(int));
    next[0] = 1;
    do {
        memcpy(curr, next, g->nbv * sizeof(int));
        for (int i = 0; i < g->nbv; i++) {
            if (curr[i] == 1) {
                int* voisins = getIssuedEdgesLabels(g, &(g->vertices[i]));
                logicalOrTab(next, voisins, g->nbv);
                free(voisins);
            }
        }
    } while (!tabEquals(curr, next, g->nbv, g->nbv));
    for (int i = 0; i < g->nbv; i++) {
        if (curr[i] == 0) { return 0; }
    }
    return 1;
}

void printGraph(Graph* g) {

    printf("--- Graph \"%s\" ---\n", g->name);
    printf("Vertices :\n");
    for (int i = 0; i < g->nbv; i++) {
        printVertex(g->vertices[i]);
    }
    printf("Edges :\n");
    for (int i = 0; i < g->nbe; i++) {

        printEdge(g->edges[i]);
    }
    printf("--------\n");
    return;
}

void printIssuedEdges(IssuedEdges* voisins) {
    printf("[%s : [", voisins->v->name);
    for (int i = 0; i < voisins->size; i++) {
        printf("%s,", voisins->issuedEdges[i]->name);
    }
    printf("]]\n");
}

void relabelGraph(Graph* g) {
    int n = g->nbe + g->nbv;
    // Table of available labels
    int* seen = (int*)malloc(n * sizeof(int));
    int lenseen = n;
    for (int i = 0; i < n; i++) {
        seen[i] = i + 1;
    }
    int curr;
    // We add the vertex first
    for (int i = 0; i < g->nbv; i++) {
        curr = rand() % lenseen;

        g->vertices[i].label = seen[curr];
        permuteTab(seen, curr, lenseen - 1);
        lenseen--;
    }

    for (int i = 0; i < g->nbe; i++) {

        curr = rand() % lenseen;

        g->edges[i].label = seen[curr];

        permuteTab(seen, curr, lenseen - 1);
        lenseen--;
    }
}
void relabelEdgesGraph(Graph* g, int* labelsSommets, int taille) {

    int n = g->nbe + g->nbv;

    int* seen = (int*)malloc(n * sizeof(int));
    int lenseen = n;
    for (int i = 0; i < n; i++) {
        seen[i] = i + 1;
    }
    int curr;

    for (int i = 0; i < taille; i++) {

        curr = 0;
        while (seen[curr] != labelsSommets[i]) {
            curr++;
        }

        g->vertices[i].label = seen[curr];
        permuteTab(seen, curr, lenseen - 1);
        lenseen--;
    }

    for (int i = taille; i < g->nbv; i++) {
        curr = rand() % lenseen;

        g->vertices[i].label = seen[curr];
        permuteTab(seen, curr, lenseen - 1);
        lenseen--;
    }

    for (int i = 0; i < g->nbe; i++) {

        curr = rand() % lenseen;

        g->edges[i].label = seen[curr];

        permuteTab(seen, curr, lenseen - 1);
        lenseen--;
    }
}

IssuedEdges* getIssuedEdges(Graph* g, Vertex* v) {

    IssuedEdges* voisins = (IssuedEdges*)malloc(sizeof(IssuedEdges));
    voisins->v = v;
    voisins->issuedEdges = (Edge**)malloc(sizeof(Edge*) * (g->nbv - 1));
    int count = 0;
    for (int i = 0; i < (g->nbe); i++) {

        if ((g->edges[i].vertices[0].name == v->name) ||
            (g->edges[i].vertices[1].name == v->name)) {
            voisins->issuedEdges[count] = &(g->edges[i]);
            count++;
        }
    }

    voisins->size = count;

    return voisins;
}

void freeIssuedEdges(IssuedEdges* v) {
    free(v->issuedEdges);
    free(v);
}

int* degreesTab(Graph* g) {
    int* tab = (int*)malloc(g->nbv * sizeof(int));
    for (int i = 0; i < g->nbv; i++) {
        tab[i] = (getIssuedEdges(g, &(g->vertices[i])))->size;
    }
    return tab;
}

void smallestDegree(int* degres, int* tab, int taille) {
    // Put the vertex with the smallest degree in first
    int min = -1;
    int indice_min = 0;
    for (int i = 1; i < taille; i++) {
        if (tab[i] != -1 && min < degres[tab[i]]) {
            min = degres[tab[i]];
            indice_min = i;
        }
    }
    int temp = tab[0];
    tab[0] = tab[indice_min];
    tab[indice_min] = temp;
}

int** initializeD(Graph* g, int maxm, int* baseDegrees) {
    // Table D of degeneracy
    int** D = malloc(maxm * sizeof(int*));
    int* tab = degreesTab(g);
    int cpt;
    int check;
    int check2 = 0;
    int nbv = g->nbv;
    if (!nbv) { check2 = 1; }

    for (int i = 0; i < maxm; i++) {
        cpt = 0;
        D[i] = malloc((nbv + check2) * sizeof(int));

        for (int j = 0; j < (nbv + check2); j++) {
            // par defaut
            check = 0;
            D[i][j] = -1;
            for (int k = cpt; k < nbv; k++) {

                // We check if there is a vertex with this degree in the table
                if (tab[k] == i) {

                    D[i][j] = k;
                    j++;
                    cpt = k + 1;
                    check = 1;
                }
            }

            if (check) { j--; }
        }
        smallestDegree(baseDegrees, D[i], nbv);
    }

    return D;
}

int isD(int** D, int size) {
    // check if D contains only -1
    for (int i = 0; i < size; i++) {
        if (D[i][0] != -1) { return 1; }
    }
    return 0;
}

int countInBetween(int* L, int size, int min, int max) {
    // len([k for k in L && min<=k<=max])
    int res = 0;
    for (int i = 0; i < size; i++) {
        if (min <= L[i] && L[i] <= max) { res++; }
    }
    return res;
}

int clonedGraphIndex(int* L, int size, int D) {
    int res = 0;
    int curr = D;
    int prev = 0;
    int temp = D;
    int ecart = countInBetween(L, size, prev, curr);
    while (ecart) {
        curr += ecart;
        prev = temp + 1;

        temp = curr;
        res += ecart;
        ecart = countInBetween(L, size, prev, curr);
    }
    return D + res;
}

// k degenerted algorithm but the vertices are ordered by their degrees
int* degeneratedOrder(Graph* G) {

    // Since we are modifying the graph, we will clone it
    Graph* g = cloneGraph(G);
    int* baseTab = degreesTab(g);

    int* L = malloc(g->nbv * sizeof(int));
    for (int i = 0; i < g->nbv; i++) {
        L[i] = g->nbv;
    }
    int k = 0;
    int max = maxInTab(baseTab, g->nbv);
    int** D = initializeD(g, max, baseTab);
    int nbv = g->nbv;

    while (isD(D, max)) {

        for (int i = 0; i < max; i++) {
            // The D tables are ordered with the interesting vertex in the
            // beginning
            if (D[i][0] != -1) {

                // We retain the position of the vertex
                L[k] = clonedGraphIndex(L, nbv, D[i][0]);

                k++;

                deleteVertex(g, g->vertices[D[i][0]]);

                D = initializeD(g, max, baseTab);

                break;
            }
        }
    }

    freeGraph(g);
    return L;
}

int countTriangle(Graph* g) {
    Vertex v1;
    Vertex v2;
    int res = 0;
    int* tab1;
    int* tab2;

    for (int i = 0; i < g->nbe; i++) {
        v1 = g->edges[i].vertices[0];
        v2 = g->edges[i].vertices[1];

        tab1 = getIssuedEdgesLabels(g, &v1);
        tab2 = getIssuedEdgesLabels(g, &v2);
        for (int j = 0; j < g->nbv; j++) {
            res += tab1[j] * tab2[j];
        }
    }
    return (int)(res / 3);
}

Graph* createAntiprism(char* name, int n) {
    Graph* ap = createGraph(name, 2 * n);
    char buff[BUFFER_SIZE];

    // Inner vertices
    for (int i = 0; i < n; i++) {
        sprintf(buff, "x%d", i + 1);
        addVertex(ap, createVertex(buff, i + 1));
    }

    // Outer vertices
    for (int i = 0; i < n; i++) {
        sprintf(buff, "y%d", i + 1);
        addVertex(ap, createVertex(buff, n + i + 1));
    }

    // Inner cycle
    for (int i = 0; i < n - 1; i++) {
        sprintf(buff, "e%d", i);
        addEdge(ap, createEdge(buff, 2 * n + i + 1, ap->vertices[i],
                               ap->vertices[i + 1]));
    }
    sprintf(buff, "e%d", n - 1);
    addEdge(ap,
            createEdge(buff, 2 * n + n, ap->vertices[n - 1], ap->vertices[0]));

    // Outer Cycle
    for (int i = 0; i < n - 1; i++) {
        sprintf(buff, "e%d", n + i);
        addEdge(ap, createEdge(buff, 3 * n + i + 1, ap->vertices[n + i],
                               ap->vertices[n + i + 1]));
    }
    sprintf(buff, "e%d", 2 * n - 1);
    addEdge(ap, createEdge(buff, 3 * n + n, ap->vertices[2 * n - 1],
                           ap->vertices[n]));

    // Linking edges
    for (int i = 0; i < n; i++) {
        sprintf(buff, "e%d", 2 * n + i);
        addEdge(ap, createEdge(buff, 4 * n + i + 1, ap->vertices[i],
                               ap->vertices[n + i]));
    }
    for (int i = 0; i < n - 1; i++) {
        sprintf(buff, "e%d", 3 * n + i);
        addEdge(ap, createEdge(buff, 5 * n + i + 1, ap->vertices[i + 1],
                               ap->vertices[n + i]));
    }
    sprintf(buff, "e%d", 3 * n - 1);
    addEdge(ap, createEdge(buff, 5 * n + n, ap->vertices[0],
                           ap->vertices[n + n - 1]));

    return ap;
}

Graph* oddAntiprismVMT(char* name, int n) {
    Graph* ap = createAntiprism(name, n);
    for (int i = 0; i < 2 * n; i++) {
        ap->vertices[i].label = 2 * i + 1;
    }
    int cpt = 4 * n + 1;
    for (int i = 0; i < n; i++) {
        ap->edges[i].label = cpt;
        cpt += 2;
    }
    cpt = 2 * n + 2;
    for (int i = n; i < 2 * n; i += 2) {
        ap->edges[i].label = cpt;
        cpt += 4;
    }
    for (int i = n + 1; i < 2 * n; i += 2) {
        ap->edges[i].label = cpt;
        cpt += 4;
    }
    cpt = 6 * n;
    for (int i = 2 * n; i < 3 * n; i++) {
        ap->edges[i].label = cpt;
        cpt -= 4;
    }
    for (int i = 3 * n; i < 4 * n; i++) {
        ap->edges[i].label = cpt;
        cpt -= 2;
    }

    return ap;
}

int sameLabeling(Graph* g1, Graph* g2) {
    int n1 = g1->nbv;
    int n2 = g2->nbv;
    if (n1 != n2) { return 0; }
    int* tab1 = malloc(n1 * sizeof(int));
    for (int i = 0; i < n1; i++) {
        tab1[i] = g1->vertices[i].label;
    }
    int* tab2 = malloc(n1 * sizeof(int));
    for (int i = 0; i < n1; i++) {
        tab2[i] = g2->vertices[i].label;
    }

    qsort(tab1, n1, sizeof(int), cmp);
    qsort(tab2, n1, sizeof(int), cmp);
    for (int i = 0; i < n1; i++) {

        if (tab1[i] != tab2[i]) {

            free(tab1);
            free(tab2);
            return 0;
        }
    }
    free(tab1);
    free(tab2);
    return 1;
}

Graph *createCompleteGraph(int size){
    Graph* g=createGraph("CompleteGraph",size);
        char buff[BUFFER_SIZE];
        
        for(int i=0;i<size;i++){
            sprintf(buff, "v%d", i);
            addVertex(g, createVertex(buff, i+1 ));
        }
        int label=0;
        for (int i = 0; i < size-1 ; i++) {
            for(int j=i+1;j<size;j++){
                sprintf(buff, "e%d", label);
                addEdge(g, createEdge(buff, label+size+1,  g->vertices[i], g->vertices[j]));
                label++;
            }
        }
    return g;
}

