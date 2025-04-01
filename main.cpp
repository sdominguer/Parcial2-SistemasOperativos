// main.cpp
#include <iostream>
#include <string>
#include <vector>
#include <limits> // Para limpiar el buffer de entrada
#include "ImageProcessor.h" // Incluir nuestra clase

int main(int argc, char* argv[]) {
    std::cout << "=== PROCESAMIENTO DE IMAGEN (Parte 1 y 2: Carga y Rotación) ===" << std::endl;

    // --- Procesamiento de argumentos de línea de comandos (Simplificado) ---
    if (argc < 2) {
        std::cerr << "Error: Se requiere el nombre del archivo de imagen de entrada." << std::endl;
        std::cerr << "Uso: " << argv[0] << " <entrada.jpg>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];
    // Nombre de archivo de salida fijo por ahora
    std::string output_filename = "imagen_rotada.png";

    // --- Carga de la Imagen ---
    ImageProcessor processor(input_filename);

    // Verificar si la carga fue exitosa
    if (!processor.isLoaded()) {
        return 1; // El error ya se imprimió
    }

    // --- Mostrar Información Original ---
    processor.displayInfo("Información de la Imagen Original");

    // --- Pedir Ángulo de Rotación al Usuario ---
    double rotation_angle = 0.0;
    std::cout << "Introduce el ángulo de rotación deseado (en grados, sentido horario - clockwise): ";
    while (!(std::cin >> rotation_angle)) {
        std::cerr << "Entrada inválida. Por favor, introduce un número para el ángulo: ";
        std::cin.clear(); // Limpiar el estado de error
        // Descartar la entrada incorrecta del buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // --- Rotar la Imagen ---
    processor.rotateCenterClockwise(rotation_angle);

    // --- Guardar la Imagen Procesada ---
    processor.saveImage(output_filename);

    std::cout << "\n=== Procesamiento Finalizado ===" << std::endl;

    return 0; // Éxito
}