`timescale 1ns / 1ps
module MixColumns(clk,state_in,state_out);

    input clk;
    input [127:0] state_in;
    output [127:0] state_out;
    
    wire [7:0] a00 = state_in[127:120];
    wire [7:0] a01 = state_in[95:88];
    wire [7:0] a02 = state_in[63:56];
    wire [7:0] a03 = state_in[31:24];

    wire [7:0] a10 = state_in[119:112];
    wire [7:0] a11 = state_in[87:80];
    wire [7:0] a12 = state_in[55:48];
    wire [7:0] a13 = state_in[23:16];
    
    wire [7:0] a20 = state_in[111:104];
    wire [7:0] a21 = state_in[79:72];
    wire [7:0] a22 = state_in[47:40];
    wire [7:0] a23 = state_in[15:8];
    
    wire [7:0] a30 = state_in[103:96];
    wire [7:0] a31 = state_in[71:64];
    wire [7:0] a32 = state_in[39:32];
    wire [7:0] a33 = state_in[7:0];

    wire[31:0] p00,p01,p02,p03;
    wire[31:0] p10,p11,p12,p13;
    wire[31:0] p20,p21,p22,p23;
    wire[31:0] p30,p31,p32,p33;

//[
//  [a00,a01,a02,a03],
//  [a10,a11,a12,a13],
//  [a20,a21,a22,a23],
//  [a30,a31,a32,a33]
//]

//[
//  [a00, a01, a02, a03],
//  [a11, a12, a13, a10],
//  [a22, a23, a20, a21],
//  [a33, a30, a31, a32]
//]

    table_lookup 
        t0(clk, {a00,a11,a22,a33}, p00, p11, p22, p33),
        t1(clk, {a01,a12,a23,a30}, p01, p12, p23, p30),
        t2(clk, {a02,a13,a20,a31}, p02, p13, p20, p31),
        t3(clk, {a03,a10,a21,a32}, p03, p10, p21, p32);

    wire [31:0] col1,col2,col3,col4;
    assign col1 = p00 ^ p11 ^ p22 ^ p33;
    assign col2 = p01 ^ p12 ^ p23 ^ p30;
    assign col3 = p02 ^ p13 ^ p20 ^ p31;
    assign col4 = p03 ^ p10 ^ p21 ^ p32;

    // always @ (posedge clk)
    assign state_out = {
      col1,col2,col3,col4
    };
endmodule


    // wire [7:0] a00 = state_in[127:120];
    // wire [7:0] a01 = state_in[119:112];
    // wire [7:0] a02 = state_in[111:104];
    // wire [7:0] a03 = state_in[103:96];

    // Shift 1 column
    // wire [7:0] a10 = state_in[95:88];
    // wire [7:0] a11 = state_in[87:80];
    // wire [7:0] a12 = state_in[79:72];
    // wire [7:0] a13 = state_in[71:64];
    
    // Shift 2 columns
    // wire [7:0] a20 = state_in[63:56];
    // wire [7:0] a21 = state_in[55:48];
    // wire [7:0] a22 = state_in[47:40];
    // wire [7:0] a23 = state_in[39:32];
    
    // Shift 3 columns
    // wire [7:0] a30 = state_in[31:24];
    // wire [7:0] a31 = state_in[23:16];
    // wire [7:0] a32 = state_in[15:8];
    // wire [7:0] a33 = state_in[7:0];