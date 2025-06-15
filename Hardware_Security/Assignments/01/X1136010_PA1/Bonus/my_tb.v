`timescale 1ns / 1ps

module my__tb();

    localparam real DELAY_SETS [0:15][0:10] = '{
        '{0.11, 0.12, 0.13, 0.15, 0.27, 0.33, 0.46, 0.57, 0.62, 0.75, 0.88},
        '{0.14, 0.19, 0.22, 0.26, 0.31, 0.39, 0.41, 0.48, 0.52, 0.68, 0.84},
        '{0.10, 0.21, 0.23, 0.29, 0.35, 0.42, 0.49, 0.51, 0.65, 0.76, 0.83},
        '{0.16, 0.17, 0.18, 0.25, 0.28, 0.36, 0.45, 0.59, 0.62, 0.70, 0.82},
        '{0.11, 0.13, 0.29, 0.34, 0.38, 0.44, 0.53, 0.63, 0.69, 0.77, 0.90},
        '{0.12, 0.15, 0.28, 0.31, 0.37, 0.46, 0.55, 0.60, 0.66, 0.79, 0.86},
        '{0.18, 0.24, 0.26, 0.32, 0.40, 0.43, 0.50, 0.52, 0.61, 0.71, 0.87},
        '{0.10, 0.18, 0.22, 0.27, 0.30, 0.41, 0.46, 0.54, 0.64, 0.76, 0.89},
        '{0.11, 0.12, 0.16, 0.19, 0.36, 0.39, 0.47, 0.58, 0.67, 0.78, 0.83},
        '{0.12, 0.20, 0.25, 0.33, 0.38, 0.48, 0.56, 0.59, 0.65, 0.73, 0.81},
        '{0.14, 0.25, 0.29, 0.30, 0.41, 0.45, 0.53, 0.62, 0.69, 0.80, 0.90},
        '{0.13, 0.23, 0.27, 0.36, 0.38, 0.44, 0.52, 0.64, 0.75, 0.78, 0.89},
        '{0.10, 0.17, 0.26, 0.31, 0.42, 0.48, 0.50, 0.61, 0.68, 0.74, 0.82},
        '{0.15, 0.19, 0.22, 0.33, 0.37, 0.43, 0.51, 0.54, 0.63, 0.77, 0.84},
        '{0.11, 0.18, 0.24, 0.28, 0.42, 0.49, 0.55, 0.67, 0.70, 0.78, 0.85},
        '{0.10, 0.12, 0.17, 0.25, 0.29, 0.35, 0.46, 0.52, 0.66, 0.72, 0.88}
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
        $fsdbDumpfile("my__tb.fsdb");
        $fsdbDumpvars(0, my__tb);
    end
    always @(posedge ready)begin
        if(ready) begin
            $display("At time %t, chall_in = %h, response = %h, ready = %b", $time, chall_in, response, ready);
        end
    end
    
    initial begin
        clk = 0;
        // first challenge
        $display("1 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'h00;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000

        // second challenge
        $display("2 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'hff;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000
        
        // third challenge
        $display("3 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'h01;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000
        
        // fourth challenge
        $display("4 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'hfe;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000

        // 5 challenge
        $display("5 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'h02;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000

        // 6 challenge
        $display("6 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'hfd;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000
        
        // 7 challenge
        $display("7 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'h03;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000
        
        // 8 challenge
        $display("8 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'hfc;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000
        
        // 9 challenge
        $display("9 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'h04;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000

        // 10 challenge
        $display("10 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'hfb;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000
        
        // 11 challenge
        $display("11 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'hd9;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000
        
        // 12 challenge
        $display("12 challenge");
        en = 0;
        sys_rst_pos = 1;
        chall_in = 8'hd9;
        #10
        sys_rst_pos = 0;
        #10
        en = 1;
        #2000
        // fourth challenge
        $display("Fourth challenge");
        $finish;
    end

endmodule
