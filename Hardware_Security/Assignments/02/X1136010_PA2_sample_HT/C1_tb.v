`timescale 1ns / 1ps

module AES_top_tb_1();

    // Testbench signals
    reg clk;
    reg rst;
    reg [127:0] plaintext;
    reg [127:0] key;
    wire [127:0] out;

    // Instantiate the device under test (DUT)
    AES_HT_top dut (
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
        plaintext = 128'h9193_4567_89ab_cdef_0123_4567_89ab_cdef;
        key   = 128'h1111_2222_3333_4444_5555_6666_7777_C0DE;
        // plaintext2 = 128'h9193_4567_89ab_cdef_0123_4567_89ab_cdef;
        #20;
        rst = 0;

        #4000;
        rst=1;
        plaintext = 128'hffff_4567_1233_cdef_0123_4567_89ab_cdef;
        key   = 128'h1111_2222_3333_4444_5555_6666_7777_8888;

        #4020;
        rst=0;


        #8000; 

        // End simulation
        $finish;
    end

    always @(*) begin
        if(dut.cycle_count == 200)begin
            $display("cycle_count=%d, key=%h, out=%h\n", dut.cycle_count, key[15:0], out);
        end
    end

    // Monitor signals
    // initial begin
    //     $display("rst=%b, key=%h, out=%h\n",rst, key[15:0], out);
    //     $monitor("rst=%b, key=%h, out=%h\n",rst, key[15:0], out);
    //     // $display("127=%h, 119=%h",plaintext[127:120],plaintext[119:112]);
    // end



    // always @(rst, key, out) begin
    //     $display("Time=%t, cycle_count=%d", $time, dut.cycle_count);
    // end


endmodule
