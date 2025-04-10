// Image.cpp
#include "ImageProcessor.h"
#include <iostream>
#include <vector>    // No estrictamente necesario ahora, pero útil a veces
#include <cmath>     // Para sin, cos, floor, M_PI
#include <algorithm> // Para std::min, std::max, std::fill
#include <fstream>   // Para guardar en archivo PPM
#include <new>       // Para std::nothrow

// Definir M_PI si no está disponible
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Implementaciones de Métodos de Imagen ---

// Constructor Principal
Imagen::Imagen(int w, int h, int c) : ancho(w), alto(h), canales(c), pixeles(nullptr) {
    if (w <= 0 || h <= 0 || c <= 0) {
        std::cerr << "[Error Imagen] Dimensiones inválidas (" << w << "x" << h << "x" << c << ")" << std::endl;
        ancho = alto = canales = 0;
        return;
    }
    size_t total_bytes = static_cast<size_t>(ancho) * alto * canales;
    std::cout << "[Memoria] Solicitando " << total_bytes << " bytes con 'new[]'..." << std::endl;
    // Usar std::nothrow para evitar excepción y poder comprobar el puntero
    pixeles = new(std::nothrow) unsigned char[total_bytes];

    if (!pixeles) {
        std::cerr << "[Error Memoria] Fallo al asignar " << total_bytes << " bytes." << std::endl;
        ancho = alto = canales = 0; // Marcar como inválida
    } else {
        std::fill(pixeles, pixeles + total_bytes, 0); // Inicializar a negro
        std::cout << "[Memoria] Asignación exitosa." << std::endl;
    }
}

// Destructor
Imagen::~Imagen() {
    if (pixeles) {
        std::cout << "[Memoria] Liberando memoria con 'delete[]'..." << std::endl;
        delete[] pixeles;
        pixeles = nullptr; // Importante poner a null
        std::cout << "[Memoria] Memoria liberada." << std::endl;
    }
}

// Constructor de Movimiento
Imagen::Imagen(Imagen&& other) noexcept
    : pixeles(other.pixeles), ancho(other.ancho), alto(other.alto), canales(other.canales)
{
    // El objeto 'other' ya no es dueño de la memoria
    other.pixeles = nullptr;
    other.ancho = 0;
    other.alto = 0;
    other.canales = 0;
    std::cout << "[Memoria] Imagen movida (constructor)." << std::endl;
}

// Asignación de Movimiento
Imagen& Imagen::operator=(Imagen&& other) noexcept {
    if (this != &other) { // Evitar auto-asignación
        // Liberar recursos propios antes de tomar los de 'other'
        delete[] pixeles;

        // Tomar posesión de los recursos de 'other'
        pixeles = other.pixeles;
        ancho = other.ancho;
        alto = other.alto;
        canales = other.canales;

        // Dejar a 'other' en un estado válido y vacío
        other.pixeles = nullptr;
        other.ancho = 0;
        other.alto = 0;
        other.canales = 0;
        std::cout << "[Memoria] Imagen movida (asignación)." << std::endl;
    }
    return *this;
}

bool Imagen::esValida() const {
    return pixeles != nullptr && ancho > 0 && alto > 0 && canales > 0;
}

int Imagen::getAncho() const { return ancho; }
int Imagen::getAlto() const { return alto; }
int Imagen::getCanales() const { return canales; }

long long Imagen::getIndiceBase(int x, int y) const {
    if (!esValida() || x < 0 || x >= ancho || y < 0 || y >= alto) {
        return -1; // Índice inválido
    }
    // Usar long long para cálculo intermedio evita desbordamiento con imágenes grandes
    return (static_cast<long long>(y) * ancho + x) * canales;
}

bool Imagen::establecerPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (canales < 3) return false; // Solo soportamos RGB por ahora
    long long indice_base = getIndiceBase(x, y);
    if (indice_base == -1) {
        return false; // Fuera de límites
    }
    pixeles[indice_base + 0] = r;
    pixeles[indice_base + 1] = g;
    pixeles[indice_base + 2] = b;
    // Ignorar canal alfa si existiera (canales == 4)
    return true;
}

