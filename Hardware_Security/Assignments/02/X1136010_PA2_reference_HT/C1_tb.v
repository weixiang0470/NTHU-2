`timescale 1ns / 1ps

module AES_top_tb_1();

    // Testbench signals
    reg clk;
    reg rst;
    reg [127:0] plaintext;
    reg [127:0] key;
    wire [127:0] out;
    reg [127:0] mykey;

    // Instantiate the device under test (DUT)
    AES_HT_top dut(
        .clk(clk),
        .rst(rst),
        .plaintext(plaintext),
        .key(key),
        .out(out)
    );

    Reverse_key dut2(
        .clk(clk),
        .ciphertext(out),
        .key(mykey)
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
        #20;
        rst = 0;

        #225; 
        rst   = 1;
        plaintext = 128'hf1f3_4567_89ab_cdef_0123_4567_89ab_cdef;
        #235;
        rst=0;

        #415;
        rst   = 1;
        plaintext = 128'he1e3_4567_89ab_cdef_0123_4567_89ab_cdef;
        #425;
        rst=0;

        #645;
        rst=1;
        plaintext = 128'h1113_4567_89ab_cdef_0123_4567_89ab_cdef;
        #665;
        rst=0;

        #865;
        rst=1;
        plaintext = 128'hf1f3_4567_89aa_cdef_0123_4567_89ab_cdef;
        #885;
        rst=0;

        #1085;
        rst   = 1;
        plaintext = 128'h9193_4567_89ab_cdef_0123_4567_89ab_cdef;
        #1105;
        rst=0;

        #1305;
        rst=1;
        plaintext = 128'h1113_4567_89ab_cdef_0123_4567_89ab_cdef;
        #1325;
        rst=0;

        #1525;
        rst   = 1;
        plaintext = 128'h9193_4567_89ab_cdef_0123_4567_89ab_cdef;
        #1545;
        rst=0;

        #1745;
        rst=1;
        plaintext = 128'h1113_4567_89ab_cdef_0123_4567_89ab_cdef;
        #1765;
        rst=0;

        #1965;
        // End simulation
        $finish;
    end

    always @(out)begin
        if (dut.aes.trigger==1) begin
            $display("Trigger=%d, reversed_key=%h",dut.aes.trigger ,mykey);
        end
        $display("rst=%b, en=%b, key=%h, out=%h, ctr=%d\n",rst,dut.aes.en,key[15:0],out,dut.aes.ctr);
    end

    // Monitor signals
    initial begin
        // $monitor("rst=%b, en=%b, key=%h, out=%h, ctr=%d\n",rst,dut.aes.en,key[15:0],out,dut.aes.ctr);
        // $display("127=%h, 119=%h",plaintext[127:120],plaintext[119:112]);
    end
    // always @(rst, key, out2) begin
    //     $display("Time=%t, cycle_count=%d", $time, dut2.cycle_count);
    // end


endmodule
