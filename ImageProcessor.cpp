#include "ImageProcessor.h"
#include <iostream>

ImageProcessor::ImageProcessor(const std::string& image_path, BuddyAllocator& allocator)
    : input_filename(image_path), loaded_ok(false), allocator(allocator) {

    cv::Mat original = cv::imread(image_path, cv::IMREAD_COLOR);
    if (!original.data) {
        std::cerr << "No se pudo cargar la imagen: " << image_path << "\n";
        return;
    }

    original_width = original.cols;
    original_height = original.rows;
    original_channels = original.channels();

    size_t image_size = original.total() * original.elemSize();  // total bytes
    void* buffer = allocator.allocate(image_size);
    if (!buffer) {
        std::cerr << "No se pudo asignar memoria con BuddyAllocator\n";
        return;
    }

    // Copiamos los datos originales a nuestro buffer
    std::memcpy(buffer, original.data, image_size);

    // Creamos el Mat con ese buffer (sin copiar otra vez)
    image_mat = cv::Mat(original.rows, original.cols, original.type(), buffer);
    loaded_ok = true;
}

bool ImageProcessor::isLoaded() const {
    return loaded_ok;
}

void ImageProcessor::displayInfo(const std::string& title) const {
    if (!loaded_ok) {
        std::cerr << "Imagen no cargada.\n";
        return;
    }
    std::cout << title << "\n";
    std::cout << "Dimensiones actuales: " << image_mat.cols << " x " << image_mat.rows << "\n";
    std::cout << "Canales: " << image_mat.channels() << "\n";
}

int ImageProcessor::getOriginalWidth() const { return original_width; }
int ImageProcessor::getOriginalHeight() const { return original_height; }
int ImageProcessor::getOriginalChannels() const { return original_channels; }

int ImageProcessor::getCurrentWidth() const { return image_mat.cols; }
int ImageProcessor::getCurrentHeight() const { return image_mat.rows; }
int ImageProcessor::getCurrentChannels() const { return image_mat.channels(); }

const cv::Mat& ImageProcessor::getMat() const {
    return image_mat;
}

void ImageProcessor::rotateCenterClockwise(double angle_degrees) {
    if (!loaded_ok) return;

    cv::Point2f center(image_mat.cols / 2.0f, image_mat.rows / 2.0f);
    cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle_degrees, 1.0);
    cv::Mat rotated;

    cv::warpAffine(image_mat, rotated, rotation_matrix, image_mat.size());
    image_mat = rotated;
}

void ImageProcessor::saveImage(const std::string& output_path) const {
    if (!loaded_ok) {
        std::cerr << "No hay imagen cargada para guardar.\n";
        return;
    }
    cv::imwrite(output_path, image_mat);
}
