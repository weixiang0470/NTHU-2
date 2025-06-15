`timescale 1ns / 1ps

module Race_Arbiter (
    input a, 
    input b, 
    input rst,
    output result,
    output done
);

    // -------------------------------------------------------------------------
    // Module Declaration:
    // This module acts as an arbiter to determine which signal (a or b)
    // arrives or asserts first. 'rst' provides an active-high reset that
    // clears any latch or final decision. 'result' indicates which input
    // won the "race," and 'done' indicates the module has detected an activation.
    // -------------------------------------------------------------------------

    assign result = (rst) ? 0 : (a) ? 1 : (b) ? 0 : result;
    assign done = (rst) ? 0 : (a | b);
    // always @(posedge a or posedge b or posedge rst) begin
    //     if (rst) begin
    //         result = 0;
    //         done = 0;
    //     end else if (a) begin
    //         result = 1;
    //         done = 1;
    //     end else if (b) begin
    //         result = 0;
    //         done = 1;
    //     end
    // end

endmodule