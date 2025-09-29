#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <filesystem>
#include <string>
#include <cstring>
#include <omp.h>
using namespace std;

int main() {
    double start_time = omp_get_wtime();

    // List of input image filenames
    vector<string> inputFilenames = {"image1.png", "image2.png", "image3.png","image4.png","image5.png","image6.png"}; // Add more as needed

    // Create output folders if they don't exist
    std::filesystem::create_directory("negative");
    std::filesystem::create_directory("edge");

    int totalProcessed = 0;
    for (const auto& inputFilename : inputFilenames) {
        int width, height, channels;

        // Extract base filename (without path)
        string inputPath(inputFilename);
        size_t pos = inputPath.find_last_of("/\\");
        string baseName = (pos == string::npos) ? inputPath : inputPath.substr(pos + 1);

        // Load PNG (converted to grayscale)
        unsigned char* img = stbi_load(inputFilename.c_str(), &width, &height, &channels, 1);
        if (!img) {
            cerr << "Error: Could not load " << inputFilename << "\n";
            continue;
        }

        cout << "Image loaded: " << inputFilename << " - " << width << "x" << height << " (channels=" << channels << ")\n";

        // Convert to 2D vector
        vector<vector<int>> image(height, vector<int>(width));
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j] = img[i * width + j];  // grayscale value (0–255)
            }
        }

        // Negative transformation
        vector<unsigned char> negativeData(width * height);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                negativeData[i * width + j] = 255 - image[i][j];
            }
        }
        string negativePath = "negative/" + baseName;
        stbi_write_png(negativePath.c_str(), width, height, 1, negativeData.data(), width);

        // Sobel edge detection
        int Gx[3][3] = { {-1,0,1}, {-2,0,2}, {-1,0,1} };
        int Gy[3][3] = { {-1,-2,-1}, {0,0,0}, {1,2,1} };

        vector<unsigned char> edgesData(width * height, 0);
        for (int i = 1; i < height - 1; i++) {
            for (int j = 1; j < width - 1; j++) {
                int sumX = 0, sumY = 0;
                for (int m = -1; m <= 1; m++) {
                    for (int n = -1; n <= 1; n++) {
                        int pixel = image[i+m][j+n];
                        sumX += pixel * Gx[m+1][n+1];
                        sumY += pixel * Gy[m+1][n+1];
                    }
                }
                int mag = sqrt(sumX*sumX + sumY*sumY);
                if (mag > 255) mag = 255;
                edgesData[i * width + j] = mag;
            }
        }
        string edgePath = "edge/" + baseName;
        stbi_write_png(edgePath.c_str(), width, height, 1, edgesData.data(), width);

        cout << "Processed " << inputFilename << ": Saved " << negativePath << " and " << edgePath << "\n";
        stbi_image_free(img);
        totalProcessed++;
    }
    double end_time = omp_get_wtime();
    cout << "Batch processing complete! " << totalProcessed << " images processed.\n";
    cout << "Total completion time: " << (end_time - start_time) << " seconds\n";
    return 0;
}
