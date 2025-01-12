// kmeans.cpp
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
using namespace std;

float dist(float dx, float dy, float dz) {
    return sqrt((dx*dx) + (dy*dy) + (dz*dz));
}

void kmeans(const vector<vector<float>>& init_data, vector<vector<float>>& centroids, vector<int>& buckets, int count) {
    count++;
    vector<int> freakyclusterct(centroids.size(), 0);
    vector<vector<float>> new_centroids(centroids.size(), vector<float>(centroids[0].size(), 0.0));
    
    for (int i = 0; i < init_data.size(); i++) {
        float min_dist = numeric_limits<float>::max();
        int best_cluster = 0;
        
        for (int j = 0; j < centroids.size(); j++) {
            float d = dist(
                abs(centroids[j][0] - init_data[i][0]),
                abs(centroids[j][1] - init_data[i][1]),
                abs(centroids[j][2] - init_data[i][2])
            );
            if (d < min_dist) {
                min_dist = d;
                best_cluster = j;
            }
        }
        
        buckets[i] = best_cluster;
        freakyclusterct[best_cluster]++;
        
        for (int k = 0; k < init_data[i].size(); k++) {
            new_centroids[best_cluster][k] += init_data[i][k];
        }
    }
    
    bool changed = false;
    for (int i = 0; i < centroids.size(); i++) {
        if (freakyclusterct[i] > 0) {
            for (int j = 0; j < centroids[i].size(); j++) {
                float new_pos = new_centroids[i][j] / freakyclusterct[i];
                if (abs(centroids[i][j] - new_pos) > 0.0001) {
                    changed = true;
                }
                centroids[i][j] = new_pos;
            }
        }
    }
    
    if (changed && count < 1000) {
        kmeans(init_data, centroids, buckets, count);
    }
}

vector<vector<float>> make_me_nums(int howmany) {
    srand(time(0));
    vector<vector<float>> boys = {};
    for (int i=0; i< howmany; i++) {
        vector<float> hellyeah;
        float x = ((float)rand() / RAND_MAX) * 10 - 5;
        float y = ((float)rand() / RAND_MAX) * 10 - 5;
        float z = ((float)rand() / RAND_MAX) * 10 - 5;
        hellyeah.push_back(x);
        hellyeah.push_back(y);
        hellyeah.push_back(z);
        boys.push_back(hellyeah);
    }
    return boys;
}

int main() {
    int k = 3;
    vector<vector<float>> init_data = make_me_nums(10000);
    vector<vector<float>> centroids = vector<vector<float>>(init_data.end() - k, init_data.end());
    vector<int> buckets(init_data.size(), 0);
    
    kmeans(init_data, centroids, buckets, 0);
    
    // Write data points and their clusters to file
    ofstream outfile("cluster_data.txt");
    for (int i = 0; i < init_data.size(); i++) {
        outfile << init_data[i][0] << " " << init_data[i][1] << " " 
               << init_data[i][2] << " " << buckets[i] << "\n";
    }
    outfile.close();
    
    // Write centroids to a separate file
    ofstream centroid_file("centroids.txt");
    for (const auto& centroid : centroids) {
        centroid_file << centroid[0] << " " << centroid[1] << " " << centroid[2] << "\n";
    }
    centroid_file.close();
    
    cout << "Data written to cluster_data.txt and centroids.txt" << endl;
    return 0;
}
