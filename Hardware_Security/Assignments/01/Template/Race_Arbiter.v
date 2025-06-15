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

endmodule