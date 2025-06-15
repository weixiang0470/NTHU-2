# National Tsing Hua University
- 11320IIS500800 Hardware Security
- Final Project: Data Integrity with Checkpoint and ECC
- Authors: team01

## A. How to compile and execute your config file including Hamming Code 
### After building gem5 with the modified ECC support, you can execute simulations using the following commands:
```
./build/X86/gem5.opt --debug-flags=MemCtrl ./configs/config_ecc_none.py 	>> project_output.txt
./build/X86/gem5.opt --debug-flags=MemCtrl ./configs/config_ecc_hamming.py 	>> project_output.txt
./build/X86/gem5.opt --debug-flags=MemCtrl ./configs/config_ecc_rs.py 		>> project_output.txt
./build/X86/gem5.opt --debug-flags=MemCtrl ./configs/config_ecc_bch.py 		>> project_output.txt
```

**Note: The >> project_output.txt portion is optional; it redirects the simulation output to a file for convenience.**

**Note: The --debug-flags=MemCtrl option must be enabled. ECC-related logs are printed only under this debug flag, which includes encode/decode operations and any detected/corrected memory errors.**

## B. The files modified or added

- The following files were added or modified as part of this ECC integration. 
    - Please place them in the correct directories within the gem5 source tree.

```
/configs/config_ecc_none.py		
/configs/config_ecc_hamming.py		
/configs/config_ecc_rs.py	
/configs/config_ecc_bch.py 
/src/mem/MemCtrl.py
/src/mem/mem_ctrl.hh
/src/mem/mem_ctrl.cc 			
/src/mem/class-read-solomon-code.hh	
/src/mem/class-hamming-code.hh	
/src/mem/class-BCH-code.hh
```