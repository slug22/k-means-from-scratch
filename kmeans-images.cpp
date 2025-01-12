#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <string>
#include <limits>
#include <random>

using namespace std;

// BMP file header structure - must be packed to match file format
#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t file_type{0x4D42};  // Always 'BM' for bitmap
    uint32_t file_size{0};       // Size of the file in bytes
    uint16_t reserved1{0};       // Reserved, must be 0
    uint16_t reserved2{0};       // Reserved, must be 0
    uint32_t offset_data{0};     // Start position of pixel data
};

struct BMPInfoHeader {
    uint32_t size{0};                    // Size of this header
    int32_t width{0};                    // Width in pixels
    int32_t height{0};                   // Height in pixels
    uint16_t planes{1};                  // Number of color planes
    uint16_t bit_count{0};               // Bits per pixel
    uint32_t compression{0};             // Compression type
    uint32_t size_image{0};              // Image size in bytes
    int32_t x_pixels_per_meter{0};       // Horizontal resolution
    int32_t y_pixels_per_meter{0};       // Vertical resolution
    uint32_t colors_used{0};             // Number of colors used
    uint32_t colors_important{0};        // Important colors
};
#pragma pack(pop)

// Pixel structure to hold position and color data
struct Pixel {
    float x, y;     // Normalized coordinates (0-1)
    float r, g, b;  // Normalized color values (0-1)
};

// Main image processing class
class ImageProcessor {
public:
void writeColorData(const vector<Pixel>& pixel_data, const string& filename) {
    ofstream file(filename);
    if (!file) {
        cerr << "Failed to create output file: " << filename << endl;
        return;
    }
    
    // Write x, y, color_sum, r, g, b for each pixel
    for (const auto& pixel : pixel_data) {
        float color_sum = pixel.r + pixel.g + pixel.b;
        file << pixel.x << " " << pixel.y << " " << color_sum << " "
             << pixel.r << " " << pixel.g << " " << pixel.b << "\n";
    }
}
 bool readBMP(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Cannot open file " << filename << endl;
        return false;
    }

    // Read file header
    file.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
    if (!file) {
        cerr << "Error: Failed to read file header" << endl;
        return false;
    }

    // Print file header information for debugging
    cout << "File type: 0x" << hex << file_header.file_type << dec << endl;
    cout << "File size: " << file_header.file_size << " bytes" << endl;
    cout << "Data offset: " << file_header.offset_data << " bytes" << endl;

    // Verify this is a BMP file
    if (file_header.file_type != 0x4D42) {  // "BM" in hex
        cerr << "Error: Not a valid BMP file (incorrect file type signature)" << endl;
        return false;
    }

    // Read info header
    file.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));
    if (!file) {
        cerr << "Error: Failed to read info header" << endl;
        return false;
    }

    // Print image information for debugging
    cout << "Image dimensions: " << info_header.width << "x" << info_header.height << endl;
    cout << "Bits per pixel: " << info_header.bit_count << endl;
    cout << "Compression: " << info_header.compression << endl;

    // Verify image format is supported
    if (info_header.bit_count != 24) {
        cerr << "Error: Only 24-bit BMP files are supported. This image is " 
             << info_header.bit_count << "-bit" << endl;
        return false;
    }

    if (info_header.compression != 0) {
        cerr << "Error: Compressed BMP files are not supported" << endl;
        return false;
    }


        // Calculate row padding
        row_padding = (4 - (info_header.width * 3) % 4) % 4;

        // Read pixel data
        pixels.resize(info_header.width * info_header.height);
        
        // Move to pixel data start
        file.seekg(file_header.offset_data, file.beg);

        // Read pixels (BMP stores them bottom-up)
        for (int y = 0; y < info_header.height; y++) {
            for (int x = 0; x < info_header.width; x++) {
                unsigned char color[3];
                file.read(reinterpret_cast<char*>(color), 3);

                int actual_y = info_header.height - 1 - y;
                size_t index = actual_y * info_header.width + x;

                // Store normalized values
                pixels[index].x = static_cast<float>(x) / info_header.width;
                pixels[index].y = static_cast<float>(actual_y) / info_header.height;
                pixels[index].b = static_cast<float>(color[0]) / 255.0f;
                pixels[index].g = static_cast<float>(color[1]) / 255.0f;
                pixels[index].r = static_cast<float>(color[2]) / 255.0f;
            }
            file.seekg(row_padding, ios::cur);  // Skip padding bytes
        }

        return true;
    }
