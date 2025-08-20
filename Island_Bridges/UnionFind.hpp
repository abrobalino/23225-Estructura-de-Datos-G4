#ifndef UNIONFIND_HPP
#define UNIONFIND_HPP

class UnionFind {
private:
    int* padre;
    int* rango;
    int tamano;

public:
    UnionFind(int n);
    ~UnionFind();
    int encontrar(int x);
    void unir(int x, int y);
};

#endif
