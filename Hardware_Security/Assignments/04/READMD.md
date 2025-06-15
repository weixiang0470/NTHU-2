# NBTI
- When PMOS working at high temp and -ve voltage
    - 導通， 高溫
1. Si–H 鍵斷裂：位於矽與氧化層界面（Si/SiO₂）的矽氫鍵會被打斷。
2. 界面陷阱產生：產生帶電的界面陷阱（interface traps），增加了 VT（閾值電壓）。
3. 效能劣化：導致電晶體的閾值電壓 V_th 上升，進而降低驅動能力與速度，影響數位電路的效能與可靠性。

# 5 Microarchitecture-level magic-based attack

- No technical or equipment requirement, the MAGIC program is executed on the targeteed device in its functional mode.

## The OpenSPARC framework
- Demo the attack on OpenSPARC T1 processor [Oracle 2006b].
    - Open for academic use
    - Note: MAGIC-based attack is generic 
    - MAGIC program can be generated for any processor based on its netlist and instruction set.

- E stage is the slowest pipestage (target)

## Methodology
- circuit-level attack is used to identify the patterns
    - used to accelerate NBTI aging in the critical path
- ISA -> MAGIC instructions (MAGIC program)
    - MAGIC instruction produce MAGIC patterns
- To accelerate NBTI aging, flip-flop should hold thier values for a certain time
    - MAGIC state
- Phase 1, certain instruction to bring the processor into MAGIC state
- Phase 2, MAGIC instruction are repeatedly executed
- Focus on combinational gates
- State change after OS has finished booting
    - from S_rnd -> S_i -> S_i+1 -> ... -> S_magic -> S_magic...
- Obtain the input pattern using an ATPG tool
    - If all the FF cannot be set to particular value by executing any instruction then cannot put under maximum stree
    - If majority of those FF are crontrollable, then critical path can be put under considerable stress

## Construct MAGIC instructions
- use inputs of the decode stage and ISA to construct MAGIC instructions

# 6 Results and analysis

## Experimental setup
- 45nm predictive technology model
    - high-k dielectric[Model 2007]
- Source code of T1 processor was obtain from Oracle [2006a]
- Synopsys Design Compiler tool
    - logic synthesis
- Synopsys PrimeTime
    - extracting timing-critical paths
- Python
    - select final patterns and evaluate the attack outcome
- Synopsys VCS 
    - Functional simulations
- Nominal voltage 
    - 1.1V
- Nominal temperature 
    - 80'C, consider for the core temperature
    - On average is 90'C(45nm Intel processors)
- Effect of aging over time
    - Mathematical model in Wang et al.[2010]
- Table II SPARC Core configuration detials
## Experimental Results
- Critical path of the E stage
    - In the bypass logic(figure 6b)
    - 110 gates : 46 AND gates, 15 INV, 46 OR gates, 3 XOR gates
- Input patterns, 603 primary inputs to E stage, 563 were don't care
    - 40 inputs to E stage were the outputs of the decode stage.
    - From decode logic get how to produce these outputs From Decode to E stage
- Fig 9
    - op == 10
    - rd == ddddd, don't care
    - op3 == 0d01dd, SUB, SUBcc, ANDN, ANDNcc, ORN, ORNcc, XNOR, XNORcc, any of this will have same effect, paper use SUB
    - rs1 == ddddd, don't care
    - use_imm = 1
    - immediate = 0FED
- MAGIC instructions e.g. 
    - SUB r12, OFED, r20
    - SUB r4, 001A, r27
- Possible MAGIC instruc
    - 2^5 * 2^5 * 2^3 = 8192
    - But cannot have same source or destinations register
    - 8192 - (2^5 * 2^3) = 7936
- To reach MAGIC state, 77 flip-flops were controllable

- Increase switching activity -> increase power dissipation and temperature -> NBTI effects

- Fig 10,11
    - random -> 0.28-0.41(1 month - 6 month)
    - different number of controllable FF will have different result
    