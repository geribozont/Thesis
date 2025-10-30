#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <queue>

using namespace std;

int act(int x, vector<pair<int, int>> act_vector, int scale, int bit_width){


    //cout << endl;
    //cout << "Acc" << endl;
    //cout << "x: " << x << endl;
    long long int k = x + pow(2,bit_width-1);
    cout << "ez egy ká: " << k << endl;
    x += pow(2,bit_width-1);
    cout << "x: " << x << " scale " << scale << " bit width " << bit_width << endl;
    cout << x << ' ' << pow(2,bit_width-1) - pow(2,scale+4-1) << ' ' << pow(2,bit_width-1) + pow(2,scale+4-1) << endl;
    
    int lower = (1 << (bit_width - 1)) - (1 << (scale + 3));
    int upper = (1 << (bit_width - 1)) + (1 << (scale + 3));
    
    if (x < lower){
        cout << "min" << endl;
        return 0;
    } else 
    if (x > upper){
        cout << "max" << endl;
        return pow(2,scale);
    }

    int delta = 14 - scale;
    int interval = x >> (14 - 1 - delta);
    //interval = max(0, min((int)act_vector.size() - 1, interval));

    cout << "Interval: " << interval << endl;
    //cout << "log2x: " << log2(x) << endl;
    cout << "res1 szorzas shift előtt: " << x << endl;
    long long int res1 = x;
    //cout << "res1 szorzas elott shift utan: " << res1 << " amivel szorzunk " << act_vector[interval].first << endl;
    res1 *= act_vector[interval].first;
    //cout << "res1 szorzas utan shift elott: " << res1 << endl;
    res1 = res1 >> 14;
    //cout << "res1 összeadás előtt: " << res1 << endl;
    //cout << act_vector[interval].second << endl;
    //cout << endl;
    // 51491
    return res1 + (act_vector[interval].second >> (delta));
}

int max_szam = 0;

int act2(int x, vector<pair<int, int>> act_vector, int scale, int bit_width){

    int default_scale = 14;

    long long xx = x;
    // terv: 
    // elágazás ha
    // x < kritikus szakasz alsó határa  -> minimum
    // x > kritikus szakasz felső határa -> maximum
    // egyébként tolunk és kizárólag a scaling faktorral dolgozunk

    if (xx < -(1 << (scale + 3))){
        return 0;
    } else 
    if (xx > (1 << (scale + 3))){
        return 999999;
    }
    cout << "x: " << xx << endl;
    xx += pow(2, scale+3);
    cout << "scale: " << scale << " bit width: " << bit_width << endl;
    cout << "x: " << xx << endl;
    int delta = default_scale - scale;
    int interval = xx >> default_scale - delta - 1;
    cout << "interval: " << interval << " delta " << delta << endl;
    xx = xx >> 8 - delta;
    
    xx = xx * act_vector[interval].first;

    if (xx > max_szam) max_szam = xx;

    cout << "szorzas utan: " << xx << endl;
    xx = xx >> 6 + delta;
    cout << "shift után:" << xx << endl;
    cout << "amit kivonunk: " << (act_vector[interval].second >> delta) << endl;
    xx += act_vector[interval].second >> delta;
    cout << "eredmeny: " << xx << endl;

    return xx;
}


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
    
    // Ellenőrzés: hány elemet olvastunk be
    std::cout << "Beolvasott elemek szama: " << numbers.size() << std::endl;
    return numbers;
}

int main(){

    queue<int> weights = read_file("brevitas_conf_params_weight.txt");
    queue<int> biases = read_file("brevitas_conf_params_bias.txt");
    queue<int> weight_scales = read_file("brevitas_conf_params_weight_scale.txt");
    queue<int> bias_scales = read_file("brevitas_conf_params_bias_scale.txt");
    vector<pair<int,int>> sigmoid_default = readAct("segments_4_my_sigmoid.txt");
    vector<pair<int,int>> softplus_default = readAct("segments_4_my_softplus.txt");
    vector<pair<int,int>> tanh_default = readAct("segments_4_my_tanh.txt");

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
    for (size_t i = 0; i < 1000; ++i) {
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
        
        int bit_width = 18;

        for (int i = 0; i < 4; i++) {
            int acc = 0;
            for (int j = 0; j < 3; j++) {
                // cout << inp[j] << ' ';
                int w = weights.front(); weights.pop();
                acc += w * inp[j]; 
                weights.push(w);
                cout << acc << " +=" << w << " * " << inp[j] << endl;
            }
            cout << endl;
            cout << "Before acc & bias: " << acc << ' ' << endl;
            int bias = biases.front(); biases.pop(); biases.push(bias);
            bias = (bias << (weight_scales.front() + 8 - bias_scales.front())); // 8 a weight scale miatt
            acc += bias;
            cout << "After bias, before acc: " << acc << ' ' << endl;
            acc = act2(acc, softplus_default, weight_scales.front() + 8, bit_width);
            layer1out.push_back(acc);
            cout << "Afret acc & bias: "<< acc << ' ' << endl;
            msb.push_back(log2(abs(acc)));
            cout << "MSB: " << msb.back() << endl;
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

        cout << "#################################################################################################################################################" << endl;
        cout << endl;
        max_msb = 0;
        bit_width = 19;

        for (int i = 0; i < 4; i++) {
            int acc = 0;
            for (int j = 0; j < 4; j++) {
                // cout << inp[j] << ' ';
                int w = weights.front(); weights.pop();
                cout << "layer1out " << layer1out[j] << " weight " << w << endl;
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
            acc = act2(acc, softplus_default, weight_scales.front() + scf_out, bit_width);
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

        cout << "#################################################################################################################################################" << endl;
        cout << endl;

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
            acc = act(acc, sigmoid_default, weight_scales.front() + scf_out, bit_width);
            layer2out.push_back(acc);
            cout << "Afret acc & bias: "<< acc << ' ' << endl;
        }
    }
    
    cout  << "FINITO, de a max szam: " << max_szam << endl;
    return 0;
}