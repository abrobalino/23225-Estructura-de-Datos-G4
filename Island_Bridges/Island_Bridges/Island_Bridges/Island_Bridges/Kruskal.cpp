#include "Kruskal.hpp"
#include <iostream>
using namespace std;


int partition(Bridge arr[], int low, int high) {
    Bridge pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;

            Bridge temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    Bridge temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;

    return (i + 1);
}

void quickSort(Bridge arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int Kruskal::calcularCostoOptimo(const Graph& grafo) {
    const MiListaDoble<Bridge>& puentes = grafo.obtenerPuentes();
    int n = grafo.obtenerIslas().tamano();


    int tamano = puentes.tamano();
    Bridge* arreglo = new Bridge[tamano];

    for (int i = 0; i < tamano; ++i) {
        arreglo[i] = puentes[i];
    }


    quickSort(arreglo, 0, tamano - 1);


    UnionFind dsu(n);
    int costoTotal = 0;
    int aristasUsadas = 0;

    for (int i = 0; i < tamano && aristasUsadas < n - 1; ++i) {
        int a = arreglo[i].obtenerIslaA();
        int b = arreglo[i].obtenerIslaB();
        int costo = arreglo[i].obtenerCosto();

        if (dsu.encontrar(a) != dsu.encontrar(b)) {
            dsu.unir(a, b);
            costoTotal += costo;
            aristasUsadas++;
            cout << "Puente agregado entre Isla " << a << " e Isla " << b
                 << ", Costo: " << costo << endl;
        }
    }

    delete[] arreglo;
    cout << "Costo total del árbol de expansión mínima: " << costoTotal << endl;
    return costoTotal;
}
