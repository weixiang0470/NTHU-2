`timescale 1ns / 1ps
/* one AES round for every two clock cycles */
module one_round (clk, state_in, key, state_out);
    input              clk;
    input      [127:0] state_in, key;
    output reg [127:0] state_out;

// SubBytes
// ShiftRows
// MixColumns
    wire [127:0] state;
    MixColumns
        mc(clk,state_in,state);

// AddRoundKey
    AddRoundKey
        ar (key,state,state_out);

endmodule

/* AES final round for every two clock cycles */
module final_round (clk, state_in, key_in, state_out);
    input              clk;
    input      [127:0] state_in;
    input      [127:0] key_in;
    output reg [127:0] state_out;

// SubBytes
    // wire [7:0] b00,b01,b02,b03;
    // wire [7:0] b10,b11,b12,b13;
    // wire [7:0] b20,b21,b22,b23;
    // wire [7:0] b30,b31,b32,b33;
    wire [127:0] state;
    SubBytes
        sb(clk,state_in,state);
// ShiftRows
    //b1 shift 1 col, b2 shift 2 cols, b3 shift 3 cols
    wire [31:0] b0,b1,b2,b3;
    // assign b0 = {b00,b01,b02,b03};
    // assign b1 = {b11,b12,b13,b10};
    // assign b2 = {b22,b23,b20,b21};
    // assign b3 = {b33,b30,b31,b32};
    ShiftRows
        sr(state,{b0,b1,b2,b3});

// AddRoundKey
    AddRoundKey
        ar (key_in,{b0,b1,b2,b3},state_out);

endmodule


    // wire [7:0] c00 = b0[31:24];
    // wire [7:0] c01 = b0[23:16];
    // wire [7:0] c02 = b0[15:8];
    // wire [7:0] c03 = b0[7:0];
    // wire [7:0] c10 = b1[31:24];
    // wire [7:0] c11 = b1[23:16];
    // wire [7:0] c12 = b1[15:8];
    // wire [7:0] c13 = b1[7:0];
    // wire [7:0] c20 = b2[31:24];
    // wire [7:0] c21 = b2[23:16];
    // wire [7:0] c22 = b2[15:8];
    // wire [7:0] c23 = b2[7:0];
    // wire [7:0] c30 = b3[31:24];
    // wire [7:0] c31 = b3[23:16];
    // wire [7:0] c32 = b3[15:8];
    // wire [7:0] c33 = b3[23:16];

    // wire [31:0] c0,c1,c2,c3;
    // xS
    //     s00 (clk, c00, c0[31:24]),
    //     s01 (clk, c01, c0[23:16]),
    //     s02 (clk, c02, c0[15:8]),
    //     s03 (clk, c03, c0[7:0]),
    //     s10 (clk, c10, c1[31:24]),
    //     s11 (clk, c11, c1[23:16]),
    //     s12 (clk, c12, c1[15:8]),
    //     s13 (clk, c13, c1[7:0]),
    //     s20 (clk, c20, c2[31:24]),
    //     s21 (clk, c21, c2[23:16]),
    //     s22 (clk, c22, c2[15:8]),
    //     s23 (clk, c23, c2[7:0]),
    //     s30 (clk, c30, c3[31:24]),
    //     s31 (clk, c31, c3[23:16]),
    //     s32 (clk, c32, c3[15:8]),
    //     s33 (clk, c33, c3[7:0]);