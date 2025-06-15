`timescale 1ns / 1ps
module ShiftRows(state_in, state_out);

    input [127:0] state_in;
    output [127:0] state_out;

    // wire [31:0] b0,b1,b2,b3;
    // wire [31:0] b0=state_in[127:96];
    // wire [31:0] b1=state_in[95:64];
    // wire [31:0] b2=state_in[63:32];
    // wire [31:0] b3=state_in[31:0];
    // assign state_out = {b0,b1[23:0],b1[31:24],b2[15:0],b2[31:16],b3[7:0],b3[31:8]};
    
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

    assign state_out = {
        a00,a11,a22,a33,
        a01,a12,a23,a30,
        a02,a13,a20,a31,
        a03,a10,a21,a32
        };
endmodule
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