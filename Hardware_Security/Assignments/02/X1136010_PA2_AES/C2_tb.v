`timescale 1ns / 1ps

module AES_top_tb_1();

    // Testbench signals
    reg clk;
    reg rst;
    reg [127:0] plaintext;
    reg [127:0] key;
    wire [127:0] out;

    // Instantiate the device under test (DUT)
    AES_top dut (
        .clk(clk),
        .rst(rst),
        .plaintext(plaintext),
        .key(key),
        .out(out)
    );

    // Dump waveforms for debugging
    initial begin
        $dumpfile("AES_top_tb.fsdb");    // Waveform output file
        $dumpvars(0, AES_top_tb_1);       // Dump all signals in this testbench
    end
    
    // Generate a clock with 10 ns period (toggle every 5 ns)
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // Apply stimulus
    initial begin
        // Initially assert reset and set default inputs
        rst   = 1;
        plaintext = 128'h1111_5555_abab_c145_9587_a193_89ab_cdef;
        key   = 128'h2b7e_1516_28ae_d2a6_abf7_1588_09cf_4f3c;

        #20;
        rst = 0;

        #4000; 

        // End simulation
        $finish;
    end

    // Monitor signals
    initial begin
        $monitor("rst=%b, key[15:0]=%h, out=%h\n",
                rst, key[15:0], out);
    end



endmodule
