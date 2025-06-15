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

    wire [127:0] state;
    SubBytes
        sb(clk,state_in,state);
// ShiftRows
    //b1 shift 1 col, b2 shift 2 cols, b3 shift 3 cols
    wire [31:0] b0,b1,b2,b3;
    ShiftRows
        sr(state,{b0,b1,b2,b3});

// AddRoundKey
    AddRoundKey
        ar (key_in,{b0,b1,b2,b3},state_out);

endmodule