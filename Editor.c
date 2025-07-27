#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Graph.h"
#include "GraphParser.h"
#include "Rectangle.h"
#include "Utils.h"

#include "Editor.h"

Graph* startEditor(int argc, char** argv, Graph* toEdit) {
    clear(argc, argv);
    printf("You are going to create or modify the graph you want to solve.\n"
           "Please select an option to start the editor\n\n");
    printCommandList();
    char inputBuff[BUFFER_SIZE];
    Graph* editedGraph = toEdit;
    while (!editedGraph) {
        fgets(inputBuff, BUFFER_SIZE, stdin);
        editedGraph = execCommand(inputBuff);
    }
    clear(argc, argv);
    printf("Working from the graph below\n");
    printGraph(editedGraph);
    printf("\n");
    printEditorExplanation();

    int awaitInput = 1;
    while (awaitInput) {
        fgets(inputBuff, BUFFER_SIZE, stdin);
        awaitInput = editingParser(editedGraph, inputBuff);
    }

    return editedGraph;
}

Graph* execCommand(char* cmdBuff) {
    char cmd[MAX_CMD_SIZE];

    int argIndex = parseWord(0, MAX_CMD_SIZE - 1, cmdBuff, cmd);

    if (strcmp(cmd, OPEN_CMD) == 0) { return openCmd(cmdBuff + argIndex); }

    if (strcmp(cmd, COMPLETE_CMD) == 0) {
        return completeCmd(cmdBuff + argIndex);
    }

    if (strcmp(cmd, COMPLETE_BIPARTITE_CMD) == 0) {
        return bipartiteCmd(cmdBuff + argIndex);
    }

    if (strcmp(cmd, ANTIPRISM_CMD) == 0) {
        return antiprismCmd(cmdBuff + argIndex);
    }

    if (strcmp(cmd, EMPTY_CMD) == 0) { return emptyCmd(cmdBuff + argIndex); }

    printf(ANSI_COLOR_RED "Command not found" ANSI_RESET_ALL "\n");
    printCommandList();
    return NULL;
}

int editingParser(Graph* g, char* inputBuffer) {

    char word[MAX_CMD_SIZE];
    int argIndex = parseWord(0, MAX_CMD_SIZE - 1, inputBuffer, word);

    if (strcmp(word, SOLVE_CMD) == 0) { return 0; }

    if (strcmp(word, RELABEL_CMD) == 0) {
        relabelLogic(g, inputBuffer + argIndex);
        return 1;
    }

    if (strcmp(word, LABEL_CMD) == 0) {
        labelLogic(g, inputBuffer + argIndex);
        return 1;
    }

    if (strcmp(word, LINK_CMD) == 0) {
        linkLogic(g, inputBuffer + argIndex);
        return 1;
    }

    if (strcmp(word, UNLINK_CMD) == 0) {
        unlinkLogic(g, inputBuffer + argIndex);
        return 1;
    }

    if (strcmp(word, SAVE_CMD) == 0) {
        saveLogic(g, inputBuffer + argIndex);
        return 1;
    }

    printf(ANSI_COLOR_RED "Command %s not found" ANSI_RESET_ALL "\n",
           inputBuffer);
    printEditorExplanation();

    return 1;
}

void printCommandList() {
    printf("List of available commands: \n"
           "- " OPEN_CMD " <filename> create a graph with the open file\n"
           "- " COMPLETE_CMD " <n> generate a complete graph of order n\n"
           "- " COMPLETE_BIPARTITE_CMD
           " <n> <m> generate a complete bipartite n,m graph "
           "parameters\n"
           "- " ANTIPRISM_CMD
           " <n> generate an antiprsimatic graph of order n\n"
           "- " EMPTY_CMD
           " <n> start creating a graph with n disconnected vertices\n\n");
}

void printEditorExplanation() {
    printf(
        "List of available commands: \n"
        "- " LINK_CMD
        " takes a list vertex separated by a number, and will link all "
        "vertices to the first one with the correctly labelled edge\n"
        "- " UNLINK_CMD " takes a list of vertex and remove all links from "
        "the first vertex to all the other ones\n"
        "- " LABEL_CMD " takes a list of element name along with a number to "
        "change its label\n"
        "- " RELABEL_CMD
        " to make a consecutive labeling on all graph (or edges if specified)\n"
        "- " SOLVE_CMD " to start the solve\n"
        "- " SAVE_CMD " to save the graph inside a dotfile\n\n");
}

