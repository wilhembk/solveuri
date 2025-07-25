#pragma once

#include "Graph.h"

#define BUFFER_SIZE 256
#define MAX_CMD_SIZE 16
#define MAX_ORDER_NUMERALS 5

#define OPEN_CMD "open"
#define COMPLETE_CMD "complete"
#define COMPLETE_BIPARTITE_CMD "complbip"
#define ANTIPRISM_CMD "antiprism"
#define EMPTY_CMD "begin"

#define SOLVE_CMD "solve"
#define RELABEL_CMD "relabel"
#define EDGE_ARG "edges"
#define LABEL_CMD "label"
#define LINK_CMD "link"
#define UNLINK_CMD "unlink"
#define SAVE_CMD "save"

/**
 * Starts the editor and returns the final graph
 * @param argc The argument count of the binary call
 * @param argv The arguments value of the binary call
 * @return The final version of the graph to solve
 */
Graph* startEditor(int argc, char** argv);

/**
 * Copies the first word it sees, seperated by a space
 * @param startingPoint The index on where to start
 * @param readSize The max amount of characters to read
 * @param toRead The string to read
 * @param word The pointer on wich we store the word, must be of size readSize
 * +1
 * @return The size of the word, couting the whitespace
 */
int parseWord(int startingPoint, int readSize, const char* toRead, char* word);

/**
 * Execute the command provided and returns the expected graph
 * @param cmdBuffer The buffer that contains the command
 * @return The wanted graph
 */
Graph* execCommand(char* cmdBuff);

/**
 * Execute the editing logic with the command provided
 * @param g The graph to edit
 * @param inputBuffer The buffer containing the editing instructions
 * @return 1 if further edit can be made, 0 if not
 */
int editingParser(Graph* g, char* inputBuffer);

/**
 * Utility funciton to obtain an edge given two vertices
 * @param issuedEdges The edges issued by the first vertex
 * @param v2 The second vertex
 * @return The corresponding edge or NULL if it does not exist
 */
Edge* getEdgeByVertex(IssuedEdges* issuedEdges, Vertex* v2);

/**
 * Prints the first command list
 */
void printCommandList();

/**
 * Prints the editor command list
 */
void printEditorExplanation();

/**
 * Clears terminal, but displays the binary call
 * @param argc The argument count of the binary call
 * @param argv The arguments value of the binary call
 */
void clear(int argc, char** argv);

/**
 * Prints an error message if arguments are missing
 * @param cmdName The command name that has missing arguments
 * @param argNeeded The name of the needed argument
 */
void missingArg(char* cmdName, char* argNeeded);

/**
 * Correctly parse the order of the graph
 * @param orderStr the order in the string format
 * @param min the minimum order allowed (couting the bound)
 */
int parseOrder(char* orderStr, int min);

/**
 * Executes the open command logic
 * @param argBuff The buffer containing the command argument
 * @return The expected graph
 */
Graph* openCmd(char* argBuff);

/**
 * Executes the complete command logic
 * @param argBuff The buffer containing the command argument
 * @return The expected graph
 */
Graph* completeCmd(char* argBuff);

/**
 * Executes the bipartite command logic
 * @param argBuff The buffer containing the command argument
 * @return The expected graph
 */
Graph* bipartiteCmd(char* argBuff);

/**
 * Executes the antiprism command logic
 * @param argBuff The buffer containing the command argument
 * @return The expected graph
 */
Graph* antiprismCmd(char* argBuff);

/**
 * Executes the empty command logic
 * @param argBuff The buffer containing the command argument
 * @return The expected graph
 */
Graph* emptyCmd(char* argBuff);

/**
 * Executes the editing "link" logic
 * @param g The graph to edit
 * @param argBuff The buffer containing the command arguments
 */
void linkLogic(Graph* g, char* argBuff);

/**
 * Executes the editing "unlink" logic
 * @param g The graph to edit
 * @param argBuff The buffer containing the command arguments
 */
void unlinkLogic(Graph* g, char* argBuff);

/**
 * Executes the editing "label" logic
 * @param g The graph to edit
 * @param argBuff The buffer containing the command arguments
 */
void labelLogic(Graph* g, char* argBuff);

/**
 * Executes the editing "relabel" logic
 * @param g The graph to edit
 * @param argBuff The buffer containing the command arguments
 */
void relabelLogic(Graph* g, char* argBuff);

void saveLogic(Graph* g, char* argBuff);
