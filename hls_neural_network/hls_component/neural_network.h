#ifndef PIXEL_SHIFT_H
#define PIXEL_SHIFT_H

#include "ap_int.h"
#include "hls_stream.h"
#include "read_data.h"

void neural_network(
    hls::stream<rgb_t> &in_stream,
    hls::stream<bool> &out_stream
);

void rgb_unpacker(
    hls::stream<rgb_t> &in_stream,
    hls::stream<datain_t> &r_stream,
    hls::stream<datain_t> &g_stream,
    hls::stream<datain_t> &b_stream
);

void layer1_matvec(
    hls::stream<datain_t> &r_stream,
    hls::stream<datain_t> &g_stream,
    hls::stream<datain_t> &b_stream,
    hls::stream<scaled_data4_t> &layer1_out
);

void layer2_matvec(
    hls::stream<scaled_data4_t> &layer1_out,
    hls::stream<scaled_data4_t> &layer2_out
);

void layer3_matvec(
    hls::stream<scaled_data4_t> &layer2_out,
    hls::stream<acc_t> &final_out
);

#endif