`timescale 1ns / 1ps

module Counter #(parameter WIDTH = 3) (
    input clk,
    input in,
    input rst,
    output reg out
);
    // -------------------------------------------------------------------------
    // Module Declaration:
    // This module is declared with a parameter WIDTH (default at 3 bits).
    // The inputs include:
    //   - clk: the clock signal driving the counter.
    //   - in : a control signal that influences the counter increment.
    //   - rst: an active-high reset signal that clears the counter.
    // The output 'out' is a registered signal indicating a condition such as
    // "counter has reached its maximum."
    // -------------------------------------------------------------------------

    // 1. Declare a register to hold the counter value.
    //    This register has a width of WIDTH bits, which is 3 bits by default.

    // 2. Implement the synchronous block to handle the counter and output logic.
    //    If the counter reaches its maximum value (e.g., 7 for a 3-bit counter),
    //    the output signal (out) is set high. The counter and output are reset
    //    to 0 when the reset signal (rst) is high.

    reg[WIDTH-1:0] count_reg;

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            count_reg <= 0;
            out <= 0;
        end else if(count_reg == (2**WIDTH - 1)) begin
            out <= 1;
            //count_reg <= 0;
        end else if(in && !out) begin
            count_reg <= count_reg + 1;
        end else begin
            out<=0;
        end
        
    end
endmodule