#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <queue>

using namespace std;

vector<pair<int, int>> readAct(const string& filename) {
    vector<pair<int, int>> data;
    ifstream file(filename);
    string line;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string a_str, b_str;
        
        getline(ss, a_str, ',');
        getline(ss, b_str);
        
        int a = stoi(a_str);
        int b = stoi(b_str);
        
        data.push_back({a, b});
    }
    
    return data;
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
    
    return numbers;
}

int act_softplus(int x, int scale, int bit_width){

    int default_scale = 14;
    vector<pair<int,int>> act_vector = readAct("segments_4_my_softplus.txt");

    // megnézem hogy az "érdekes" tartományban van-e a szám 
    if (x < -(1 << (scale + 3)))     return 0;
    else if (x > (1 << (scale + 3))) return x;

    // behúzom jobbra, hogy csak a pozitív tengelyen legyen értelmetve
    x += pow(2, scale+3);

    // szükséges paraméterek kiszámolása, interval haározza meg, hogy az interpoláció 
    // melyik szegmensét használjuk
    int delta = default_scale - scale;
    int interval = x >> default_scale - delta - 1;
    
    // előzetes bitshift, hogy a szorzást eredménye kisebb bitszélességű legyen, majd a maradék shift
    x = x >> (8 - delta);
    x = x * act_vector[interval].first;
    x = x >> (6 + delta);

    // + b
    x += act_vector[interval].second >> delta;

    return x;
}

int act_sigmoid(int x, int scale, int bit_width){

    int default_scale = 14;
    vector<pair<int,int>> act_vector = readAct("segments_4_my_sigmoid.txt");

    // megnézem hogy az "érdekes" tartományban van-e a szám 
    if (x < -(1 << (scale + 3)))     return 0;
    else if (x > (1 << (scale + 3))) return (1 << scale);

    x += pow(2, scale+3);

    int delta = default_scale - scale;
    int interval = x >> default_scale - delta - 1;

    x = x >> (0 - delta);
    x = x * act_vector[interval].first;
    x = x >> (14 + delta);

    x += act_vector[interval].second >> delta;

    return x;
}


int main(){

    // data open
    queue<int> weights = read_file("brevitas_conf_params_weight.txt");
    queue<int> biases = read_file("brevitas_conf_params_bias.txt");
    queue<int> weight_scales = read_file("brevitas_conf_params_weight_scale.txt");
    queue<int> bias_scales = read_file("brevitas_conf_params_bias_scale.txt");
    //vector<pair<int,int>> tanh_default = readAct("segments_4_my_tanh.txt");

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

    // init test data
    vector<vector<int>> input_test;
    for (size_t i = 0; i <1000; i++) {
        input_test.push_back(inputs[i]);
    }

    // init miss counters
    int miss = 0;
    vector<int> misses;
    int counter = 0;

    // Fő ciklus, az egész forward passra
    for (vector<int> inp : input_test) {

        // init outputs
        vector<int> layer1out;
        vector<int> layer2out;
        vector<int> layer3out;

        // first layer

        int bit_width = 18;
        int max_msb = 0;

        for (int i = 0; i < 4; i++) {
            int acc = 0;
            for (int j = 0; j < 3; j++) {
                int w = weights.front(); weights.pop(); weights.push(w);
                acc += w * inp[j];
            }
            int bias = biases.front(); biases.pop(); biases.push(bias);
            bias = (bias << (weight_scales.front() + 8 - bias_scales.front())); // 8 a weight scale miatt
            acc += bias;
            acc = act_softplus(acc, weight_scales.front() + 8, bit_width);
            layer1out.push_back(acc);
            if (log2(abs(acc)) > max_msb) max_msb = log2(abs(acc));
        }

        int scf_out = weight_scales.front() + 8 - (max_msb + 1 - 7);
        for (int i = 0; i < 4; i++) { 
            layer1out[i] = layer1out[i] >> ( max_msb + 1 - 7);
        }
        
        // a súly es bias scaling faktor kezelése
        int ws = weight_scales.front(); weight_scales.pop(); weight_scales.push(ws);
        int bs = bias_scales.front(); bias_scales.pop(); bias_scales.push(bs);

        // second layer

        max_msb = 0;
        bit_width = 19;

        for (int i = 0; i < 4; i++) {
            int acc = 0;
            for (int j = 0; j < 4; j++) {
                int w = weights.front(); weights.pop(); weights.push(w);
                acc += w * layer1out[j]; 
            }
            int bias = biases.front(); biases.pop(); biases.push(bias);
            bias = (bias << (weight_scales.front() + scf_out - bias_scales.front()));
            acc += bias;
            acc = act_softplus(acc, weight_scales.front() + scf_out, bit_width);
            layer2out.push_back(acc);
            if (log2(abs(acc)) > max_msb) max_msb = log2(abs(acc));
        }

        scf_out = weight_scales.front() + scf_out - (max_msb + 1 - 7);
        for (int i = 0; i < 4; i++) { 
            layer2out[i] = layer2out[i] >> ( max_msb + 1 - 7);
        }

        ws = weight_scales.front(); weight_scales.pop(); weight_scales.push(ws);
        bs = bias_scales.front(); bias_scales.pop(); bias_scales.push(bs);

        // layer 3

        max_msb = 0;

        for (int i = 0; i < 1; i++) {
            int acc = 0;
            for (int j = 0; j < 4; j++) {
                int w = weights.front(); weights.pop(); weights.push(w);
                acc += w * layer2out[j]; 
            }
            int bias = biases.front(); biases.pop(); biases.push(bias);
            bias = (bias << (weight_scales.front() + scf_out - bias_scales.front()));
            acc += bias;
            acc = act_sigmoid(acc, weight_scales.front() + scf_out, bit_width);
            layer3out.push_back(acc);
            if (log2(abs(acc)) > max_msb) max_msb = log2(abs(acc));
        }

        // Döntés
        if (layer3out[0] >= (1 << (weight_scales.front() + scf_out - 1))) layer3out[0] = 1;
        else layer3out[0] = 0;

        ws = weight_scales.front(); weight_scales.pop(); weight_scales.push(ws);
        bs = bias_scales.front(); bias_scales.pop(); bias_scales.push(bs);

        // Hibaszámoló
        if (layer3out[0] != labels[counter]) {
            miss++;
            misses.push_back(counter);
        }
        counter ++;
    }
    
    cout  << "Hibák száma: " << miss << endl;
    if (!misses.empty()){
        for (int i=0; i<misses.size(); i++){
            cout << misses[i] << endl;
        }
    }

    return 0;
}