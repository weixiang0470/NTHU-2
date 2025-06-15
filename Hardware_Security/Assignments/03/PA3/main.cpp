#include "./sc_function_bipolar.h"
#include "./error_injection.h"
#include <systemc.h>

using namespace std;

// random_device rd;
// mt19937 gen(rd());
// uniform_real_distribution<> dis(-100,100);
uniform_int_distribution<> dis(-5000, 5000);

double normalize_sc_int16(sc_int<16> x) {
    if (x >= 0)
        return static_cast<double>(x / 32767.0f);   // 32767
    else
        return static_cast<double>(x / 32767.0f);  // 32768
}

int denormalize_to_sc_int16(double val) {
    if (val >= 0.0f)
        return (val * 32767 * 32767);
    else
        return (val * 32767 * 32767);
}

sc_lv<16> int_array_to_lv(int* bits) {
    sc_lv<16> lv;
    
    for (int i = 0; i < 16; ++i) {
        lv[i] = bits[i] ? '1' : '0';  // 将 1 和 0 转换为 '1' 和 '0' 字符
    }
    return lv;
}
void test_ej(int original_val,int bit_stream_length){
    //  = 10;      // 示例输入值
    //  = 100000;   // 比特流的长度
    cout<<"Original value = "<<original_val<<endl;
    int* bits = error_injection(original_val, bit_stream_length);  // 获取经过错误注入的比特流
    // cout<<"bits done "<< *bits <<endl;
    // 这里的 bits 是一个 int 数组，包含了注入错误后的比特流
    // int bin[16] = {0};
    // for (int i = 0; i < 16; ++i) {
    //     bin[i] = (*bits >> i) & 1;
    //     cout<<bin[i];
    // }
    // cout<<endl;
    sc_lv<16> lv = int_array_to_lv(bits);  // 将 bits 数组转换为 sc_lv<16> 类型

    // 假设 ofmap 是一个已经定义的数组
    int ofmap[10];  // 示例，假设你有一个长度为 10 的 ofmap 数组
    ofmap[0] = lv.to_int();  // 将 lv 转换为整数，并存储到 ofmap[0]

    // 输出转换后的结果  
    cout << "Converted value (after attack): " << ofmap[0] << endl;

    // 释放动态分配的内存
    // delete[] bin;
}
void test_sc(){
    float a=0.01;  // Bipolar: 0.5 → Probability: 0.75
    float b=-0.2; // Bipolar: -0.2 → Probability: 0.4
    int bits = 100000; // Bitstream length
    double sc_result;
    int exact_result;
    double total_err=0;
    int te2=0;
    int round = 10;

    double r2 = static_cast<double>(round);
    // for(int i=0;i<round;i++){
    //     a=dis(gen);
    //     b=dis(gen);
    cout<<"Input 1 = "<<a<<endl;//" Normalised = "<< a/32767.0f <<endl;
    cout<<"Input 2 = "<<b<<endl;//" Normalised = "<< b/32767.0f <<endl;
    sc_result = bipolar_mul(a, b, bits);
    exact_result = a * b;
    double result = a * b;
    // cout<<"Round "<<i+1<<endl;
    cout << "SC Result: " << sc_result <<" int = "<<denormalize_to_sc_int16(sc_result)<< endl;
    cout << "Exact Result: " <<result<<" int = "<< denormalize_to_sc_int16(result)<< endl;
    cout << "Error: " << abs(sc_result - result) <<" int = "<<abs(denormalize_to_sc_int16(sc_result) - denormalize_to_sc_int16(result))<< endl;
    total_err += abs(sc_result - result);
    te2 += abs(denormalize_to_sc_int16(sc_result) - denormalize_to_sc_int16(result));
        // double normalized_val = normalize_sc_int16(val);
        // double normalized_val = normalize_sc_int16(val);
    // }
    // cout<<"Average error : "<< total_err/r2 << " int ="<<te2/round<<endl;
}

int sc_main(int argc, char* argv[]) {
    cout<<"*****************************"<<endl;
    cout<<"Start stochastic test"<<endl;
    test_sc();
    cout<<"*****************************"<<endl;
    cout<<"\n\n"<<endl;
    cout<<"*****************************"<<endl;
    cout<<"Start error test"<<endl;
    test_ej(20000,1000000);
    cout<<"*****************************"<<endl;
    return 0;
}