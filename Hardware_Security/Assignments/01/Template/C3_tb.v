`timescale 1ns / 1ps

module C3_tb();

    localparam real DELAY_SETS [0:15][0:10] = '{
        '{0.12, 0.23, 0.34, 0.45, 0.56, 0.67, 0.78, 0.89, 1.01, 1.12, 1.23},
        '{0.13, 0.24, 0.35, 0.46, 0.57, 0.68, 0.79, 0.90, 1.02, 1.13, 1.24},
        '{0.14, 0.25, 0.36, 0.47, 0.58, 0.69, 0.80, 0.91, 1.03, 1.14, 1.25},
        '{0.15, 0.26, 0.37, 0.48, 0.59, 0.70, 0.81, 0.92, 1.04, 1.15, 1.26},
        '{0.16, 0.27, 0.38, 0.49, 0.60, 0.71, 0.82, 0.93, 1.05, 1.16, 1.27},
        '{0.17, 0.28, 0.39, 0.50, 0.61, 0.72, 0.83, 0.94, 1.06, 1.17, 1.28},
        '{0.18, 0.29, 0.40, 0.51, 0.62, 0.73, 0.84, 0.95, 1.07, 1.18, 1.29},
        '{0.19, 0.30, 0.41, 0.52, 0.63, 0.74, 0.85, 0.96, 1.08, 1.19, 1.30},
        '{0.20, 0.31, 0.42, 0.53, 0.64, 0.75, 0.86, 0.97, 1.09, 1.20, 1.31},
        '{0.21, 0.32, 0.43, 0.54, 0.65, 0.76, 0.87, 0.98, 1.10, 1.21, 1.32},
        '{0.22, 0.33, 0.44, 0.55, 0.66, 0.77, 0.88, 0.99, 1.11, 1.22, 1.33},
        '{0.23, 0.34, 0.45, 0.56, 0.67, 0.78, 0.89, 1.00, 1.12, 1.23, 1.34},
        '{0.24, 0.35, 0.46, 0.57, 0.68, 0.79, 0.90, 1.01, 1.13, 1.24, 1.35},
        '{0.25, 0.36, 0.47, 0.58, 0.69, 0.80, 0.91, 1.02, 1.14, 1.25, 1.36},
        '{0.26, 0.37, 0.48, 0.59, 0.70, 0.81, 0.92, 1.03, 1.15, 1.26, 1.37},
        '{0.27, 0.38, 0.49, 0.60, 0.71, 0.82, 0.93, 1.04, 1.16, 1.27, 1.38}
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
        $fsdbDumpfile("C3_tb.fsdb");
        $fsdbDumpvars(0, C3_tb);
        $monitor("At time %t, chall_in = %h, response = %h, ready = %b", $time, chall_in, response, ready);
    end
    
    initial begin
        clk = 0;
        // first challenge
        $display("First challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'hd1;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #5000

        // second challenge
        $display("Second challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'hfe;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #5000
        
        // third challenge
        $display("Third challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'h30;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #5000
        
        // fourth challenge
        $display("Fourth challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'h58;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000
        chall_in = 8'h58;
        #3000

        $finish;
    end

endmodule