void clear(int argc, char** argv) {
    // Clears terminal
    printf("\e[1;1H\e[2J");
    // Inputs the typed command
    for (int i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    printf("========[Solveuri EDITOR]=========\n");
}

void missingArg(char* cmdName, char* argNeeded) {
    printf(ANSI_COLOR_RED "%s command needs %s as argument" ANSI_RESET_ALL
                          "\n\n",
           cmdName, argNeeded);
}

int parseOrder(char* orderStr, int min) {
    int res = (int)strtol(orderStr, NULL, 10);
    if (res < min) {
        printf(ANSI_COLOR_RED "The order %s is not valid" ANSI_RESET_ALL "\n\n",
               orderStr);
        return 0;
    }
    return res;
}

Graph* openCmd(char* argBuff) {
    char filename[BUFFER_SIZE];
    int argSize = parseWord(0, BUFFER_SIZE - 1, argBuff, filename);
    if (argSize == 1) {
        missingArg(OPEN_CMD, "filename");
        return NULL;
    }
    char ext[BUFFER_SIZE] = "";
    char file[BUFFER_SIZE];
    sscanf(filename, "%[^.].%s", file, ext);

    if (strcmp(ext, "dot") == 0) { return getFromDotFile(filename); }

    GraphList* gl = parseAllGraphs(filename);

    if (!gl) return NULL;

    freeFromCell(gl->head->nxt);
    Graph* res = gl->head->g;
    free(gl);
    return res;
}

Graph* completeCmd(char* argBuff) {
    char orderStr[MAX_ORDER_NUMERALS + 1];
    int argSize = parseWord(0, MAX_ORDER_NUMERALS, argBuff, orderStr);
    if (argSize == 1) {
        missingArg(COMPLETE_CMD, "order");
        return NULL;
    }

    int order = parseOrder(orderStr, 1);
    if (!order) return NULL;

    return createCompleteGraph(order);
}

Graph* bipartiteCmd(char* argBuff) {
    char nS[MAX_ORDER_NUMERALS + 1];
    int argSize = parseWord(0, MAX_ORDER_NUMERALS, argBuff, nS);

    if (argSize == 1) {
        missingArg(COMPLETE_BIPARTITE_CMD, "n");
        return NULL;
    }
    int n = parseOrder(nS, 1);
    if (!n) return NULL;

    char mS[MAX_ORDER_NUMERALS + 1];
    argSize = parseWord(argSize, MAX_ORDER_NUMERALS, argBuff, mS);

    if (argSize == 1) {
        missingArg(COMPLETE_BIPARTITE_CMD, "m");
        return NULL;
    }

    int m = parseOrder(mS, 1);
    if (!m) return NULL;

    Rectangle* r = createRectangle("editor", n, m);
    Graph* res = r->g;
    for (int i = 0; i < r->height; i++) {
        free(r->table[i]);
    }
    free(r->table);
    free(r);
    strcpy(res->name, "CompleteBipartite_Editor");
    relabelGraph(res);
    return res;
}

Graph* antiprismCmd(char* argBuff) {
    char orderStr[MAX_ORDER_NUMERALS + 1];
    int argSize = parseWord(0, MAX_ORDER_NUMERALS, argBuff, orderStr);

    if (argSize == 1) {
        missingArg(ANTIPRISM_CMD, "order");
        return NULL;
    }

    int n = parseOrder(orderStr, 3);
    if (!n) return NULL;

    return createAntiprism("Antiprism_editor", n);
}

Graph* emptyCmd(char* argBuff) {
    char orderStr[MAX_ORDER_NUMERALS + 1];
    int argSize = parseWord(0, MAX_ORDER_NUMERALS, argBuff, orderStr);

    if (argSize == 1) {
        missingArg(EMPTY_CMD, "order");
        return NULL;
    }

    int n = parseOrder(orderStr, 1);
    if (!n) return NULL;

    Graph* g = createGraph("editor", n);
    for (int i = 0; i < n; i++) {
        char name[BUFFER_SIZE];
        sprintf(name, "v%d", i);
        addVertex(g, createVertex(name, i + 1));
    }
    return g;
}

/**
 * Parses the command give and returns the list of arguments along with the
 * number associated with the command
 * @param inputBuff The buffer to parse
 */

void relabelLogic(Graph* g, char* argBuff) {

    int relabelEdges = 0;

    char edgeArgStr[MAX_CMD_SIZE];
    int argSize = parseWord(0, MAX_CMD_SIZE - 1, argBuff, edgeArgStr);

    if (argSize > 1) {
        if (strcmp(edgeArgStr, EDGE_ARG) == 0) {
            printf(ANSI_COLOR_YELLOW
                   "WARNING: You are about to relabel all "
                   "edges, is that okay ? [y/N]" ANSI_RESET_ALL "\n");
            relabelEdges = 1;
        } else {
            printf(ANSI_BACKGROUND_RED
                   "argument %s is not valid, did you mean %s ?",
                   edgeArgStr, EDGE_ARG);
            return;
        }
    } else {
        printf(ANSI_COLOR_YELLOW "WARNING: You are about to relabel all "
                                 "graph, is that okay ? [y/N]" ANSI_RESET_ALL
                                 "\n");
    }
    char confirmation[3];
    fgets(confirmation, 3, stdin);
    if (confirmation[0] == 'y') {
        printf(ANSI_COLOR_GREEN
               "Graph labeling is now consecutive" ANSI_RESET_ALL "\n");

        if (relabelEdges) {
            int vertexLabels[g->nbv];
            for (int i = 0; i < g->nbv; i++) {
                vertexLabels[i] = g->vertices[i].label;
            }
            relabelEdgesGraph(g, vertexLabels, g->nbv);
        } else relabelGraph(g);
        printGraph(g);
        printf("\n");
    } else {
        printf(ANSI_COLOR_RED "Graph labeling did not changed" ANSI_RESET_ALL
                              "\n");
    }
}

void labelLogic(Graph* g, char* argBuff) {

    int labelChange = 0;
    int previousIgnored = 0;
    int nxtIndex = 0;
    while (1) {

        char elementName[BUFFER_SIZE];
        int wordSize =
            parseWord(nxtIndex, BUFFER_SIZE - 1, argBuff, elementName);

        if (wordSize == 1) {
            if (labelChange || previousIgnored) break;
            printf(ANSI_COLOR_RED "Expected element name" ANSI_RESET_ALL "\n");
            return;
        }
        nxtIndex += wordSize;

        char labelS[MAX_ORDER_NUMERALS + 1];
        wordSize = parseWord(nxtIndex, MAX_ORDER_NUMERALS, argBuff, labelS);

        if (wordSize == 1) {
            printf(ANSI_COLOR_RED "Expected a label value" ANSI_RESET_ALL "\n");
            break;
        }

        nxtIndex += wordSize;
        int label = (int)strtol(labelS, NULL, 10);

        if (elementName[0] == 'v') {
            Vertex* v = getVertex(g, elementName);
            if (!v) {
                printf(ANSI_COLOR_RED "Vertex %s was not found" ANSI_RESET_ALL
                                      "\n",
                       elementName);
                previousIgnored = 1;
                continue;
            }
            labelChange++;
            v->label = label;
        }

        else if (elementName[0] == 'e') {
            Edge* e = getEdge(g, elementName);
            if (!e) {
                printf(ANSI_COLOR_RED "Edge %s was not found" ANSI_RESET_ALL
                                      "\n",
                       elementName);
                previousIgnored = 1;
                continue;
            }
            labelChange++;
            e->label = label;
        } else {
            printf(ANSI_COLOR_RED "Element %s does not exists" ANSI_RESET_ALL
                                  "\n",
                   elementName);
            previousIgnored = 1;
            continue;
        }
        previousIgnored = 0;
    }

    if (labelChange)
        printf(ANSI_COLOR_GREEN "Labels have changed !" ANSI_RESET_ALL "\n");
    else printf(ANSI_COLOR_RED "No label have changed..." ANSI_RESET_ALL "\n");

    printGraph(g);
}

void linkLogic(Graph* g, char* argBuff) {
    char vertexToLink[BUFFER_SIZE];
    int wordSize = parseWord(0, BUFFER_SIZE - 1, argBuff, vertexToLink);

    if (wordSize == 1) {
        printf(ANSI_COLOR_RED "Expected element name" ANSI_RESET_ALL "\n");
        return;
    }

    int nxtIndex = wordSize;

    Vertex* v1 = getVertex(g, vertexToLink);
    if (!v1) {
        printf(ANSI_COLOR_RED "Vertex %s was not found" ANSI_RESET_ALL "\n",
               vertexToLink);
        return;
    }

    int edgeCount = 0;
    int previousIgnored = 0;
    while (1) {
        char labelForEdge[MAX_ORDER_NUMERALS];
        wordSize = parseWord(nxtIndex, BUFFER_SIZE - 1, argBuff, labelForEdge);

        if (wordSize == 1) {
            if (previousIgnored || edgeCount) break;
            printf(ANSI_COLOR_RED "Expected label for the edge" ANSI_RESET_ALL
                                  "\n");
            return;
        }

        nxtIndex += wordSize;
        int label = (int)strtol(labelForEdge, NULL, 10);

        char vertex[BUFFER_SIZE];
        wordSize = parseWord(nxtIndex, BUFFER_SIZE - 1, argBuff, vertex);

        if (wordSize == 1) {
            printf(ANSI_COLOR_RED "Expected a vertex to link" ANSI_RESET_ALL
                                  "\n");
            break;
        }
        nxtIndex += wordSize;

        Vertex* v2 = getVertex(g, vertex);
        if (!v2) {
            printf(ANSI_COLOR_RED "Vertex %s was not found" ANSI_RESET_ALL "\n",
                   vertex);
            previousIgnored = 1;
            continue;
        }
        char name[BUFFER_SIZE];
        sprintf(name, "e%d", g->nbe);
        if (v1 == v2) {
            printf(ANSI_COLOR_YELLOW
                   "WARNING Ignored the creation of the edge thatconnect "
                   "%s to itself" ANSI_RESET_ALL "\n",
                   vertex);
            previousIgnored = 1;
            continue;
        }
        if (!addEdge(g, createEdge(name, label, *v1, *v2))) {
            previousIgnored = 1;
        } else {
            edgeCount++;
            previousIgnored = 0;
        }
    }
    if (edgeCount) printf(ANSI_COLOR_GREEN "Edges added !" ANSI_RESET_ALL "\n");
    if (!edgeCount)
        printf(ANSI_COLOR_RED "No new edges added..." ANSI_RESET_ALL "\n");

    printGraph(g);
}

void unlinkLogic(Graph* g, char* argBuff) {
    char vertexToUnlink[BUFFER_SIZE];
    int wordSize = parseWord(0, BUFFER_SIZE - 1, argBuff, vertexToUnlink);

    if (wordSize == 1) {
        printf(ANSI_COLOR_RED "Expected element name" ANSI_RESET_ALL "\n");
        return;
    }
    int nxtIndex = wordSize;

    Vertex* v1 = getVertex(g, vertexToUnlink);
    if (!v1) {
        printf(ANSI_COLOR_RED "Vertex %s was not found" ANSI_RESET_ALL "\n",
               vertexToUnlink);
        return;
    }
    IssuedEdges* issuedFromv1 = getIssuedEdges(g, v1);
    int edgeCount = 0;
    int previousIgnored = 0;
    while (1) {
        char vertex[BUFFER_SIZE];
        wordSize = parseWord(nxtIndex, BUFFER_SIZE - 1, argBuff, vertex);

        if (wordSize == 1) {
            if (previousIgnored || edgeCount) break;
            printf(ANSI_COLOR_RED "Expected a vertex to unlink" ANSI_RESET_ALL
                                  "\n");
            return;
        }
        nxtIndex += wordSize;

        Vertex* v2 = getVertex(g, vertex);
        if (!v2) {
            printf(ANSI_COLOR_RED "Vertex %s was not found" ANSI_RESET_ALL "\n",
                   vertex);
            previousIgnored = 1;
            continue;
        }
        Edge* e = getEdgeByVertex(issuedFromv1, v2);
        if (!e) {
            printf(ANSI_COLOR_RED
                   "Edge between %s and %s was not found" ANSI_RESET_ALL "\n",
                   vertexToUnlink, vertex);
            previousIgnored = 1;
            continue;
        } else {
            deleteEdge(g, *e);
            previousIgnored = 0;
            edgeCount++;
        }
    }
    if (edgeCount)
        printf(ANSI_COLOR_GREEN "Edges removed !" ANSI_RESET_ALL "\n");
    if (!edgeCount)
        printf(ANSI_COLOR_RED "No edges were removed..." ANSI_RESET_ALL "\n");

    printGraph(g);
}

void saveLogic(Graph* g, char* argBuff) {
    char filename[BUFFER_SIZE];
    int wordSize = parseWord(0, BUFFER_SIZE - 1, argBuff, filename);

    if (wordSize == 1) {
        missingArg("save", "filename");
        return;
    }

    generateDotFile(filename, g, 0);
}

int parseWord(int startingPoint, int readSize, const char* toRead, char* word) {
    char currChr;
    for (int i = 0; i < readSize; i++) {
        currChr = toRead[startingPoint + i];
        if (currChr == '\n' || currChr == '\0' || currChr == ' ') {
            word[i] = '\0';
            return i + 1;
        }
        word[i] = toRead[startingPoint + i];
    }
    word[readSize] = '\0';
    return readSize + 1;
}

Edge* getEdgeByVertex(IssuedEdges* issuedEdges, Vertex* v2) {
    for (int i = 0; i < issuedEdges->size; i++) {
        Vertex* vtab = issuedEdges->issuedEdges[i]->vertices;
        if (strcmp(vtab[0].name, v2->name) == 0 ||
            strcmp(vtab[1].name, v2->name) == 0)
            return issuedEdges->issuedEdges[i];
    }
    return NULL;
}
