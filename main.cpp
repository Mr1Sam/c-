#include <iostream>
#include <fstream>
#include <vector>	
#include <cstdint>

// Структура заголовка BMP файла
#pragma pack(push, 1)
struct BMPHeader {
    char signature[2];
    std::uint32_t fileSize;
    std::uint16_t reserved1;
    std::uint16_t reserved2;
    std::uint32_t dataOffset;
    std::uint32_t headerSize;
    std::int32_t width;
    std::int32_t height;
    std::uint16_t planes;
    std::uint16_t bitsPerPixel;
    std::uint32_t compression;
    std::uint32_t imageSize;
    std::int32_t xPixelsPerM;
    std::int32_t yPixelsPerM;
    std::uint32_t colorsUsed;
    std::uint32_t colorsImportant;
};
#pragma pack(pop)

int main() {
    const char* inputFileName = "123.bmp";
    const char* outputFileName = "output.123.bmp";

    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Не удалось открыть входной файл." << std::endl;
        return 1;
    }

    BMPHeader header;
    inputFile.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    if (header.signature[0] != 'B' || header.signature[1] != 'M') {
        std::cerr << "Входной файл не является BMP." << std::endl;
        return 1;
    }

    if (header.bitsPerPixel != 24) {
        std::cerr << "Поддерживаются только изображения с глубиной цвета 24 бита." << std::endl;
        return 1;
    }

    // Вычисляем новые размеры изображения после поворота на 90 градусов
    const int newWidth = header.height;
    const int newHeight = header.width;

    // Рассчитываем размер новых данных изображения
    const std::uint32_t newDataSize = newWidth * newHeight * 3;

    // Записываем новые размеры в заголовок
    header.width = newWidth;
    header.height = newHeight;
    header.fileSize = header.dataOffset + newDataSize;

    std::ofstream outputFile(outputFileName, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Не удалось создать выходной файл." << std::endl;
        return 1;
    }

    // Записываем заголовок в выходной файл
    outputFile.write(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    // Выделяем буфер для данных пикселей
    std::vector<char> pixelData(newDataSize);

    // Считываем данные пикселей из входного файла
    inputFile.read(pixelData.data(), pixelData.size());

    // Создаем буфер для данных повернутых пикселей
    std::vector<char> rotatedPixelData(newDataSize);

    // Поворачиваем изображение на 90 градусов
    for (int y = 0; y < header.height; ++y) {
        for (int x = 0; x < header.width; ++x) {
            int rotatedX = header.width - 1 - x;
            int rotatedY = y;

            // Копируем пиксель из исходных данных в данные повернутых пикселей
            for (int channel = 0; channel < 3; ++channel) {
                rotatedPixelData[(rotatedY * header.width + rotatedX) * 3 + channel] =
                    pixelData[(y * header.width + x) * 3 + channel];
            }
        }
    }

    // Записываем данные повернутых пикселей в выходной файл
    outputFile.write(rotatedPixelData.data(), rotatedPixelData.size());

    std::cout << "Изображение успешно повернуто и сохранено в " << outputFileName << std::endl;

    return 0;
}