bool Imagen::obtenerPixel(int x, int y, unsigned char& r, unsigned char& g, unsigned char& b) const {
     if (canales < 3) return false;
     long long indice_base = getIndiceBase(x, y);
     if (indice_base == -1) {
         r = g = b = 0; // Devolver negro si está fuera de límites
         return false;
     }
     r = pixeles[indice_base + 0];
     g = pixeles[indice_base + 1];
     b = pixeles[indice_base + 2];
     return true;
}


bool Imagen::guardarPPM(const std::string& nombre_archivo) const {
    if (!esValida() || canales < 3) {
         std::cerr << "[Error Guardar] Imagen inválida o no es RGB para guardar como PPM." << std::endl;
        return false;
    }
    std::ofstream archivo_salida(nombre_archivo);
    if (!archivo_salida) {
        std::cerr << "[Error Guardar] No se pudo abrir el archivo '" << nombre_archivo << "'." << std::endl;
        return false;
    }

    // Cabecera PPM P3 (RGB Texto)
    archivo_salida << "P3\n";
    archivo_salida << ancho << " " << alto << "\n";
    archivo_salida << "255\n"; // Valor máximo de color

    // Escribir datos de píxeles
    for (int y = 0; y < alto; ++y) {
        for (int x = 0; x < ancho; ++x) {
            long long indice_base = getIndiceBase(x, y);
            // Asumimos índice válido porque ya comprobamos esValida()
            unsigned char r = pixeles[indice_base + 0];
            unsigned char g = pixeles[indice_base + 1];
            unsigned char b = pixeles[indice_base + 2];
            archivo_salida << static_cast<int>(r) << " "
                           << static_cast<int>(g) << " "
                           << static_cast<int>(b) << "\n";
        }
    }

    archivo_salida.close();
    std::cout << "[INFO] Imagen guardada como '" << nombre_archivo << "'." << std::endl;
    return true;
}


// --- Lógica de Rotación e Interpolación (Funciones Auxiliares Libres) ---
//    (Se ponen aquí en el .cpp para no contaminar el .h)
namespace { // Namespace anónimo limita el alcance a este archivo .cpp

// Obtiene un componente de color de forma segura, clamping a bordes
unsigned char obtenerComponenteSeguro(const Imagen& img, int x, int y, int componente) {
    // Clamp (asegurar que las coordenadas estén dentro de los límites)
    x = std::max(0, std::min(img.getAncho() - 1, x));
    y = std::max(0, std::min(img.getAlto() - 1, y));

    long long indice_base = (static_cast<long long>(y) * img.getAncho() + x) * img.getCanales();
    if (componente < 0 || componente >= img.getCanales()) {
        return 0; // Devolver negro si el canal es inválido
    }
    return img.pixeles[indice_base + componente];
}


// Interpolación bilineal para un punto flotante (x_src, y_src) en la imagen fuente
unsigned char interpolacionBilineal(const Imagen& img_fuente, double x_src, double y_src, int componente) {
    // Asegurarse de que el punto base esté *ligeramente* dentro para evitar problemas
    // en el borde exacto al calcular x2/y2.
    x_src = std::max(0.0, std::min(static_cast<double>(img_fuente.getAncho() - 1.0001), x_src));
    y_src = std::max(0.0, std::min(static_cast<double>(img_fuente.getAlto() - 1.0001), y_src));

    int x1 = static_cast<int>(std::floor(x_src));
    int y1 = static_cast<int>(std::floor(y_src));
    int x2 = x1 + 1;
    int y2 = y1 + 1;

    double dx = x_src - x1;
    double dy = y_src - y1;
    double inv_dx = 1.0 - dx;
    double inv_dy = 1.0 - dy;

    // Obtener los 4 píxeles vecinos usando la función segura
    double p11 = obtenerComponenteSeguro(img_fuente, x1, y1, componente);
    double p12 = obtenerComponenteSeguro(img_fuente, x1, y2, componente);
    double p21 = obtenerComponenteSeguro(img_fuente, x2, y1, componente);
    double p22 = obtenerComponenteSeguro(img_fuente, x2, y2, componente);

    // Calcular interpolación
    double valor_interpolado = (p11 * inv_dx * inv_dy) + (p21 * dx * inv_dy) +
                               (p12 * inv_dx * dy) + (p22 * dx * dy);

    // Clamp final a [0, 255] y convertir
    return static_cast<unsigned char>(std::max(0.0, std::min(255.0, valor_interpolado)));
}

} // Fin namespace anónimo


