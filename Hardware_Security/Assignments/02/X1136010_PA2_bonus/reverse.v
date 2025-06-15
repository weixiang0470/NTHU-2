`timescale 1ns / 1ps
module Reverse(ciphertext,out);
    input [127:0] ciphertext;
    output [127:0] out;
    
    assign out = ciphertext ^ ~128'h1234_5678_abcd_dcba_abcd_cdab_4321_3412;

endmodule