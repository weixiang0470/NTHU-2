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
    wire [15:0] fro_out;
    genvar i;
    generate
        for (i=0;i<16;i=i+1) begin : gen_fros
            FRO #(DELAY_SETS[i]) fro_inst (
                .en(en),
                .rst(sys_rst_pos),
                .out(fro_out[i])
            );
        end
    endgenerate
    //   - Scrambler: scrambles challenges (chall_in) into scrm_out.
    // -------------------------------------------------------------------------
    // 2. Scrambler instantiation
    // -------------------------------------------------------------------------
    wire [7:0] scrm_out;
    Scrambler scrambler_inst (
        .in(chall_in),
        .clk(clk),
        .rst(sys_rst_pos),
        .out(scrm_out)
    );
    //   - MUX_16to1: selects one bit among 16 inputs based on scrm_out.
    // -------------------------------------------------------------------------
    // 3. MUX instantiations
    // -------------------------------------------------------------------------
    wire muxa_out;
    wire muxb_out;
    MUX_16to1 mux_inst1(
        .in(fro_out),
        .sel(scrm_out[3:0]),
        .out(muxa_out)
    );
    MUX_16to1 mux_inst2(
        .in(fro_out),
        .sel(scrm_out[7:4]),
        .out(muxb_out)
    );
    //   - Counter: counts rising edges or specific events, parameterized by WIDTH.
    // -------------------------------------------------------------------------
    // 4. Counter instantiations
    //    - Pass the WIDTH parameter to the Counter module.
    // -------------------------------------------------------------------------
    wire countera_out;
    wire counterb_out;
    Counter #(WIDTH) counter_inst1(
        .clk(clk),
        .in(muxa_out),
        .rst(buffer_rst),
        .out(countera_out)
    );
    Counter #(WIDTH) counter_inst2(
        .clk(clk),
        .in(muxb_out),
        .rst(buffer_rst),
        .out(counterb_out)
    );
    //   - Race_Arbiter: determines which Counter output (a or b) triggers first.
    // -------------------------------------------------------------------------
    // 5. Race_Arbiter instantiation
    // -------------------------------------------------------------------------
    wire ab_result;
    wire ab_done;
    Race_Arbiter race_arbiter_inst(
        .a(countera_out), 
        .b(counterb_out), 
        .rst(buffer_rst),
        .result(ab_result),
        .done(ab_done)
    );
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
    // 6. Buffer instantiation
    // -------------------------------------------------------------------------
    Buffer buffer_inst(
        .clk(clk),
        .rst(sys_rst_pos),
        .arbiter_result(ab_result),
        .arbiter_done(ab_done),

        .response(response),
        .ready(ready),
        .buf_rst(buffer_rst)
    );
endmodule