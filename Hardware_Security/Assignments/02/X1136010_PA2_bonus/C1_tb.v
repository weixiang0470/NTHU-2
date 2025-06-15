`timescale 1ns / 1ps

module AES_top_tb_1();

    // Testbench signals
    reg clk;
    reg rst;
    reg [127:0] plaintext;
    reg [127:0] key;
    wire [127:0] out,my_out;


    // Instantiate the device under test (DUT)
    AES_top dut (
        .clk(clk),
        .rst(rst),
        .plaintext(plaintext),
        .key(key),
        .out(out)
    );

    Reverse dut2 (
        .ciphertext(out),
        .out(my_out)
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

        #205; 
        rst   = 1;
        plaintext = 128'hf1f3_4567_89ab_cdef_0123_ab67_89ab_cdf1;
        #225;
        rst=0;

        #425;
        rst   = 1;
        plaintext = 128'he1e3_4567_89ab_cdef_0123_ab67_89ab_cde1;
        #445;
        rst=0;

        #645;
        rst=1;
        plaintext = 128'h1234_5678_abcd_dcba_abcd_cdab_4321_3412;
        #665;
        rst=0;

        #865;
        rst=1;
        plaintext = 128'h9193_4567_89ab_cdef_0123_4567_89ab_cdef;
        #885;
        rst=0;

        #1085;
        rst   = 1;
        plaintext = 128'hf1f3_4567_89ab_cdef_0123_ab67_89ab_cdf1;
        #1105;
        rst=0;

        #1305;
        rst=1;
        plaintext = 128'he1e3_4567_89ab_cdef_0123_ab67_89ab_cde1;
        #1325;
        rst=0;

        #1525;
        rst   = 1;
        plaintext = 128'hf1f3_4567_89ab_cdef_0123_ab67_89ab_cdf1;
        #1545;
        rst=0;

        #1745;
        rst=1;
        plaintext = 128'he1e3_4567_89ab_cdef_0123_ab67_89ab_cde1;
        #1765;
        rst=0;

        #1965;

        // End simulation
        $finish;
    end

    always @(*) begin
        if(dut.aes.ctr == 3) begin
            $display("ctr=%d, ciphertext=%h\ntrigger=%d, my_out=%h\n",dut.aes.ctr,out,dut.aes.trigger,my_out);
        end
    end

    // // Monitor signals
    // always @(dut.aes.ctr) begin
    //     $monitor("rst=%b, key=%h, out=%h, ctr=%d\n",rst, key[15:0], out, dut.aes.ctr);
    // end

endmodule
