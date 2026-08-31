#include "ap_int.h"
#include "hls_stream.h"
#include <iostream>
#include "neural_network.h"
#include "read_data.h"
#include "activations.h"

/**
 * Finds the position of the most significant bit (MSB).
 * On FPGA, __builtin_clz (Count Leading Zeros) maps to a hardware priority encoder.
 */

inline int get_msb_position(acc_t value) {
    #pragma HLS INLINE
    
    acc_t abs_val = (value < 0) ? (acc_t) (-value) : value;
    int lzc = __builtin_clz((unsigned int)abs_val);
    const int WIDTH = 32;
    int msb = (WIDTH - 1) - lzc;
    return msb;
}


/**
 * Splits the incoming 24-bit RGB pixel into separate color channels (R, G, B).
 * Uses an AXI-Stream interface for continuous data processing.
 */

void rgb_unpacker(
    hls::stream<rgb_t> &in_stream,
    hls::stream<datain_t> &r_stream,
    hls::stream<datain_t> &g_stream,
    hls::stream<datain_t> &b_stream
) {
    #pragma HLS INTERFACE axis port=in_stream
    #pragma HLS INTERFACE axis port=r_stream
    #pragma HLS INTERFACE axis port=g_stream
    #pragma HLS INTERFACE axis port=b_stream
        
    #pragma HLS PIPELINE II=1
    
        rgb_t pixel_in = in_stream.read();

        datain_t r = pixel_in.range(23, 16);
        datain_t g = pixel_in.range(15, 8);
        datain_t b = pixel_in.range(7, 0);

        r_stream.write(r);
        g_stream.write(g);
        b_stream.write(b);
}

/**
 * First hidden layer: 3 inputs (RGB) -> 4 output neurons.
 * Uses fixed-point arithmetic and scaled bias shifting.
 */

void layer1_matvec(
    hls::stream<datain_t> &r_stream,
    hls::stream<datain_t> &g_stream,
    hls::stream<datain_t> &b_stream,
    hls::stream<scaled_data4_t> &layer1_out
) {
#pragma HLS INLINE off
#pragma HLS INTERFACE axis port=r_stream
#pragma HLS INTERFACE axis port=g_stream
#pragma HLS INTERFACE axis port=b_stream
#pragma HLS INTERFACE axis port=layer1_out
#pragma HLS PIPELINE II=1

    // Compute scaling factors for fixed-point representation
    const scale_t weight_scale = WEIGHT_SCALES[0];
    const scale_t bias_scale   = BIAS_SCALES[0];
    const ap_uint<4> shift_amount = weight_scale + INPUT_SCALE - bias_scale;

    datain_t rgb_in[3];
#pragma HLS ARRAY_PARTITION variable=rgb_in complete

    rgb_in[0] = r_stream.read();
    rgb_in[1] = g_stream.read();
    rgb_in[2] = b_stream.read();

    acc_t acc[4] = {0, 0, 0, 0};
#pragma HLS ARRAY_PARTITION variable=acc complete

    // Fully unrolled
    for (int i = 0; i < LAYER1_OUT; i++) {
#pragma HLS UNROLL
        for (int j = 0; j < LAYER1_IN; j++) {
#pragma HLS UNROLL
            acc[i] += rgb_in[j] * WEIGHTS_L1[i * LAYER1_IN + j];
        }
    }

    scaled_data4_t out_val;
#pragma HLS ARRAY_PARTITION variable=out_val.data complete

    for (int i = 0; i < LAYER1_OUT; i++) {
#pragma HLS UNROLL
        acc_t tmp = acc[i] + (((acc_t)BIASES[i]) << shift_amount);

        //tmp = act_softplus_pwl(tmp, WEIGHT_SCALES[0] + 8);
        //tmp = act_softplus_pwc(tmp, WEIGHT_SCALES[0] + 8);
        //tmp = act_softplus_numeric(tmp, WEIGHT_SCALES[0] + 8);
        //tmp = act_softplus_lut_L1(tmp);            
        
        //tmp = act_relu(tmp,  WEIGHT_SCALES[0] + 8);
        tmp = act_leaky_relu(tmp,  WEIGHT_SCALES[0] + 8);


        tmp = tmp >> 9;
        out_val.data[i] = (data_t)tmp;
    }
    layer1_out.write(out_val);
}



void layer2_matvec(
    hls::stream<scaled_data4_t> &layer1_in,
    hls::stream<scaled_data4_t> &layer2_out
) {
#pragma HLS INLINE off
#pragma HLS INTERFACE axis port=layer1_in
#pragma HLS INTERFACE axis port=layer2_out
#pragma HLS PIPELINE II=1

    const scale_t weight_scale = WEIGHT_SCALES[1];
    const scale_t bias_scale   = BIAS_SCALES[1];
    const scale_t input_scale  = 5;
    const ap_uint<5> layer_scale = weight_scale + input_scale;
    const ap_uint<4> shift_amount = layer_scale - bias_scale;
    
        scaled_data4_t input = layer1_in.read();


        acc_t partial_sum[LAYER2_OUT];
#pragma HLS ARRAY_PARTITION variable=partial_sum complete
        for (int i = 0; i < LAYER2_OUT; i++) {
#pragma HLS UNROLL
            partial_sum[i] = 0;
        }

        for (int row = 0; row < LAYER2_OUT; row++) {
#pragma HLS UNROLL
            for (int col = 0; col < LAYER2_IN; col++) {
#pragma HLS UNROLL
                partial_sum[row] += WEIGHTS_L2[row * LAYER2_IN + col] * input.data[col];
            }
        }

        scaled_data4_t out_val;
#pragma HLS ARRAY_PARTITION variable=out_val.data complete
        for (int i = 0; i < LAYER2_OUT; i++) {
#pragma HLS UNROLL
            acc_t tmp = partial_sum[i] + (((acc_t)BIASES[4 + i]) << shift_amount);

            //tmp = act_softplus_pwl(tmp, WEIGHT_SCALES[1] + input_scale);
            //tmp = act_softplus_pwc(tmp, WEIGHT_SCALES[1] + input_scale);            
            //tmp = act_softplus_numeric(tmp, WEIGHT_SCALES[1] + input_scale);
            //tmp = act_softplus_lut_L2(tmp);
            
            //tmp = act_relu(tmp,  WEIGHT_SCALES[0] + 8);
            tmp = act_leaky_relu(tmp,  WEIGHT_SCALES[0] + 8);

            tmp = tmp >> 7;
            out_val.data[i] = (data_t)tmp;
        }

        layer2_out.write(out_val);
    }

