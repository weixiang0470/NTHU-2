`timescale 1ns / 1ps

module C2_tb();

    localparam real DELAY_SETS [0:15][0:10] = '{
        '{0.51, 0.52, 0.53, 0.55, 0.57, 0.63, 0.66, 0.67, 0.72, 0.75, 0.88},
        '{0.54, 0.59, 0.62, 0.66, 0.71, 0.79, 0.81, 0.88, 0.92, 0.98, 0.84},
        '{0.50, 0.61, 0.63, 0.69, 0.75, 0.82, 0.89, 0.91, 0.95, 0.96, 0.83},
        '{0.56, 0.57, 0.58, 0.65, 0.68, 0.76, 0.85, 0.89, 0.92, 0.90, 0.82},
        '{0.51, 0.53, 0.69, 0.74, 0.78, 0.84, 0.93, 0.97, 0.99, 0.77, 0.90},
        '{0.52, 0.55, 0.68, 0.71, 0.77, 0.86, 0.95, 0.90, 0.96, 0.79, 0.86},
        '{0.58, 0.64, 0.66, 0.72, 0.80, 0.83, 0.90, 0.92, 0.91, 0.81, 0.87},
        '{0.50, 0.58, 0.62, 0.67, 0.70, 0.81, 0.86, 0.94, 0.99, 0.76, 0.89},
        '{0.51, 0.52, 0.56, 0.59, 0.76, 0.79, 0.87, 0.98, 0.97, 0.78, 0.83},
        '{0.52, 0.60, 0.65, 0.73, 0.78, 0.88, 0.96, 0.99, 0.95, 0.83, 0.81},
        '{0.54, 0.65, 0.69, 0.70, 0.81, 0.85, 0.93, 0.92, 0.99, 0.80, 0.90},
        '{0.53, 0.63, 0.67, 0.76, 0.78, 0.84, 0.92, 0.94, 0.95, 0.78, 0.89},
        '{0.50, 0.57, 0.66, 0.71, 0.82, 0.88, 0.90, 0.91, 0.98, 0.74, 0.82},
        '{0.55, 0.59, 0.62, 0.73, 0.77, 0.83, 0.91, 0.94, 0.93, 0.77, 0.84},
        '{0.51, 0.58, 0.64, 0.68, 0.82, 0.89, 0.95, 0.97, 0.90, 0.78, 0.85},
        '{0.50, 0.52, 0.57, 0.65, 0.69, 0.75, 0.86, 0.92, 0.96, 0.72, 0.88}
    };
    parameter WIDTH = 3;
    
    reg clk, en, sys_rst_pos;
    reg [7:0] chall_in;
    wire [7:0] response;
    wire ready;

    // Instantiate the Top module
    Top #(.DELAY_SETS(DELAY_SETS), .WIDTH(WIDTH)) uut (
        .clk(clk),
        .en(en),
        .sys_rst_pos(sys_rst_pos),
        .chall_in(chall_in),
        .response(response),
        .ready(ready)
    );
    
    always #5 clk = ~clk;

    initial begin
        $fsdbDumpfile("C2_tb.fsdb");
        $fsdbDumpvars(0, C2_tb);
        $monitor("At time %t, chall_in = %h, response = %h, ready = %b", $time, chall_in, response, ready);
    end
    
    initial begin
        clk = 0;
        // first challenge
        $display("First challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'h01;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000

        // second challenge
        $display("Second challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'hc1;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000
        
        // third challenge
        $display("Third challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'h2c;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000
        
        // fourth challenge
        $display("Fourth challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'h01;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000

        $finish;
    end

endmodule