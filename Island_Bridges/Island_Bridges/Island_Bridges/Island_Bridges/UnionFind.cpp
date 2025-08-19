#include "UnionFind.hpp"

UnionFind::UnionFind(int _tamano) : tamano(_tamano) {
    padre = new int[tamano];
    rango = new int[tamano];
    for (int i = 0; i < tamano; ++i) {
        padre[i] = i;
        rango[i] = 0;
    }
}

UnionFind::~UnionFind() {
    delete[] padre;
    delete[] rango;
}

int UnionFind::encontrar(int nodo) {
    if (padre[nodo] != nodo) {
        padre[nodo] = encontrar(padre[nodo]);
    }
    return padre[nodo];
}

void UnionFind::unir(int nodoA, int nodoB) {
    int raizA = encontrar(nodoA);
    int raizB = encontrar(nodoB);
    if (raizA != raizB) {
        if (rango[raizA] < rango[raizB]) {
            padre[raizA] = raizB;
        } else if (rango[raizA] > rango[raizB]) {
            padre[raizB] = raizA;
        } else {
            padre[raizB] = raizA;
            rango[raizA]++;
        }
    }
}
