#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <filesystem>
#include <string>
#include <omp.h>
using namespace std;

int main() {
    double start_time = omp_get_wtime();
    cout << "Albin John - 2023BCD0005\n";
    cout << "Rupsika T - 2023BCS0197\n";

    // List of input images
    vector<string> inputFilenames = {"image1.png", "image2.png", "image3.png", "image4.png", "image5.png", "image6.png"};

    // Create output folders
    std::filesystem::create_directory("negative");
    std::filesystem::create_directory("edge");

    int totalProcessed = 0;

    // ===================== PARALLELIZE BATCH PROCESSING =====================
    #pragma omp parallel for reduction(+:totalProcessed) schedule(dynamic)
    for (int idx = 0; idx < inputFilenames.size(); idx++) {
        const string& inputFilename = inputFilenames[idx];
        int width, height, channels;

        // Extract base name
        size_t pos = inputFilename.find_last_of("/\\");
        string baseName = (pos == string::npos) ? inputFilename : inputFilename.substr(pos + 1);

        // Load PNG as grayscale
        unsigned char* img = stbi_load(inputFilename.c_str(), &width, &height, &channels, 1);
        if (!img) {
            #pragma omp critical
            cerr << "Error: Could not load " << inputFilename << "\n";
            continue;
        }

        #pragma omp critical
        cout << "Image loaded: " << inputFilename << " - " << width << "x" << height << "\n";

        // Convert to 2D vector
        vector<vector<int>> image(height, vector<int>(width));
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j] = img[i * width + j];
            }
        }

        // ===================== PARALLEL NEGATIVE TRANSFORMATION =====================
        vector<unsigned char> negativeData(width * height);
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                negativeData[i * width + j] = 255 - image[i][j];
            }
        }
        string negativePath = "negative/" + baseName;
        stbi_write_png(negativePath.c_str(), width, height, 1, negativeData.data(), width);

        // ===================== PARALLEL SOBEL EDGE DETECTION =====================
        int Gx[3][3] = { {-1,0,1}, {-2,0,2}, {-1,0,1} };
        int Gy[3][3] = { {-1,-2,-1}, {0,0,0}, {1,2,1} };

        vector<unsigned char> edgesData(width * height, 0);
        #pragma omp parallel for collapse(2)
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

        stbi_image_free(img);

        #pragma omp critical
        {
            cout << "Processed " << inputFilename << ": Saved " << negativePath << " and " << edgePath << "\n";
            totalProcessed++;
        }
    }

    double end_time = omp_get_wtime();
    cout << "Batch processing complete! " << totalProcessed << " images processed.\n";
    cout << "Total completion time: " << (end_time - start_time) << " seconds\n";

    return 0;
}
