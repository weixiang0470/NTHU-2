#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>

#include <random>

using namespace std;

random_device rd;
mt19937 g(rd());

float bipolar_mul(float input1, float input2, int bit_stream_length) {
    if(input1 == 0 || input2 == 0){
        // cout<<"bipolar result = "<<0<<endl;
        return 0.0f;
    }
    // assert(input1 >= -1.0f && input1 <=1.0f);
    /*******************************/
    vector<int> thresholds(bit_stream_length);
    uniform_int_distribution<> dis(1, bit_stream_length);
    for (int i = 0; i < bit_stream_length; ++i) {
        thresholds[i] = i+1;
    }
    // input1
    // Generate no repeated random number
    /**Complete your Design**/
    shuffle(thresholds.begin(), thresholds.end(), g);
    // for (int i = 0; i < bit_stream_length; ++i) {
    //     thresholds[i] = dis(g);
    // }

    // Transfer the binary number to bit stream
    /**Complete your Design**/ // Map [-1,1] -> [0,1] probability
    float prob1 = (input1 + 1.0f) / 2.0f; 
    vector<bool> bitstream1(bit_stream_length);
    int ones_count1 = 0;
    for (int i = 0; i < bit_stream_length; ++i) {
        bitstream1[i] = (prob1 * bit_stream_length >= thresholds[i]);
        ones_count1 += bitstream1[i];
    }
    float reconstructed1 = (2.0f * ones_count1 / bit_stream_length) - 1.0f;
    // cout<<"input 1 = "<< input1 << " Reconstrcuted 1 = "<<reconstructed1<<endl;
    // Count the number of ones and transfer the bit stream to binary number
    /**Complete your Design**/

    /*******************************/
    // input2
    // Generate no repeated random number
    /**Complete your Design**/
    shuffle(thresholds.begin(), thresholds.end(), g);
    // for (int i = 0; i < bit_stream_length; ++i) {
    //     thresholds[i] = dis(g);
    // }

    // Transfer the binary number to bit stream
    /**Complete your Design**/
    float prob2 = (input2 + 1.0f) / 2.0f;
    vector<bool> bitstream2(bit_stream_length);
    int ones_count2 = 0;
    for (int i = 0; i < bit_stream_length; ++i) {
        bitstream2[i] = (prob2 * bit_stream_length >= thresholds[i]);
        ones_count2 += bitstream2[i];
    }
    float reconstructed2 = (2.0f * ones_count2 / bit_stream_length) - 1.0f;

    // Count the number of ones and transfer the bit stream to binary number
    /**Complete your Design**/

    /*******************************/
    // Multiplication on stochastic computing
    /**Complete your Design**/
    int result_ones = 0;
    for (int i = 0; i < bit_stream_length; ++i) {
        // XNOR = !(A XOR B)
        bool xnor_result = !(bitstream1[i] ^ bitstream2[i]); 
        result_ones += xnor_result;
    }

    /*******************************/
    // Scaled addition on stochastic computing
    /**Complete your Design**/
    float result_prob = static_cast<float>(result_ones) / bit_stream_length;
    float bipolar_result = 2.0f * result_prob - 1.0f;
    
    /*******************************/
    // cout<<"bipolar result = "<<bipolar_result<<endl;
    return bipolar_result;
}
