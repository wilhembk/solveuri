#pragma once

#define ANSI_RESET_ALL "\x1b[0m"

#define ANSI_COLOR_BLACK "\x1b[30m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"

#define ANSI_BACKGROUND_BLACK "\x1b[40m"
#define ANSI_BACKGROUND_RED "\x1b[41m"
#define ANSI_BACKGROUND_GREEN "\x1b[42m"
#define ANSI_BACKGROUND_YELLOW "\x1b[43m"
#define ANSI_BACKGROUND_BLUE "\x1b[44m"
#define ANSI_BACKGROUND_MAGENTA "\x1b[45m"
#define ANSI_BACKGROUND_CYAN "\x1b[46m"
#define ANSI_BACKGROUND_WHITE "\x1b[47m"

#define ANSI_STYLE_BOLD "\x1b[1m"
#define ANSI_STYLE_ITALIC "\x1b[3m"
#define ANSI_STYLE_UNDERLINE "\x1b[4m"

#define BUFFER_SIZE 256

/**
 * Comparaison fonction in ascending order for integers
 * @param a The parameter to compare
 * @param b The parameter that is compared to
 * @return The comparison
 */
int cmp(const void* a, const void* b);

/**
 * Comparaison fonction in descending order for integers
 * @param a The parameter to compare
 * @param b The parameter that is compared to
 * @return The comparison
 */
int cmp_inv(const void* a, const void* b);

/**
 * Finds the maximum value in a table
 * @param tab A table of integers
 * @param size The size of thz table
 * @return The highest value that appears in the table
 */
int maxInTab(int* tab, int size);

/**
 * Permute the values with index i and j in a table
 * @param tab A table of integers
 * @param i The index of the first value to permute
 * @param J The index of the second value to permute
 */
void permuteTab(int* tab, int i, int j);

/**
 * Print a table in the shell
 * @param i A table of integers to print
 * @param len The size of table
 */
void printTab(int* i, int len);

/**
 * Print a table in the shell
 * @param i A table of doubles to print
 * @param len The size of table
 */
void printDoubleTab(double* i, int len);

/**
 * Print the percentage of progression during an iterated algorithm in the shell
 * @param k The number of iteration already done
 * @param max The maximum number of iteration authorized
 */
void showProgress(float k, float max);

/**
 * Elevate 10 to the power of a number
 * @param e The exponent
 * @return 10^e
 */
int e10(int e);

/**
 * Calculates the value of n!
 * @param n The integer
 * @return n!
 */
double fact(int n);

/**
 * Calculate the value of the binomial coefficient (n k)
 * @param n The first value
 * @param k The second value
 * @return (n k)
 */
double binom(int n, int k);

/**
 * Finds if two tables are the same
 * @param l1 A table of integers to compare
 * @param l2 A second table of integers to compare
 * @param t1 The saize of the first table
 * @param t2 The size of the second table
 * @return 1 if the two lists are the same, and 0 otherwise
 */
int tabEquals(int* l1, int* l2, int t1, int t2);

/**
 * Finds an optimal magic constant for a graph
 * @param nbv  The number of vertices of the graph
 * @param nbe The number of edges of the graph
 * @return an optimal magic constant for a graph
 */
int avgMagicConst(int nbv, int nbe);

/**
 * Finds if a value is present in a table
 * @param elem  The element to search in a table
 * @param tab The table of integers to search in
 * @param size The size of the table
 * @return 1 if elem is in tab and 0 otherwise
 */
int inTab(int elem, int* tab, int size);
