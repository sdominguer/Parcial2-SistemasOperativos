// Image.h
#ifndef IMAGE_H
#define IMAGE_H

#include <string> // Para std::string

// Declaración adelantada si es necesario por dependencias circulares (no en este caso)

// Estructura para representar la imagen manualmente
struct Imagen {
    // --- Miembros de Datos ---
    unsigned char* pixeles = nullptr; // Puntero a los datos (array 1D: R1G1B1 R2G2B2 ...)
    int ancho = 0;
    int alto = 0;
    int canales = 0; // Usualmente 3 para RGB

    // --- Constructores / Destructor / Asignación (RAII - Regla de 5) ---
    Imagen() = default; // Constructor por defecto
    Imagen(int w, int h, int c); // Constructor principal (asigna memoria)
    ~Imagen(); // Destructor (libera memoria)

    // Deshabilitar Copia
    Imagen(const Imagen&) = delete;
    Imagen& operator=(const Imagen&) = delete;

    // Habilitar Movimiento
    Imagen(Imagen&& other) noexcept;
    Imagen& operator=(Imagen&& other) noexcept;

    // --- Métodos Públicos ---
    bool esValida() const; // Verifica si la imagen tiene datos válidos
    int getAncho() const;
    int getAlto() const;
    int getCanales() const;

    // Establece el color de un píxel (RGB) - retorna false si falla
    bool establecerPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);

    // Obtiene el color de un píxel (RGB) - retorna false si falla
    bool obtenerPixel(int x, int y, unsigned char& r, unsigned char& g, unsigned char& b) const;

    // Rota la imagen actual y devuelve una *nueva* imagen rotada
    // Ángulo en grados, positivo es Counter-Clockwise
    Imagen rotarCentroCounterClockwise(double angulo_grados) const;

    // Guarda la imagen en formato PPM (P3 - texto simple)
    bool guardarPPM(const std::string& nombre_archivo) const;

private:
    // --- Métodos Privados Auxiliares ---
    // Calcula el índice en el array 1D, retorna -1 si está fuera de límites
    long long getIndiceBase(int x, int y) const;

    // (Los métodos de interpolación podrían ir aquí o como funciones libres en Image.cpp)

}; // Fin struct Imagen

#endif // IMAGE_H