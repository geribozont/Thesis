#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <queue>
#include <unistd.h>    // getcwd()
#include <limits.h>    // PATH_MAX

using namespace std;

int pwl_softplus_18(int x){
    x += pow(2,17);
    int interval = (x >> 13);
    switch (interval){
        case 0: return ((x * 4) >> 13) + 5;
        case 1: return ((x * 6) >> 13) + 3;
        case 2: return ((x * 10) >> 13) + -5;
        case 3: return ((x * 16) >> 13) + -23;
        case 4: return ((x * 26) >> 13) + -63;
        case 5: return ((x * 43) >> 13) + -148;
        case 6: return ((x * 71) >> 13) + -316;
        case 7: return ((x * 116) >> 13) + -631;
        case 8: return ((x * 190) >> 13) + -1223;
        case 9: return ((x * 309) >> 13) + -2294;
        case 10: return ((x * 497) >> 13) + -4174;
        case 11: return ((x * 787) >> 13) + -7364;
        case 12: return ((x * 1220) >> 13) + -12560;
        case 13: return ((x * 1832) >> 13) + -20516;
        case 14: return ((x * 2635) >> 13) + -31758;
        case 15: return ((x * 3590) >> 13) + -46083;
        case 16: return ((x * 4602) >> 13) + -62275;
        case 17: return ((x * 5557) >> 13) + -78510;
        case 18: return ((x * 6360) >> 13) + -92964;
        case 19: return ((x * 6972) >> 13) + -104592;
        case 20: return ((x * 7405) >> 13) + -113252;
        case 21: return ((x * 7695) >> 13) + -119342;
        case 22: return ((x * 7883) >> 13) + -123478;
        case 23: return ((x * 8002) >> 13) + -126215;
        case 24: return ((x * 8076) >> 13) + -127991;
        case 25: return ((x * 8121) >> 13) + -129116;
        case 26: return ((x * 8149) >> 13) + -129844;
        case 27: return ((x * 8166) >> 13) + -130303;
        case 28: return ((x * 8176) >> 13) + -130583;
        case 29: return ((x * 8182) >> 13) + -130757;
        case 30: return ((x * 8186) >> 13) + -130877;
        case 31: return ((x * 8188) >> 13) + -130939;
        default: return 255;
    }    
}

int pwl_softplus_19(int x){
    x += pow(2,18);
    int interval = (x >> 14);
    cout << "interval " << interval << endl;
    switch (interval){
        case 0: return ((x * 0) >> 11) + 0;
        case 1: return ((x * 0) >> 11) + 0;
        case 2: return ((x * 0) >> 11) + 0;
        case 3: return ((x * 0) >> 11) + 0;
        case 4: return ((x * 0) >> 11) + 0;
        case 5: return ((x * 0) >> 11) + 0;
        case 6: return ((x * 0) >> 11) + 0;
        case 7: return ((x * 0) >> 11) + 0;
        case 8: return ((x * 0) >> 11) + 0;
        case 9: return ((x * 0) >> 11) + 0;
        case 10: return ((x * 0) >> 11) + 0;
        case 11: return ((x * 0) >> 11) + 0;
        case 12: return ((x * 0) >> 11) + 0;
        case 13: return ((x * 0) >> 11) + 0;
        case 14: return ((x * 0) >> 11) + -14;
        case 15: return ((x * 177) >> 11) + -21284;
        case 16: return ((x * 1871) >> 11) + -238020;
        case 17: return ((x * 2048) >> 11) + -262126;
        case 18: return ((x * 2048) >> 11) + -262144;
        case 19: return ((x * 2048) >> 11) + -262144;
        case 20: return ((x * 2048) >> 11) + -262144;
        case 21: return ((x * 2048) >> 11) + -262144;
        case 22: return ((x * 2048) >> 11) + -262144;
        case 23: return ((x * 2048) >> 11) + -262144;
        case 24: return ((x * 2048) >> 11) + -262144;
        case 25: return ((x * 2048) >> 11) + -262144;
        case 26: return ((x * 2048) >> 11) + -262144;
        case 27: return ((x * 2048) >> 11) + -262144;
        case 28: return ((x * 2048) >> 11) + -262144;
        case 29: return ((x * 2048) >> 11) + -262144;
        case 30: return ((x * 2048) >> 11) + -262144;
        case 31: return ((x * 2048) >> 11) + -262144;
        default: return 255;
    }    
}

