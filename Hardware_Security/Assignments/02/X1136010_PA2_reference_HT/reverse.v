`timescale 1ns / 1ps
module function_f (
    clk,
    rcon,
    W,
    f_out
);
    input clk;
    input [7:0] rcon;
    input [31:0] W;  
    output [31:0] f_out;

    wire [7:0] k12 = W[31:24];
    wire [7:0] k13 = W[23:16];
    wire [7:0] k14 = W[15:8];
    wire [7:0] k15 = W[7:0];

    wire [7:0] s0, s1, s2, s3;

    S4
        S4_0 (clk, {k13,k14,k15,k12}, {s0,s1,s2,s3});

    // assign f[31:24] = f[31:24] ^ rcon;
    // always @ (posedge clk)
    //     f_out[31:24] = {f[31:24] ^ rcon,f[23:0]};
    assign f_out = {s0 ^ rcon, s1, s2, s3};
endmodule

module Reverse_round(
    clk,
    rcon,
    round_key1,
    out_1,
    out_2
);
    input clk;
    input [7:0] rcon;
    input [127:0] round_key1;
    output reg [127:0] out_1;
    output [127:0] out_2;

    wire [31:0] W0b=round_key1[127:96],W1b=round_key1[95:64],W2b=round_key1[63:32],W3b=round_key1[31:0];
    wire [31:0] f_w3b;
    wire [31:0] W0a,W1a,W2a,W3a;

    function_f
        f0 (clk, rcon, W3a, f_w3b);

    assign W0a = W0b ^ f_w3b;
    assign W1a = W1b ^ W0b;
    assign W2a = W2b ^ W1b;
    assign W3a = W3b ^ W2b;

    always @ (posedge clk)
        out_1 <= {W0a,W1a,W2a,W3a};
    assign out_2 = {W0a,W1a,W2a,W3a};
    
endmodule

module Reverse_key (
    clk,
    ciphertext,
    key
);
    input clk;
    input reg [127:0] ciphertext;
    output [127:0] key;

    reg [127:0] k10;
    wire   [127:0] k0, k1, k2, k3, k4, k5, k6, k7, k8, k9,
                   k1b, k2b, k3b, k4b, k5b, k6b, k7b, k8b, k9b;

    always @ (posedge clk)
        k10 <= ciphertext ^ 128'h1616_1616_1616_1616_1616_1616_1616_1616;
    
    Reverse_round
        a1 (clk, 8'h1,k1,k0,key),
        a2 (clk, 8'h2,k2,k1,k1b),
        a3 (clk, 8'h4,k3,k2,k2b),
        a4 (clk, 8'h8,k4,k3,k3b),
        a5 (clk, 8'h10,k5,k4,k4b),
        a6 (clk, 8'h20,k6,k5,k5b),
        a7 (clk, 8'h40,k7,k6,k6b),
        a8 (clk, 8'h80,k8,k7,k7b),
        a9 (clk, 8'h1b,k9,k8,k8b),
       a10 (clk, 8'h36,k10,k9,k9b);

endmodule