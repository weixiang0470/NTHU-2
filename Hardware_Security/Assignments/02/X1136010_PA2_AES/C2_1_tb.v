`timescale 1ns / 1ps

module AES_top_tb_2();

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
        $dumpvars(0, AES_top_tb_2);       // Dump all signals in this testbench
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
        plaintext = 128'h0123_4567_89ab_cdef_0123_4567_89ab_cdef;
        key   = 128'h1111_2222_3333_4444_5555_6666_7777_8888;

        #20;
        rst = 0;

        #4000; 

        // End simulation
        $finish;
    end

    // Monitor signals
    initial begin
        $display("Time=%t, cycle_count=%d", $time, dut.cycle_count);
    end

    always @(rst, key, out) begin
            $display("Time=%t, cycle_count=%d", $time, dut.cycle_count);
    end

    initial begin
        $monitor("rst=%b, key[15:0]=%h, out=%h\n",
                rst, key[15:0], out);
    end



endmodule
