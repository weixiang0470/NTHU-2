#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>

#include <random>
#include <limits.h>

using namespace std;



int* error_injection(int input1, int bit_stream_length) {
    // random_device rd;

    // mt19937 g(rd());
    float error_rate = 0.1;
    float nor_base = 32767.0f;//32767.0f
    // srand( time(NULL) );
    vector<int> thresholds(bit_stream_length);
    uniform_int_distribution<> dis(1, bit_stream_length);

    for (int i = 0; i < bit_stream_length; ++i) {
        thresholds[i] = i+1;
    }

    // Generate no repeated random number

    shuffle(thresholds.begin(), thresholds.end(), g);
    // for (int i = 0; i < bit_stream_length; ++i) {
    //     thresholds[i] = dis(g);
    // }


    // Transfer the binary number to bit stream

    float prob1 = ((input1 + nor_base)) / (2.0f*nor_base); 
    vector<int> bitstream1(bit_stream_length);
    int ones_count1 = 0;
    for (int i = 0; i < bit_stream_length; ++i) {
        bitstream1[i] = (prob1 * bit_stream_length >= thresholds[i]);
        ones_count1 += bitstream1[i];
    }


    // Count the number of ones and transfer the bit stream to binary number

    double reconstructed1 = ((2.0f*nor_base) * ones_count1 / bit_stream_length) - nor_base;
    // cout<<"Before atk Binary number = "<<reconstructed1<<" int = "<<reconstructed1<<endl;

    // Flipping some bits randomly
    shuffle(thresholds.begin(), thresholds.end(), g);
    for(int i=0;i<bit_stream_length*error_rate;i++){
        // int r = dis(g)-1;
        // cout<<r<<endl;
        int atk = thresholds[i] - 1;
        bitstream1[atk] = !bitstream1[atk];
    }


    // Count the number of ones after attacking and transfer the bit stream to binary number

    ones_count1 = 0;
    for (int i = 0; i < bit_stream_length; ++i) {
        // bitstream1[i] = (prob1 * bit_stream_length >= thresholds[i]);
        ones_count1 += bitstream1[i];
    }
    reconstructed1 = ((2.0f*nor_base) * ones_count1 / bit_stream_length) - nor_base;
    int result = (int)(reconstructed1);
    // cout<<"After atk Binary number = "<<reconstructed1<<" int = "<<result<<endl;

    int* bin = new int[16];
    for (int i = 0; i < 16; ++i) {
        bin[i] = (result >> i) & 1;
        // cout<<bin[i];
    }
    return bin;
}
