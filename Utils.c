#include "Utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int cmp(const void* a, const void* b) {
    return (*(const int*)a) - (*(const int*)b);
}

int cmp_inv(const void* a, const void* b) {
    return (*(const int*)b) - (*(const int*)a);
}

void permuteTab(int* tab, int i, int j) {
    int temp = tab[i];
    tab[i] = tab[j];
    tab[j] = temp;
}

int maxInTab(int* tab, int size) {
    int res = 0;
    for (int i = 0; i < size; i++) {
        if (tab[i] > res) { res = tab[i]; }
    }
    return res + 1;
}

void printTab(int* i, int len) {
    printf("[");
    for (int j = 0; j < len - 1; j++) {
        printf("%d,", i[j]);
    }
    if (len > 0) printf("%d", i[len - 1]);
    printf("]\n");
}

void printDoubleTab(double* i, int len) {
    // Affiche un tableau d'entiers
    printf("[");
    for (int j = 0; j < len; j++) {
        printf("%lf,", i[j]);
    }
    if (len > 0) printf("%lf", i[len - 1]);
    printf("]\n");
}

void showProgress(float k, float max) {
    if (k >= max) {
        printf("\r                                 \r");
        fflush(stdout);
        return;
    }
    printf("\rWorking... %.2f%% completed", k / max * 100);
    fflush(stdout);
}

int e10(int e) {
    int res = 1;
    for (int i = 0; i < e; i++) {
        res = res * 10;
    }
    return res;
}

double fact(int n) {
    double res = 1;
    for (int i = 2; i <= n; i++) {
        res = res * i;
    }
    return res;
}

double binom(int n, int k) {
    double res = 1;
    for (int i = n - k + 1; i <= n; i++) {
        res = res * i;
    }
    return res / fact(k);
}

int tabEquals(int* l1, int* l2, int t1, int t2) {
    if (t1 != t2) { return 0; }
    for (int i = 0; i < t1; i++) {
        if (l1[i] != l2[i]) { return 0; }
    }
    return 1;
}

int avgMagicConst(int nbv, int nbe) {

    double min = (binom(nbv + nbe + 1, 2) + binom(nbe + 1, 2)) / nbv;
    double max = ((2 * binom(nbv + nbe + 1, 2)) - binom(nbv + 1, 2)) / nbv;
    return round(((max - min) / 3) + min);
}

int inTab(int elem, int* tab, int size) {
    for (int i = 0; i < size; i++) {
        if (tab[i] == elem) { return 1; }
    }
    return 0;
}
