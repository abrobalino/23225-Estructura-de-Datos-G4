#ifndef MIVECTOR_HPP
#define MIVECTOR_HPP

template <typename T>
class MiListaDoble {
private:
    struct Nodo {
        T dato;
        Nodo* anterior;
        Nodo* siguiente;

        Nodo(const T& val) : dato(val), anterior(nullptr), siguiente(nullptr) {}
    };

    Nodo* cabeza;
    Nodo* cola;
    int cantidad;

public:
    MiListaDoble() {
        cabeza = cola = nullptr;
        cantidad = 0;
    }

    ~MiListaDoble() {
        limpiar();
    }

    void agregarAlFinal(const T& valor) {
        Nodo* nuevo = new Nodo(valor);
        if (!cabeza) {
            cabeza = cola = nuevo;
        } else {
            cola->siguiente = nuevo;
            nuevo->anterior = cola;
            cola = nuevo;
        }
        cantidad++;
    }

    void eliminarPrimero() {
        if (!cabeza) return;
        Nodo* temp = cabeza;
        cabeza = cabeza->siguiente;
        if (cabeza) cabeza->anterior = nullptr;
        else cola = nullptr;
        delete temp;
        cantidad--;
    }

    T& operator[](int i) {
        if (i < 0 || i >= cantidad) {
            throw "Índice fuera de rango";
        }
        Nodo* actual = cabeza;
        for (int j = 0; j < i; ++j) {
            actual = actual->siguiente;
        }
        return actual->dato;
    }

    const T& operator[](int i) const {
        if (i < 0 || i >= cantidad) {
            throw "Índice fuera de rango";
        }
        Nodo* actual = cabeza;
        for (int j = 0; j < i; ++j) {
            actual = actual->siguiente;
        }
        return actual->dato;
    }

    int tamano() const {
        return cantidad;
    }

    void limpiar() {
        while (cabeza) {
            Nodo* temp = cabeza;
            cabeza = cabeza->siguiente;
            delete temp;
        }
        cola = nullptr;
        cantidad = 0;
    }
};


#endif
