#include "neural_network.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

struct RGBPixel {
    int r, g, b;
    
    RGBPixel() : r(0), g(0), b(0) {}
    RGBPixel(int _r, int _g, int _b) : r(_r), g(_g), b(_b) {}
};

std::vector<RGBPixel> read_rgb_file(const std::string& filename) {
    std::vector<RGBPixel> pixels;
    std::ifstream fin(filename);
    
    if(!fin.is_open()) {
        std::cerr << "Error: Cannot open file '" << filename << "'!" << std::endl;
        return pixels;
    }
    
    std::string line;
    while(std::getline(fin, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<int> values;
        
        while(std::getline(ss, token, ',')) {
            size_t start = token.find_first_not_of(" \t\r\n");
            size_t end = token.find_last_not_of(" \t\r\n");
            
            if (start != std::string::npos && end != std::string::npos) {
                token = token.substr(start, end - start + 1);
                
                try {
                    int val = std::stoi(token);
                    values.push_back(val);
                    
                    if (values.size() == 3) {
                        pixels.push_back(RGBPixel(values[0], values[1], values[2]));
                        values.clear();
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Invalid value: '" << token << "'" << std::endl;
                }
            }
        }
        
    }
    
    fin.close();
    return pixels;
}

void write_results(const std::string& filename, const std::vector<bool>& results) {
    std::ofstream fout(filename);
    
    
    for(size_t i = 0; i < results.size(); i++) {
        fout << (int)results[i] << std::endl;
    }
    
    fout.close();
    std::cout << "Results saved: " << filename << std::endl;
}

int main() {
    std::cout << "Neural Network Testbench - Forward" << std::endl;
    std::cout << std::endl;
    
    std::string input_file = "val_dataset_X.txt";
    std::string label_file = "val_dataset_y.txt";
    
    std::cout << "Reading input file: " << input_file << std::endl;
    std::vector<RGBPixel> input_pixels = read_rgb_file(input_file);

    if(input_pixels.empty()) {
        std::cerr << "No data! Run the dataset generator first!" << std::endl;
        return 1;
    }

    std::cout << "Loaded: " << input_pixels.size() << " RGB pixels" << std::endl;

    std::cout << "Reading label file: " << label_file << std::endl;
    std::vector<bool> labels;

    {
        std::ifstream lf(label_file);
        if(!lf.is_open()) {
            std::cerr << "Label file not found!" << std::endl;
            return 1;
        }

        int v;
        while(lf >> v) {
            labels.push_back(v != 0);
        }
    }

    if(labels.size() != input_pixels.size()) {
        std::cerr << "ERROR: X and Y sizes do not match!" << std::endl;
        return 1;
    }

    std::cout << "Label count: " << labels.size() << std::endl << std::endl;

    hls::stream<rgb_t> input_stream("input_stream");
    hls::stream<bool> output_stream("output_stream_tb");
    std::vector<bool> results;
    
    int processed_count = 0;

    int pixelsize = input_pixels.size();

    for(size_t i = 0; i < 10000; i++) {
        rgb_t pixel_in;
        pixel_in.range(23, 16) = input_pixels[i].r;
        pixel_in.range(15, 8)  = input_pixels[i].g;
        pixel_in.range(7, 0)   = input_pixels[i].b;
        
        input_stream.write(pixel_in);

        neural_network(input_stream, output_stream);

        bool result = output_stream.read();
        results.push_back(result);         
        
        processed_count++;

        if(processed_count % 100 == 0 || processed_count == input_pixels.size()) {
            std::cout << "  Processed: " << processed_count << "/" 
                     << input_pixels.size() << " pixel" << std::endl;
        }
    }
    /*
    neural_network(input_stream, output_stream);

    for(size_t i = 0; i < 100; i++) {
        bool result = output_stream.read();
        results.push_back(result);      
    }
    */

    std::cout << "Processing complete!" << std::endl << std::endl;

    int TP = 0, TN = 0, FP = 0, FN = 0;
    std::vector<int> wrong_indices;

    for(size_t i = 0; i < results.size(); i++) {
        int pred = results[i];
        int true_label = labels[i];
        
        if(pred == 1 && true_label == 1) {
            TP++;
        } else if(pred == 0 && true_label == 0) {
            TN++;
        } else if(pred == 1 && true_label == 0) {
            FP++;
            wrong_indices.push_back(i);
        } else if(pred == 0 && true_label == 1) {
            FN++;
            wrong_indices.push_back(i);
        }
        
    }

    int total = results.size();

    double accuracy = (total > 0) ? (double)(TP + TN) / total : 0.0;
    double precision = (TP + FP > 0) ? (double)TP / (TP + FP) : 0.0;
    double recall = (TP + FN > 0) ? (double)TP / (TP + FN) : 0.0;
    double f1_score = (precision + recall > 0) ? 2.0 * (precision * recall) / (precision + recall) : 0.0;
    double specificity = (TN + FP > 0) ? (double)TN / (TN + FP) : 0.0;

    std::cout << "Confusion matrix and metrics" << std::endl;
    std::cout << "TP=" << TP << ", TN=" << TN << ", FP=" << FP << ", FN=" << FN << std::endl;
    std::cout << std::fixed << std::setprecision(8);
    std::cout << "Accuracy: " << accuracy << std::endl;
    std::cout << "Precision: " << precision << std::endl;
    std::cout << "Recall: " << recall << std::endl;
    std::cout << "F1-Score: " << f1_score << std::endl;
    std::cout << "Specificity: " << specificity << std::endl;

    if(!wrong_indices.empty()) {
        std::cout << "\nWarning: Number of incorrect predictions: " << wrong_indices.size() 
                << " (" << (double)wrong_indices.size()/total*100.0 << "%)" << std::endl;
        std::cout << "   First 10 incorrect examples: ";
        for(int i = 0; i < std::min(10, (int)wrong_indices.size()); i++) {
            int idx = wrong_indices[i];
            std::cout << idx << "(pred=" << results[idx] 
                    << ", true=" << labels[idx] << ") ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "\nAll predictions are correct!" << std::endl;
    }
    
    write_results("results_simple.txt", results);

    std::cout << "TB ok" << std::endl;
    return 0;
}