bool writeBMP(const string& filename, const vector<Pixel>& output_pixels) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Failed to create output file: " << filename << endl;
        return false;
    }

    // Calculate file size and image size
    int padding_size = (4 - (info_header.width * 3) % 4) % 4;
    uint32_t image_size = ((3 * info_header.width + padding_size) * info_header.height);
    uint32_t file_size = image_size + sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    // Update headers with correct values
    file_header.file_size = file_size;
    file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    info_header.size = sizeof(BMPInfoHeader);
    info_header.size_image = image_size;
    info_header.bit_count = 24;  // Ensure we're using 24-bit color
    info_header.compression = 0;  // No compression

    // Write updated headers
    file.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    file.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    // Write pixel data with padding
    vector<unsigned char> padding(padding_size, 0);
    
    for (int y = info_header.height - 1; y >= 0; y--) {
        for (int x = 0; x < info_header.width; x++) {
            size_t index = y * info_header.width + x;
            unsigned char color[3];
            
            // Convert back to 0-255 range and ensure values are clamped
            color[0] = static_cast<unsigned char>(min(255.0f, max(0.0f, output_pixels[index].b * 255.0f)));
            color[1] = static_cast<unsigned char>(min(255.0f, max(0.0f, output_pixels[index].g * 255.0f)));
            color[2] = static_cast<unsigned char>(min(255.0f, max(0.0f, output_pixels[index].r * 255.0f)));
            
            file.write(reinterpret_cast<char*>(color), 3);
        }
        
        // Write padding bytes at the end of each row
        if (padding_size > 0) {
            file.write(reinterpret_cast<const char*>(padding.data()), padding_size);
        }
    }

    return file.good();  // Return whether all write operations succeeded
}
    // Helper function for 5D distance calculation
    float distance(const Pixel& p1, const Pixel& p2, float pos_weight, float color_weight) {
        float dx = p1.x - p2.x;
        float dy = p1.y - p2.y;
        float dr = p1.r - p2.r;
        float dg = p1.g - p2.g;
        float db = p1.b - p2.b;

        return sqrt(
            pos_weight * (dx*dx + dy*dy) + 
            color_weight * (dr*dr + dg*dg + db*db)
        );
    }

    // K-means clustering implementation
    void kMeansClustering(int k, float pos_weight = 1.0f, float color_weight = 2.0f) {
        // Initialize random centroids
        centroids.resize(k);
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, pixels.size() - 1);

        for (int i = 0; i < k; i++) {
            centroids[i] = pixels[dis(gen)];
        }

        // Initialize cluster assignments
        vector<int> clusters(pixels.size());
        bool changed = true;
        int iterations = 0;
        const int MAX_ITERATIONS = 100;

        while (changed && iterations < MAX_ITERATIONS) {
            changed = false;
            iterations++;

            // Assign points to nearest centroid
            vector<vector<Pixel>> new_centroids(k);
            vector<int> cluster_sizes(k, 0);

            for (size_t i = 0; i < pixels.size(); i++) {
                float min_dist = numeric_limits<float>::max();
                int best_cluster = 0;

                // Find closest centroid
                for (int j = 0; j < k; j++) {
                    float d = distance(pixels[i], centroids[j], pos_weight, color_weight);
                    if (d < min_dist) {
                        min_dist = d;
                        best_cluster = j;
                    }
                }

                // Assign to cluster
                if (clusters[i] != best_cluster) {
                    changed = true;
                    clusters[i] = best_cluster;
                }

                new_centroids[best_cluster].push_back(pixels[i]);
                cluster_sizes[best_cluster]++;
            }

            // Update centroids
            for (int i = 0; i < k; i++) {
                if (cluster_sizes[i] > 0) {
                    Pixel avg = {0, 0, 0, 0, 0};
                    for (const auto& p : new_centroids[i]) {
                        avg.x += p.x;
                        avg.y += p.y;
                        avg.r += p.r;
                        avg.g += p.g;
                        avg.b += p.b;
                    }
                    avg.x /= cluster_sizes[i];
                    avg.y /= cluster_sizes[i];
                    avg.r /= cluster_sizes[i];
                    avg.g /= cluster_sizes[i];
                    avg.b /= cluster_sizes[i];
                    centroids[i] = avg;
                }
            }

            cout << "Iteration " << iterations << " completed" << endl;
        }

        // create segmented image
        segmented_pixels = pixels;
        for (size_t i = 0; i < pixels.size(); i++) {
            int cluster = clusters[i];
            segmented_pixels[i].r = centroids[cluster].r;
            segmented_pixels[i].g = centroids[cluster].g;
            segmented_pixels[i].b = centroids[cluster].b;
        }
    }

    // Getters
    int getWidth() const { return info_header.width; }
    int getHeight() const { return info_header.height; }
    const vector<Pixel>& getSegmentedPixels() const { return segmented_pixels; }
    const vector<Pixel>& getOriginalPixels() const { return pixels; }
private:
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    vector<Pixel> pixels;
    vector<Pixel> centroids;
    vector<Pixel> segmented_pixels;
    int row_padding;
};
  
int main() {
    ImageProcessor processor;
    string input_filename = "low_res_image.bmp";
    string output_filename = "output.bmp";
    
    // Read input image
    if (!processor.readBMP(input_filename)) {
        cerr << "Failed to read input image" << endl;
        return 1;
    }

    cout << "Image loaded successfully" << endl;
    cout << "Dimensions: " << processor.getWidth() << "x" << processor.getHeight() << endl;

    // Write original color data
    processor.writeColorData(processor.getOriginalPixels(), "original_colors.txt");

    // Perform k-means clustering
    int k = 16;  // Number of clusters
    float pos_weight = 1.0f;    // Weight for spatial distance
    float color_weight = 4.0f;  // Weight for color distance
    
    cout << "Starting k-means clustering with k=" << k << endl;
    processor.kMeansClustering(k, pos_weight, color_weight);

    // Write segmented color data
    processor.writeColorData(processor.getSegmentedPixels(), "segmented_colors.txt");

    // Write output image
    if (!processor.writeBMP(output_filename, processor.getSegmentedPixels())) {
        cerr << "Failed to write output image" << endl;
        return 1;
    }

    cout << "Segmented image saved successfully to " << output_filename << endl;
    return 0;
}
