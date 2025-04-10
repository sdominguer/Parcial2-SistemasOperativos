// main.cpp
#include <iostream>
#include <string>
#include <vector>
#include <limits> // Para limpiar el buffer de entrada
#include "ImageProcessor.h"
#include "BuddyAllocator.h"

int main(int argc, char* argv[]) {
    std::cout << "=== PROCESAMIENTO DE IMAGEN (Parte 1 y 2: Carga y Rotación) ===" << std::endl;

    if (argc < 2) {
        std::cerr << "Error: Se requiere el nombre del archivo de imagen de entrada." << std::endl;
        std::cerr << "Uso: " << argv[0] << " <entrada.jpg>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];
    std::string output_filename = "imagen_rotada.png";

    // --- Inicializar el Buddy Allocator ---
    const size_t memory_pool_size = 1024 * 1024 * 20; // 20 MB
    BuddyAllocator buddyAllocator(memory_pool_size);

    std::cout << "Memoria total disponible: " << memory_pool_size << " bytes" << std::endl;

    // --- Cargar la imagen usando el Buddy Allocator ---
    ImageProcessor processor(input_filename, buddyAllocator);

    if (!processor.isLoaded()) {
        return 1;
    }

    processor.displayInfo("Información de la Imagen Original");

    // --- Mostrar uso de memoria antes de la rotación ---
    std::cout << "Memoria usada antes de rotar: " << buddyAllocator.getUsedMemory() << " bytes" << std::endl;

    // --- Pedir ángulo ---
    double rotation_angle = 0.0;
    std::cout << "Introduce el ángulo de rotación deseado (en grados, sentido horario - clockwise): ";
    while (!(std::cin >> rotation_angle)) {
        std::cerr << "Entrada inválida. Por favor, introduce un número para el ángulo: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // --- Rotar imagen ---
    processor.rotateCenterClockwise(rotation_angle);

    // --- Mostrar uso de memoria después de la rotación ---
    std::cout << "Memoria usada después de rotar: " << buddyAllocator.getUsedMemory() << " bytes" << std::endl;

    // --- Guardar imagen ---
    processor.saveImage(output_filename);

    std::cout << "\n=== Procesamiento Finalizado ===" << std::endl;

    return 0;
}
