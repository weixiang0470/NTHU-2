`timescale 1ns / 1ps
module SubBytes(clk,state_in, state_out);
    input clk;
    input [127:0] state_in;
    output [127:0] state_out;

    // wire [31:0] a0 = state_in[127:96];
    // wire [31:0] a1 = state_in[95:64];
    // wire [31:0] a2 = state_in[63:32];
    // wire [31:0] a3 = state_in[31:0];
    
    S4
        S_0(clk,state_in[127:96],state_out[127:96]),
        S_1(clk,state_in[95:64],state_out[95:64]),
        S_2(clk,state_in[63:32],state_out[63:32]),
        S_3(clk,state_in[31:0],state_out[31:0]);
endmodule