// --- Implementación del Método de Rotación ---
Imagen Imagen::rotarCentroCounterClockwise(double angulo_grados) const {
     if (!esValida()) {
         std::cerr << "[Error Rotar] La imagen original no es válida." << std::endl;
         return Imagen(); // Devuelve imagen por defecto (inválida)
     }
     if (canales < 3) {
         std::cerr << "[Error Rotar] La rotación actual solo soporta imágenes RGB (3 canales)." << std::endl;
         return Imagen();
     }

    std::cout << "[INFO] Iniciando rotación manual (Counter-Clockwise)..." << std::endl;
    std::cout << "       Ángulo: " << angulo_grados << " grados" << std::endl;

    // Crear imagen de destino del mismo tamaño (podría calcularse un tamaño mayor)
    Imagen img_rotada(ancho, alto, canales);
    if (!img_rotada.esValida()) {
        std::cerr << "[Error Rotar] No se pudo crear la imagen de destino." << std::endl;
        return img_rotada; // Devuelve la imagen vacía que se intentó crear
    }

    double centro_x = static_cast<double>(ancho) / 2.0;
    double centro_y = static_cast<double>(alto) / 2.0;

    // Ángulo en radianes (positivo para CCW)
    double angulo_rad = angulo_grados * M_PI / 180.0;
    double cos_theta = std::cos(angulo_rad);
    double sin_theta = std::sin(angulo_rad);

    // Color de fondo (negro)
    const unsigned char COLOR_FONDO[3] = {0, 0, 0};

    // Iterar sobre píxeles de *destino* (Reverse Mapping)
    for (int y_dst = 0; y_dst < alto; ++y_dst) {
        for (int x_dst = 0; x_dst < ancho; ++x_dst) {

            // Coords relativas al centro del destino
            double x_rel_dst = static_cast<double>(x_dst) - centro_x;
            double y_rel_dst = static_cast<double>(y_dst) - centro_y;

            // Aplicar rotación inversa para hallar la posición fuente
            // (x_src_rel, y_src_rel) = R(-theta) * (x_rel_dst, y_rel_dst)
            double x_src_rel = x_rel_dst * cos_theta + y_rel_dst * sin_theta;
            double y_src_rel = -x_rel_dst * sin_theta + y_rel_dst * cos_theta;

            // Convertir a coords absolutas fuente
            double x_src = x_src_rel + centro_x;
            double y_src = y_src_rel + centro_y;

            // Comprobar si la fuente está dentro de los límites de la imagen original
            if (x_src >= 0.0 && x_src < ancho && y_src >= 0.0 && y_src < alto) {
                 // Interpolar cada componente de color
                unsigned char r = interpolacionBilineal(*this, x_src, y_src, 0); // this es la img original
                unsigned char g = interpolacionBilineal(*this, x_src, y_src, 1);
                unsigned char b = interpolacionBilineal(*this, x_src, y_src, 2);
                img_rotada.establecerPixel(x_dst, y_dst, r, g, b);
            } else {
                // Si está fuera, usar color de fondo
                img_rotada.establecerPixel(x_dst, y_dst, COLOR_FONDO[0], COLOR_FONDO[1], COLOR_FONDO[2]);
            }
        }
    }

    std::cout << "[INFO] Rotación manual completada." << std::endl;
    // std::move no es estrictamente necesario aquí debido a NRVO (Named Return Value Optimization)
    // pero es explícito. La imagen se moverá eficientemente.
    return img_rotada;
}