void layer3_matvec(
    hls::stream<scaled_data4_t> &layer2_out,
    hls::stream<acc_t> &final_out
) {
#pragma HLS INLINE off
#pragma HLS INTERFACE axis port=layer2_out
#pragma HLS INTERFACE axis port=final_out
#pragma HLS PIPELINE II=1
    
    data_t vec_in[LAYER3_IN];
#pragma HLS ARRAY_PARTITION variable=vec_in complete

    scaled_data4_t input = layer2_out.read();
    for(int i = 0; i< LAYER3_IN; i++){
#pragma HLS UNROLL
        vec_in[i] = input.data[i];
    }
    scale_t weight_scale = WEIGHT_SCALES[2];
    scale_t bias_scale = BIAS_SCALES[2];
    bitw_t bit_width = 19;
    
    acc_t p0=0, p1=0, p2=0, p3=0, acc=0;

    p0 = WEIGHTS_L3[0] * vec_in[0];
    p1 = WEIGHTS_L3[1] * vec_in[1];
    p2 = WEIGHTS_L3[2] * vec_in[2];
    p3 = WEIGHTS_L3[3] * vec_in[3];

    acc = p0 + p1 + p2 + p3;

    scale_t current_sf = 4;    
    bias_t bias_raw = BIASES[8];
    int shift_amount = weight_scale + current_sf - bias_scale;
    acc_t bias_scaled = ((acc_t)bias_raw) << shift_amount;
    acc += bias_scaled;

    //acc = act_sigmoid_pwl(acc, weight_scale + current_sf);
    //acc = act_sigmoid_pwc(acc, weight_scale + current_sf);
    //acc = act_sigmoid_numeric(acc, weight_scale + current_sf);
    //acc = act_sigmoid_lut_L3(acc);    

    acc = act_tanh_pwl(acc, weight_scale + current_sf);
    //acc = act_tanh_pwc(acc, weight_scale + current_sf);
    //acc = act_tanh_numeric(acc, weight_scale + current_sf);
    //acc = act_tanh_lut_L3(acc);    
    
    final_out.write(acc);

}


void output_formatter(
    hls::stream<acc_t> &final_in,
    hls::stream<bool> &out_stream
) {
    #pragma HLS INTERFACE axis port=final_in
    #pragma HLS INTERFACE axis port=out_stream
    #pragma HLS PIPELINE II=1
    
        acc_t val = final_in.read();

        //bool sign_bit = (val > 512);
        bool sign_bit = (val > 0);

        out_stream.write(sign_bit);

}

void neural_network(
    hls::stream<rgb_t> &in_stream,
    hls::stream<bool> &out_stream
) {
    // DATAFLOW: allows layers to work in overlapped parallel execution
    #pragma HLS DATAFLOW

    // Interface definitions (AXI4-Stream for data, AXI4-Lite for control)
    #pragma HLS INTERFACE axis port=in_stream
    #pragma HLS INTERFACE axis port=out_stream
    #pragma HLS INTERFACE s_axilite port=return bundle=ctrl
        
    // Memory partitioning for parallel weight reads (BRAM/Registers)
    #pragma HLS ARRAY_PARTITION variable=WEIGHTS_L1 complete dim=1
    #pragma HLS ARRAY_PARTITION variable=WEIGHTS_L2 complete dim=1
    #pragma HLS ARRAY_PARTITION variable=WEIGHTS_L3 complete dim=1
    #pragma HLS ARRAY_PARTITION variable=BIASES complete dim=1

    // Declare internal FIFO buffers (streams)
    static hls::stream<datain_t> r_stream("r_stream");
    static hls::stream<datain_t> g_stream("g_stream");
    static hls::stream<datain_t> b_stream("b_stream");
    #pragma HLS STREAM variable=r_stream depth=32
    #pragma HLS STREAM variable=g_stream depth=32
    #pragma HLS STREAM variable=b_stream depth=32
    
    static hls::stream<scaled_data4_t> layer1_result("layer1_result");
    #pragma HLS STREAM variable=layer1_result depth=32
    static hls::stream<scaled_data4_t> layer2_result("layer2_result");
    #pragma HLS STREAM variable=layer2_result depth=32
    static hls::stream<acc_t> layer3_result("layer3_result");
    #pragma HLS STREAM variable=layer3_result depth=32

    // Task execution chain
    rgb_unpacker(in_stream, r_stream, g_stream, b_stream);
    layer1_matvec(r_stream, g_stream, b_stream, layer1_result);
    layer2_matvec(layer1_result, layer2_result);
    layer3_matvec(layer2_result, layer3_result);
    output_formatter(layer3_result, out_stream);
}
        