int pwl_sigmoid(int x){
    x += pow(2,18);
    int interval = x >> 14;
    cout << "interval " << interval << endl;
    switch (interval){
        case 0: return ((x * 0) >> 14) + 0;
        case 1: return ((x * 0) >> 14) + 0;
        case 2: return ((x * 0) >> 14) + 0;
        case 3: return ((x * 0) >> 14) + 0;
        case 4: return ((x * 0) >> 14) + 0;
        case 5: return ((x * 0) >> 14) + 0;
        case 6: return ((x * 0) >> 14) + 0;
        case 7: return ((x * 0) >> 14) + 0;
        case 8: return ((x * 0) >> 14) + 0;
        case 9: return ((x * 0) >> 14) + 0;
        case 10: return ((x * 0) >> 14) + 0;
        case 11: return ((x * 0) >> 14) + 0;
        case 12: return ((x * 0) >> 14) + 0;
        case 13: return ((x * 0) >> 14) + 0;
        case 14: return ((x * 0) >> 14) + 0;
        case 15: return ((x * 512) >> 14) + -7680;
        case 16: return ((x * 512) >> 14) + -7680;
        case 17: return ((x * 0) >> 14) + 1024;
        case 18: return ((x * 0) >> 14) + 1024;
        case 19: return ((x * 0) >> 14) + 1024;
        case 20: return ((x * 0) >> 14) + 1024;
        case 21: return ((x * 0) >> 14) + 1024;
        case 22: return ((x * 0) >> 14) + 1024;
        case 23: return ((x * 0) >> 14) + 1024;
        case 24: return ((x * 0) >> 14) + 1024;
        case 25: return ((x * 0) >> 14) + 1024;
        case 26: return ((x * 0) >> 14) + 1024;
        case 27: return ((x * 0) >> 14) + 1024;
        case 28: return ((x * 0) >> 14) + 1024;
        case 29: return ((x * 0) >> 14) + 1024;
        case 30: return ((x * 0) >> 14) + 1024;
        case 31: return ((x * 0) >> 14) + 1024;
        default: return 255;
    }    
}

queue<int> read_file(string filename){


    queue<int> numbers;
    
    ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Hiba: Nem sikerult megnyitni a fajlt!" << std::endl;
    }
    
    string line;
    while (getline(file, line)) {
        try {
            int number = stoi(line);
            numbers.push(number);
        }
        catch (const std::exception& e) {
            std::cerr << "Hiba a sor feldolgozasakor: " << line << std::endl;
        }
    }
    
    file.close();
    
    // Ellenőrzés: hány elemet olvastunk be
    std::cout << "Beolvasott elemek szama: " << numbers.size() << std::endl;
    return numbers;
}

