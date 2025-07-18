`timescale 1ns / 1ps

module FRO #(parameter real DELAY_ARR [0:10] = '{
        1.1, 1.2, 1.3, 1.4, 1.5,
        1.6, 1.7, 1.8, 1.9, 2.0,
        2.1
}) (
    input en,
    input rst,  
    output reg out
);

    // -------------------------------------------------------------------------
    // Module Declaration:
    // This module is declared with a parameterized real array DELAY_ARR,
    // intended to hold delay values for different gates. The inputs are 'en'
    // (enable) and 'rst' (reset), while 'out' is a registered signal representing
    // the final output after passing through several logic gates.
    // -------------------------------------------------------------------------

    // 1. Declare all wires required for gate connections (e.g., nand_out, and1_out, etc.)
    //    Students should create internal wire signals to connect gates.

    // 2. Implement always block to assign 'out' = 0 if rst is high,
    //    else use the final gate output as 'out'.

    // 3. Instantiate each gate with its corresponding delay from DELAY_ARR.
	//    You must follow the order of gates as shown in the diagram.

    // 4. Link the outputs of gates to the inputs of subsequent gates to recreate
    //    the original feedback loop.

    // wire declaration
    wire nand_out;
    wire and1_out, and2_out, and3_out;
    wire inv4_out, inv5_out, inv6_out, inv7_out, inv8_out, inv9_out, inv10_out;

    // Output and reset 
    always @(*) begin
        if (rst) 
            out = 1'b0;        
        else 
            out = and3_out;    
    end
    
    nand #(DELAY_ARR[0]) nand0 (nand_out, en, and3_out);
    
    and  #(DELAY_ARR[1]) and1 (and1_out, inv6_out, inv9_out);   
    and  #(DELAY_ARR[2]) and2 (and2_out, inv4_out, inv7_out);  
    and  #(DELAY_ARR[3]) and3 (and3_out, inv8_out, inv10_out); 
    
    not  #(DELAY_ARR[4]) inv4 (inv4_out, nand_out);
    not  #(DELAY_ARR[5]) inv5 (inv5_out, nand_out);
    not  #(DELAY_ARR[6]) inv6 (inv6_out, inv5_out);
    not  #(DELAY_ARR[7]) inv7 (inv7_out, and1_out);
    not  #(DELAY_ARR[8]) inv8 (inv8_out, and2_out);
    not  #(DELAY_ARR[9]) inv9 (inv9_out, and3_out);
    not  #(DELAY_ARR[10]) inv10 (inv10_out, inv5_out);
endmodule