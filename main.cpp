// main.cpp
#include "ImageProcessor.h"   // Incluir nuestra clase Imagen
#include <iostream>
#include <string>
#include <limits>    // Para limpiar buffer de entrada std::numeric_limits

int main() {
    std::cout << "=== PROCESAMIENTO MANUAL DE IMAGEN (Rotación CCW) ===" << std::endl;

    // --- Simulación de Carga (Parte 1) ---
    const int ANCHO_IMG = 300;
    const int ALTO_IMG = 200;
    const int CANALES_IMG = 3; // RGB
    std::string archivo_salida = "imagen_rotada_final.ppm";

    std::cout << "\n[Paso 1] Creando imagen de ejemplo (" << ANCHO_IMG << "x" << ALTO_IMG << ")...\n";
    Imagen imagen_original(ANCHO_IMG, ALTO_IMG, CANALES_IMG);

    if (!imagen_original.esValida()) {
        std::cerr << "Error crítico: No se pudo crear la imagen original. Saliendo." << std::endl;
        return 1;
    }

    // Llenar con un patrón (ej: rojo en la mitad izquierda, verde en la derecha)
    for (int y = 0; y < imagen_original.getAlto(); ++y) {
        for (int x = 0; x < imagen_original.getAncho(); ++x) {
            if (x < imagen_original.getAncho() / 2) {
                imagen_original.establecerPixel(x, y, 255, 0, 0); // Rojo
            } else {
                imagen_original.establecerPixel(x, y, 0, 255, 0); // Verde
            }
        }
    }
     std::cout << "[Paso 1] Imagen de ejemplo creada y llenada.\n";
    std::cout << "          Dimensiones: " << imagen_original.getAncho() << "x" << imagen_original.getAlto() << "\n";

    // --- Rotación (Parte 2) ---
    double angulo = 0.0;
    std::cout << "\n[Paso 2] Introduce el ángulo de rotación (grados, sentido ANTI-HORARIO): ";
    while (!(std::cin >> angulo)) {
        std::cerr << "Entrada inválida. Introduce un número: ";
        std::cin.clear(); // Limpiar flag de error
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Descartar buffer
    }

    // Llamar al método de rotación
    Imagen imagen_rotada = imagen_original.rotarCentroCounterClockwise(angulo);

    if (!imagen_rotada.esValida()) {
        std::cerr << "Error: La operación de rotación falló o devolvió una imagen inválida." << std::endl;
        // La memoria de imagen_original se libera al salir de main
        return 1;
    }
     std::cout << "[Paso 2] Rotación aplicada.\n";
     std::cout << "          Dimensiones rotada: " << imagen_rotada.getAncho() << "x" << imagen_rotada.getAlto() << "\n"; // Mismas dimensiones en esta implementación

    // --- Guardado ---
    std::cout << "\n[Paso 3] Intentando guardar la imagen rotada en '" << archivo_salida << "'...\n";
    if (imagen_rotada.guardarPPM(archivo_salida)) {
        std::cout << "[Paso 3] Guardado exitoso. Puedes abrirla con GIMP, IrfanView, etc.\n";
    } else {
         std::cerr << "[Paso 3] Fallo al guardar la imagen rotada.\n";
    }


    std::cout << "\n=== Procesamiento Finalizado ===\n" << std::endl;

    // Los destructores de imagen_original e imagen_rotada se llamarán aquí automáticamente (RAII)
    return 0;
}