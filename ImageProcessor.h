// ImageProcessor.h
#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <string>
#include <opencv2/opencv.hpp> // Incluir cabecera principal de OpenCV
#include "BuddyAllocator.h"

class ImageProcessor {
private:
    cv::Mat image_mat;        // Objeto Mat de OpenCV para almacenar la imagen
    std::string input_filename;
    bool loaded_ok;
    // Almacenar las dimensiones originales por si se necesitan después
    int original_width;
    int original_height;
    int original_channels;
    BuddyAllocator allocator; //Instancia de Buddy Allocator

public:
    // Constructor: Intenta cargar la imagen desde la ruta
    ImageProcessor(const std::string& image_path, BuddyAllocator& allocator);

    // Verifica si la imagen se cargó correctamente
    bool isLoaded() const;

    // Muestra información básica de la imagen (dimensiones actuales)
    void displayInfo(const std::string& title = "Información de la Imagen") const;

    // Obtiene las dimensiones *originales* de la imagen
    int getOriginalWidth() const;
    int getOriginalHeight() const;
    int getOriginalChannels() const;

    // Obtiene las dimensiones *actuales* de la imagen
    int getCurrentWidth() const;
    int getCurrentHeight() const;
    int getCurrentChannels() const;


    // Obtiene el objeto Mat de OpenCV (constante)
    const cv::Mat& getMat() const;

    // *** Método para rotar la imagen en su centro (clockwise) ***
    void rotateCenterClockwise(double angle_degrees);

    // *** Método para guardar la imagen actual ***
    void saveImage(const std::string& output_path) const;

};

#endif // IMAGEPROCESSOR_H