// ImageProcessor.cpp
#include "ImageProcessor.h"
#include <iostream>
#include <opencv2/imgproc.hpp> // Necesario para getRotationMatrix2D y warpAffine

// Constructor
ImageProcessor::ImageProcessor(const std::string& image_path)
    : input_filename(image_path),
      loaded_ok(false),
      original_width(0),
      original_height(0),
      original_channels(0)
{
    image_mat = cv::imread(image_path, cv::IMREAD_UNCHANGED);

    if (image_mat.empty()) {
        std::cerr << "Error: No se pudo cargar la imagen desde '" << image_path << "'" << std::endl;
        loaded_ok = false;
    } else {
        std::cout << "[INFO] Imagen '" << input_filename << "' cargada correctamente." << std::endl;

        // Guardar dimensiones originales
        original_width = image_mat.cols;
        original_height = image_mat.rows;
        original_channels = image_mat.channels();


        // Asegurarse de que tenga al menos 3 canales (BGR) si es posible
        if (image_mat.channels() == 1) {
             std::cout << "[INFO] Imagen en escala de grises detectada. Convirtiendo a BGR." << std::endl;
             cv::cvtColor(image_mat, image_mat, cv::COLOR_GRAY2BGR);
             // Actualizar canales originales si la conversión cambió el número
             original_channels = image_mat.channels(); // Ahora será 3
        }
        // Si es BGRA (4 canales), la dejamos así por ahora.
        loaded_ok = true;
    }
}

// Verifica si la imagen se cargó
bool ImageProcessor::isLoaded() const {
    return loaded_ok;
}

// Muestra información
void ImageProcessor::displayInfo(const std::string& title) const {
    if (!loaded_ok) {
        std::cerr << "Error: No hay imagen cargada para mostrar información." << std::endl;
        return;
    }
    std::cout << "\n--- " << title << " ---" << std::endl;
    std::cout << "Archivo de entrada: " << input_filename << std::endl;
    std::cout << "Dimensiones Originales: " << original_width << " x " << original_height << std::endl;
    std::cout << "Dimensiones Actuales: " << image_mat.cols << " x " << image_mat.rows << " (ancho x alto)" << std::endl;
    std::cout << "Canales: " << image_mat.channels();
    switch (image_mat.channels()) {
        case 3: std::cout << " (BGR)"; break;
        case 4: std::cout << " (BGRA)"; break;
        default: std::cout << " (" << image_mat.channels() << " canales)"; break;
    }
    std::cout << std::endl;
    std::cout << "---------------------------------------\n" << std::endl;
}

// Getters para dimensiones originales
int ImageProcessor::getOriginalWidth() const { return original_width; }
int ImageProcessor::getOriginalHeight() const { return original_height; }
int ImageProcessor::getOriginalChannels() const { return original_channels; }

// Getters para dimensiones actuales
int ImageProcessor::getCurrentWidth() const { return loaded_ok ? image_mat.cols : 0; }
int ImageProcessor::getCurrentHeight() const { return loaded_ok ? image_mat.rows : 0; }
int ImageProcessor::getCurrentChannels() const { return loaded_ok ? image_mat.channels() : 0; }


// Obtiene el Mat (versión constante)
const cv::Mat& ImageProcessor::getMat() const {
    return image_mat;
}

// *** NUEVO: Implementación de la rotación ***
void ImageProcessor::rotateCenterClockwise(double angle_degrees) {
    if (!loaded_ok) {
        std::cerr << "Error: No se puede rotar una imagen no cargada." << std::endl;
        return;
    }
    std::cout << "[INFO] Rotando imagen " << angle_degrees << " grados (clockwise)..." << std::endl;

    // Centro de la imagen
    cv::Point2f center(static_cast<float>(image_mat.cols) / 2.0f, static_cast<float>(image_mat.rows) / 2.0f);

    // Obtener la matriz de rotación 2D
    // OpenCV rota counter-clockwise por defecto, así que usamos el ángulo negativo para rotar clockwise
    cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle_degrees, 1.0); // Escala 1.0

    // Crear una Matriz para la imagen rotada (mismo tamaño y tipo que la original)
    cv::Mat rotated_image;

    cv::warpAffine(image_mat, rotated_image, rotation_matrix, image_mat.size(),
                   cv::INTER_LINEAR,       // Interpolación bilineal
                   cv::BORDER_CONSTANT,    // Rellenar bordes con un valor constante
                   cv::Scalar(0, 0, 0, 0)); // Color negro (BGR o BGRA)

    // Reemplazar la imagen interna con la imagen rotada
    image_mat = rotated_image;

    std::cout << "[INFO] Rotación completada." << std::endl;
}

// *** NUEVO: Implementación para guardar la imagen ***
void ImageProcessor::saveImage(const std::string& output_path) const {
    if (!loaded_ok || image_mat.empty()) {
        std::cerr << "Error: No hay imagen procesada para guardar." << std::endl;
        return;
    }

    try {
        bool success = cv::imwrite(output_path, image_mat);
        if (success) {
            std::cout << "[INFO] Imagen guardada correctamente en '" << output_path << "'" << std::endl;
        } else {
            std::cerr << "Error: No se pudo guardar la imagen en '" << output_path << "'" << std::endl;
        }
    } catch (const cv::Exception& ex) {
        std::cerr << "Error de OpenCV al guardar la imagen: " << ex.what() << std::endl;
    }
}