`timescale 1ns / 1ps

module AES_HT_top (
    input clk,
    input rst,
    input [127:0] plaintext,
    input [127:0] key,
    output reg [127:0] out
);

    wire [127:0] ciphertext;
    // reg [9:0] cycle_count;
    // wire en = plaintext[127] & plaintext[119];

    // assign en = plaintext[127] & plaintext[119];

    aes_ht_128 aes (
        .clk(clk),
        // .en(en),
        .state(plaintext),
        .key(key),
        .out(ciphertext)
    );

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            out <= {128{1'bx}};  
            // cycle_count = 10'b0000000000;
            // en <= plaintext[127] & plaintext[119];
        end 
        else begin
            // cycle_count = cycle_count + 1;
            // en <= plaintext[127] & plaintext[119];
            out <= ciphertext;  
        end
    end

endmodule