`timescale 1ns / 1ps

module MUX_16to1(
    input [15:0] in,
    input [3:0] sel,
    output wire out
);

    // -------------------------------------------------------------------------
    // Module Declaration:
    // This module is declared with 16 inputs (in), 4 select bits (sel), and 1 output (out).
    // The output 'out' is a multiplexed signal that selects one of the 16 inputs based on
    // the 4-bit select signal 'sel'.
    // -------------------------------------------------------------------------
    assign out = in[sel];
endmodule