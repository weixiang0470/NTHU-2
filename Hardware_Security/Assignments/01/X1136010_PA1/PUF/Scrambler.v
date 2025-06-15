`timescale 1ns / 1ps

module Scrambler (
    input [7:0] in,
    input clk,
    input rst,
    output reg [7:0] out
);

    // -------------------------------------------------------------------------
    // Module Declaration:
    // This module takes an 8-bit input ('in') and scrambles it based on
    // a Linear Feedback Shift Register (LFSR)-like mechanism. The clock ('clk')
    // drives the internal shift operations, while 'rst' resets the internal state.
    // The scrambled result is driven on the 8-bit output 'out'.
    // -------------------------------------------------------------------------

    // 1. Declare an internal register (e.g., state) to hold the LFSR state.

    // 2. Assign new_bit = XOR of selected bits in 'state' to produce the LFSR feedback.

    // 3. Implement the always block triggered on the rising edge of clk or rst.
    //    - If rst is high, initialize the LFSR state with 'in'.
    //    - Otherwise, shift in new_bit and generate 'out' from state ^ in.
    reg [7:0] state;
    wire new_bit;

    assign new_bit = state[7] ^ state[3] ^ state[2] ^ state[1] ^ state[0];

    always @(posedge clk or posedge rst)begin
        if(rst)begin
            state <= in;
        end else begin
            state <= {new_bit, state[7:1]};
        end        
        out = state ^ in;
    end
endmodule