int main(){

    queue<int> weights = read_file("brevitas_conf_params_weight.txt");
    queue<int> biases = read_file("brevitas_conf_params_bias.txt");
    queue<int> weight_scales = read_file("brevitas_conf_params_weight_scale.txt");
    queue<int> bias_scales = read_file("brevitas_conf_params_bias_scale.txt");

    // data open

    string line;
    ifstream infile("val_dataset_X.txt");

    vector<vector<int>> inputs;
    while (getline(infile, line)) {
        vector<int> input_vec;
        size_t pos = 0;
        while ((pos = line.find(',')) != string::npos) {
            input_vec.push_back(stoi(line.substr(0, pos)));
            line.erase(0, pos + 1);
        }
        input_vec.push_back(stoi(line));
        inputs.push_back(input_vec);
    }
    infile.close();

    ifstream labelfile("val_dataset_y.txt");
    if (!labelfile.is_open()) {
        cerr << "Hiba a fájl megnyitásakor!" << endl;
        return 1;
    }
    vector<int> labels;
    while (getline(labelfile, line)) {
        labels.push_back(stoi(line));
    }
    labelfile.close();


    vector<vector<int>> input_test;
    for (size_t i = 0; i < 1; ++i) {
        input_test.push_back(inputs[i]);
    }

    vector<int> layer1out;
    vector<int> layer2out;
    for (vector<int> inp : input_test) {
        vector<int> layer1out;
        vector<int> msb;
        // debug
        cout << "inputs:" << endl;
        for(int i : inp) cout << i << ' ';
        cout << endl;
        int max_msb = 0;

        for (int i = 0; i < 4; i++) {
            int acc = 0;
            for (int j = 0; j < 3; j++) {
                // cout << inp[j] << ' ';
                int w = weights.front(); weights.pop();
                acc += w * inp[j]; 
                weights.push(w);
                //cout << w << " = " << inp[j] << '*' << w << " + ";
            }
            //cout << endl;
            //cout << "Before acc & bias: " << acc << ' ' << endl;
            int bias = biases.front(); biases.pop(); biases.push(bias);
            bias = (bias << (weight_scales.front() + 8 - bias_scales.front())); // 8 a weight scale miatt
            acc += bias;
            //cout << "After bias, before acc: " << acc << ' ' << endl;
            acc = pwl_softplus_18(acc);
            layer1out.push_back(acc);
            cout << acc << endl;
            //cout << "Afret acc & bias: "<< acc << ' ' << endl;
            msb.push_back(log2(abs(acc)));
            //cout << "MSB: " << msb.back() << endl;
            if (log2(abs(acc)) > max_msb) max_msb = log2(abs(acc));
            cout << "Max msb:" << max_msb << endl;
        }
        int ws = weight_scales.front(); weight_scales.pop(); weight_scales.push(ws);
        int bs = bias_scales.front(); bias_scales.pop(); bias_scales.push(bs);
        //cout << endl;

        // scaling faktor in - offset; offset = MAX_MSB - 7 (maxmsb + 1 - mert 0tól indexel, 7 mert elöjelbit az első 8 biten)
        int scf_out = weight_scales.front() + 8 - (max_msb + 1 - 7);
        cout << "out scaling faktor: " << scf_out << endl;
        for (int i = 0; i < 4; i++) { 
            layer1out[i] = layer1out[i] >> ( max_msb + 1 - 7);
            //cout << layer1out[i] << ' ';
        }
        cout << endl;

        // layer 2
        
        max_msb = 0;
        for (int i = 0; i < 4; i++) {
            int acc = 0;
            for (int j = 0; j < 4; j++) {
                // cout << inp[j] << ' ';
                int w = weights.front(); weights.pop();
                cout <<  layer1out[j] << endl;
                acc += w * layer1out[j]; 
                weights.push(w);
                //cout << w << " = " << inp[j] << '*' << w << " + ";
            }
            //cout << endl;
            cout << "Before acc & bias: " << acc << ' ' << endl;
            int bias = biases.front(); biases.pop(); biases.push(bias);
            bias = (bias << (weight_scales.front() + scf_out - bias_scales.front()));
            acc += bias;
            cout << "After bias, before acc: " << acc << ' ' << endl;
            acc = pwl_softplus_19(acc);
            layer2out.push_back(acc);
            cout << "Afret acc & bias: "<< acc << ' ' << endl;
            msb.push_back(log2(acc));
            cout << "MSB: " << msb.back() << endl;
            if (log2(abs(acc)) > max_msb) max_msb = log2(abs(acc));
            cout << "Max msb:" << max_msb << endl;
        }
        cout << endl;
        scf_out = weight_scales.front() + scf_out - (max_msb + 1 - 7);
        cout << "out scaling faktor: " << scf_out << endl;
        cout << "balshift: " << max_msb + 1 - 7 << endl;
        for (int i = 0; i < 4; i++) { 
            layer2out[i] = layer2out[i] >> ( max_msb + 1 - 7);
            cout << layer2out[i] << ' ';
        }
        cout << endl;

        // layer 3

        cout << "output layer" << endl;

        max_msb = 0;
        for (int i = 0; i < 1; i++) {
            int acc = 0;
            for (int j = 0; j < 4; j++) {
                // cout << inp[j] << ' ';
                int w = weights.front(); weights.pop();
                //cout <<  layer2out[j] << endl;
                acc += w * layer2out[j]; 
                weights.push(w);
                //cout << w << " = " << inp[j] << '*' << w << " + ";
            }
            //cout << endl;
            cout << "Before acc & bias: " << acc << ' ' << endl;
            int bias = biases.front(); biases.pop(); biases.push(bias);
            bias = (bias << (weight_scales.front() + scf_out - bias_scales.front()));
            acc += bias;
            cout << "After bias, before acc: " << acc << ' ' << endl;
            acc = pwl_sigmoid(acc);
            layer2out.push_back(acc);
            cout << "Afret acc & bias: "<< acc << ' ' << endl;
        }
    }
    
    cout  << "FINITO" << endl;
    return 0;
}