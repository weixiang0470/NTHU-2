`timescale 1ns / 1ps

module Top#(
    parameter real DELAY_SETS [0:15][0:10] = '{16{ '{11{0.1}}}}, 
    parameter WIDTH = 3
) (
    input clk,
    input en,
    input sys_rst_pos,
    input [7:0] chall_in, 
    output [7:0] response,
    output ready
);

// -------------------------------------------------------------------------
    // Module Declaration:
    // This Top module coordinates the core functionality by instantiating and
    // connecting multiple submodules:
    //   - FRO (ring oscillator): generates bits (stored in fro_out).
    //   - Scrambler: scrambles challenges (chall_in) into scrm_out.
    //   - MUX_16to1: selects one bit among 16 inputs based on scrm_out.
    //   - Counter: counts rising edges or specific events, parameterized by WIDTH.
    //   - Race_Arbiter: determines which Counter output (a or b) triggers first.
    //   - Buffer: stores accumulated bits and signals when ready.
    //
    // Inputs:
    //   - clk         : clock signal for synchronous operations
    //   - en          : enable signal controlling some modules (e.g., FRO)
    //   - sys_rst_pos : active-high reset for the entire system
    //   - chall_in    : an 8-bit challenge input that is often scrambled
    // Outputs:
    //   - response    : 8-bit final result after collecting bits
    //   - ready       : indicates when the 8-bit response is fully formed
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // 1. Generate FRO instances
    //    - You can use a generate-for loop to instantiate 16 FRO modules
    //    - Pass each subarray of DELAY_SETS[i] and wire their outputs to fro_out[i].
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // 2. Scrambler instantiation
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // 3. MUX instantiations
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // 4. Counter instantiations
    //    - Pass the WIDTH parameter to the Counter module.
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // 5. Race_Arbiter instantiation
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // 6. Buffer instantiation
    // -------------------------------------------------------------------------
endmodule