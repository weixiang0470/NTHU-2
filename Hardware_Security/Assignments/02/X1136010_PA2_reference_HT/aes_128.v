module aes_ht_128(clk, state, key, out);
    input          clk;
    // input          en;
    input  [127:0] state, key;
    output [127:0] out;
    reg    [127:0] s0, k0, tmp, previous_state=128'b0;
    wire   [127:0] s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
                   k1, k2, k3, k4, k5, k6, k7, k8, k9,
                   k0b, k1b, k2b, k3b, k4b, k5b, k6b, k7b, k8b, k9b;

    reg    [1:0]   ctr=0;
    // reg    [2:0]   cycle_count=0;
    reg            trigger=0;
    reg            en;
    
    always @ (posedge clk) begin
        s0 <= state ^ key;
        k0 <= key;
        en = state[127] & state[119];
        if (en==0) begin
          ctr <= 0;
          trigger <= 0;
          tmp <= 128'h0000_0000_0000_0000_0000_0000_0000_0000;
        end
        if (previous_state!=state && en==1) begin
          ctr <= ctr+1;
          if(ctr == 3)begin
            trigger <= 1;
            tmp <= 128'hffff_ffff_ffff_ffff_ffff_ffff_ffff_ffff;
          end else begin
            trigger <= 0;
            tmp <= 128'h0000_0000_0000_0000_0000_0000_0000_0000;
          end 
        end else begin
          trigger <= 0;
          tmp <= 128'h0000_0000_0000_0000_0000_0000_0000_0000;
        end
        
        // if(trigger==1)begin
        //   cycle_count <= cycle_count + 1;
        //   if(cycle_count == 2)begin
        //     trigger<=0;
        //     ctr<=0;
        //   end
        // end
        if(ctr==3) begin
          trigger <= 1;
          tmp <= 128'hffff_ffff_ffff_ffff_ffff_ffff_ffff_ffff;
          // $display("out = %h",out);
        end else begin
          trigger <= 0;
          tmp <= 128'h0000_0000_0000_0000_0000_0000_0000_0000;
        end
        //$display("en=%b, ps=%h, s=%h",en,previous_state,state);
        previous_state <= state;
    end

    assign s10 = s9 | tmp;

    expand_key_128
        a1 (clk, k0, k1, k0b, 8'h1),
        a2 (clk, k1, k2, k1b, 8'h2),
        a3 (clk, k2, k3, k2b, 8'h4),
        a4 (clk, k3, k4, k3b, 8'h8),
        a5 (clk, k4, k5, k4b, 8'h10),
        a6 (clk, k5, k6, k5b, 8'h20),
        a7 (clk, k6, k7, k6b, 8'h40),
        a8 (clk, k7, k8, k7b, 8'h80),
        a9 (clk, k8, k9, k8b, 8'h1b),
       a10 (clk, k9,   , k9b, 8'h36);

    one_round
        r1 (clk, s0, k0b, s1),
        r2 (clk, s1, k1b, s2),
        r3 (clk, s2, k2b, s3),
        r4 (clk, s3, k3b, s4),
        r5 (clk, s4, k4b, s5),
        r6 (clk, s5, k5b, s6),
        r7 (clk, s6, k6b, s7),
        r8 (clk, s7, k7b, s8),
        r9 (clk, s8, k8b, s9);

    final_round
        rf (clk, s10, k9b, out);
endmodule

