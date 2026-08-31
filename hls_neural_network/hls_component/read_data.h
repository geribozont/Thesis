#ifndef READ_DATA_H
#define READ_DATA_H

#include "hls_stream.h"
#include "ap_int.h"
#include "ap_fixed.h"

#define LAYER1_IN 3
#define LAYER1_OUT 4
#define LAYER2_IN 4
#define LAYER2_OUT 4
#define LAYER3_IN 4
#define LAYER3_OUT 1

#define TOTAL_WEIGHTS 32
#define TOTAL_BIASES 9
#define TOTAL_WEIGHT_SCALES 3
#define TOTAL_BIAS_SCALES 3

#define INPUT_SCALE 8
#define BIT_WIDTH 20

#define NUM_OF_PIXELS 100

typedef ap_uint<24> rgb_t;
typedef ap_uint<8> datain_t;
typedef ap_int<8> data_t;
typedef ap_int<13> bias_t;
typedef ap_int<5> scale_t;
typedef ap_int<BIT_WIDTH> acc_t;
typedef ap_int<19> act_t;
typedef ap_uint<5> bitw_t;
typedef ap_fixed<16, 6> fx16;

typedef ap_int<8> data_t;
typedef struct {
    data_t data[4];
} scaled_data4_t;

static const data_t WEIGHTS[TOTAL_WEIGHTS] = {
    #include "brevitas_conf_params_weight.txt"
};

static const data_t WEIGHTS_L1[LAYER1_OUT * LAYER1_IN] = {
    WEIGHTS[0],  WEIGHTS[1],  WEIGHTS[2],
    WEIGHTS[3],  WEIGHTS[4],  WEIGHTS[5],
    WEIGHTS[6],  WEIGHTS[7],  WEIGHTS[8],
    WEIGHTS[9],  WEIGHTS[10], WEIGHTS[11]
};

static const data_t WEIGHTS_L2[LAYER2_OUT * LAYER2_IN] = {
    WEIGHTS[12], WEIGHTS[13], WEIGHTS[14], WEIGHTS[15],
    WEIGHTS[16], WEIGHTS[17], WEIGHTS[18], WEIGHTS[19],
    WEIGHTS[20], WEIGHTS[21], WEIGHTS[22], WEIGHTS[23],
    WEIGHTS[24], WEIGHTS[25], WEIGHTS[26], WEIGHTS[27]
};

static const data_t WEIGHTS_L3[LAYER3_OUT * LAYER3_IN] = {
    WEIGHTS[28], WEIGHTS[29], WEIGHTS[30], WEIGHTS[31]
};


static const bias_t BIASES[TOTAL_BIASES] = {
    #include "brevitas_conf_params_bias.txt"
};

static const scale_t WEIGHT_SCALES[TOTAL_WEIGHT_SCALES] = {
    #include "brevitas_conf_params_weight_scale.txt"
};

static const scale_t BIAS_SCALES[TOTAL_BIAS_SCALES] = {
    #include "brevitas_conf_params_bias_scale.txt"
};

#endif