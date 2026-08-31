#include "activations.h"
#include "read_data.h"
#include "hls_math.h"

acc_t act_relu(acc_t x, ap_uint<5> scale){
#pragma HLS INLINE off
    if (x < 0) return 0;
    else return x;    
}

acc_t act_leaky_relu(acc_t x, ap_uint<5> scale){
#pragma HLS INLINE off
    if (x < 0) return x/100;
    else return x;     
}

// ----------------- PWL approx -----------------

acc_t act_softplus_pwl(acc_t x, ap_uint<5> scale) {
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=pwl_softplus complete dim=1
#pragma HLS ARRAY_PARTITION variable=pwl_softplus complete dim=2

    const ap_uint<5> default_scale = 14;
    acc_t temp = (1 << (scale + 3));

    if (x < ~temp) return 0;
    else if (x > temp) return x;

    x += temp;

    ap_uint<5> delta = default_scale - scale;               // 14 - 11
    ap_uint<5> interval = x >> (default_scale - delta - 1); 
    
    return (((x >> 8) * pwl_softplus[interval][0]) >> 6) + (pwl_softplus[interval][1] >> delta);;
}

acc_t act_sigmoid_pwl(acc_t x, ap_uint<5> scale) {
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=pwl_sigmoid complete dim=1
#pragma HLS ARRAY_PARTITION variable=pwl_sigmoid complete dim=2

    const ap_uint<5> default_scale = 14;
    acc_t temp = (1 << (scale + 3));

    if (x < ~temp) return 0;
    else if (x > temp) return temp;

    x += temp;

    ap_uint<5> delta = default_scale - scale;
    ap_uint<5> interval = x >> (default_scale - delta - 1);

    return (((x >> 8) * pwl_sigmoid[interval][0]) >> 6) + (pwl_sigmoid[interval][1] >> delta);
}

acc_t act_tanh_pwl(acc_t x, ap_uint<5> scale) {
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=pwl_tanh complete dim=1
#pragma HLS ARRAY_PARTITION variable=pwl_tanh complete dim=2

    const ap_uint<5> default_scale = 14;
    acc_t temp = (1 << (scale + 3));

    if (x < ~temp) return -temp;
    else if (x > temp) return temp; 

    x += temp;

    ap_uint<5> delta = default_scale - scale;
    ap_uint<5> interval = x >> (default_scale - delta - 1);

    return (((x >> 8) * pwl_tanh[interval][0]) >> 6) + (pwl_tanh[interval][1] >> delta);
}

// ----------------- PWC approx -----------------

acc_t act_softplus_pwc(acc_t x, ap_uint<5> scale) {
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=pwc_softplus complete dim=1
#pragma HLS ARRAY_PARTITION variable=pwc_softplus complete dim=2

    const ap_uint<5> default_scale = 14;
    acc_t temp = (1 << (scale + 3));

    if (x < ~temp) return 0;
    else if (x > temp) return x;

    x += temp;

    ap_uint<5> delta = default_scale - scale;
    ap_uint<5> interval = x >> (default_scale - delta - 1);
    
    return pwc_softplus[interval] >> delta;
}

acc_t act_sigmoid_pwc(acc_t x, ap_uint<5> scale) {
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=pwc_sigmoid complete dim=1
#pragma HLS ARRAY_PARTITION variable=pwc_sigmoid complete dim=2

    const ap_uint<5> default_scale = 14;
    acc_t temp = (1 << (scale + 3));

    if (x < ~temp) return 0;
    else if (x > temp) return temp;

    x += temp;

    ap_uint<5> delta = default_scale - scale;
    ap_uint<5> interval = x >> (default_scale - delta - 1);
    
    return pwc_sigmoid[interval] >> delta;
}

acc_t act_tanh_pwc(acc_t x, ap_uint<5> scale) {
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=pwc_tanh complete dim=1
#pragma HLS ARRAY_PARTITION variable=pwc_tanh complete dim=2

    const ap_uint<5> default_scale = 14;
    acc_t temp = (1 << (scale + 3));  
    
    if (x < ~temp) return -temp;
    else if (x > temp) return temp;

    x += temp;

    ap_uint<5> delta = default_scale - scale;
    ap_uint<5> interval = x >> (default_scale - delta - 1);

    return pwc_tanh[interval] >> delta;
}

// ----------------- Numeric Solution -----------------

acc_t act_sigmoid_numeric(acc_t x, ap_uint<5> scale) {
    #pragma HLS INLINE off

    int beta = (1 << scale);
    half xf = (half)x;
    half bf = (half)beta;
    half yf = bf / (1.0f + hls::expf(-xf / bf));

    return (acc_t)yf;
}

acc_t act_softplus_numeric(acc_t x, ap_uint<5> scale) {
    #pragma HLS INLINE off

    int beta = (1 << scale);
    half xf = (half)x;
    half yf = beta * hls::log(1.0f + hls::expf(xf/beta));

    return (acc_t)yf;
}

acc_t act_tanh_numeric(acc_t x, ap_uint<5> scale) {
    #pragma HLS INLINE off

    int beta = (1 << scale);
    half xf = (half)x;
    half bf = (half)beta;
    half yf = bf * hls::tanhf(xf / bf);

    return (acc_t)yf;
}

/*

acc_t act_softplus_lut_L1(acc_t x){
    #pragma HLS inline off
    x = x >> 4;
    x += 8192;
    return SOFTPLUS_LUT_L1[x];
}

acc_t act_softplus_lut_L2(acc_t x){
    #pragma HLS inline off
    x = x >> 5;
    x += 8192;
    return SOFTPLUS_LUT_L2[x];
}

acc_t act_sigmoid_lut_L3(acc_t x){
    #pragma HLS inline off
    x = x >> 5;
    x += 8192;
    return SIGMOID_LUT_L3[x];
}
*/
acc_t act_tanh_lut_L3(acc_t x){
    #pragma HLS inline off
    x = x >> 5;
    x += 8192;
    return TANH_LUT_L3[x];
}
