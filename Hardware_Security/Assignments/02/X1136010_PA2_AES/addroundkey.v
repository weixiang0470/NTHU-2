`timescale 1ns / 1ps
module AddRoundKey(key,state_in,state_out);
    input [127:0] key,state_in;
    output [127:0] state_out;

    assign state_out = key ^ state_in;
endmodule