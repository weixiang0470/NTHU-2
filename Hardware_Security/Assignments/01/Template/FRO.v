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

endmodule