#include "LeNet.h"

/*******************************************************************
    input:
	----------------------------------------------------------------
    sc_in < DATA_TYPE > rom_data_out;
	sc_in < DATA_TYPE > ram_data_out;
	
	output:
	----------------------------------------------------------------
	sc_out < bool > rom_rd;
	sc_out < sc_uint<16> > rom_addr;
	
	sc_out < bool > ram_wr;
	sc_out < sc_uint<16> > ram_addr;
	sc_out < sc_int<16> > ram_data_in;
	
	sc_out < sc_int<16> > result;
	sc_out < bool > output_valid;
*******************************************************************/	
bool start=true;
void LeNet::run() {

    if(reset){
		rom_rd = 0;
		rom_addr = 0;
		ram_wr = 1;//read valid
		ram_addr = 0;
		ram_data_in = 0;
		result = 0;
		output_valid = 0;
		count = 0;
	}
	else{
		ram_wr = 1;//read valid
		ram_addr = 0;
		ram_data_in = 0;
		
		if(state == 0){//layer 1: input: 784 and weight: 156
			// if(start){cout<<"LeNet state 0 "<<endl;start=false;}
			P_1_0_in_valid = 0;//PE1
			P_2_0_in_valid = 0;//PE2
			P_0_1_in_valid = 0;//PE3
			P_1_1_in_valid = 0;//PE4
			P_2_1_in_valid = 0;//PE5
			P_0_2_in_valid = 0;//PE6
			P_1_2_in_valid = 0;//PE7
			P_2_2_in_valid = 0;//PE8
			
			rom_rd = 1;
			if(count<=783)
		        rom_addr = count+44426;
			else if(count<=939)
			    rom_addr = count-784;
			if( count >= 1 ){
				P_0_0_in_valid = 1;
				P_0_0_datain = sc_lv<16>(rom_data_out.read());
			}
			else{
				P_0_0_in_valid = 0;
				cout<<"LeNet state 0 "<<endl;
			}
		    result = 0;
		    output_valid = 0;
			state = (count==940)?1:0;
			count = (count==940)?0:count+1;
			// if(count==940)start=true;
		}
		
		if(state == 1){//layer 3: weight: 2416
			// if(start){cout<<"LeNet state 1 "<<endl;start=false;}
			P_1_0_in_valid = 0;
			P_0_1_in_valid = 0;
			P_1_1_in_valid = 0;
			P_2_1_in_valid = 0;
			P_0_2_in_valid = 0;
			P_1_2_in_valid = 0;
			P_2_2_in_valid = 0;
			
			rom_rd = 1;
			rom_addr = count+156;
			if( count >= 1 ){
				P_0_0_in_valid = 0;
				P_2_0_in_valid = 1;
				P_2_0_datain = sc_lv<16>(rom_data_out.read());
			}
			else{
				cout<<"LeNet state 1 "<<endl;
				P_0_0_in_valid = 1;
				P_2_0_in_valid = 0;
			}
			result = 0;
			output_valid = 0;
			state = (count==2416)?2:1;
			count = (count==2416)?0:count+1;
			// if(count==2416)start=true;
		}
		
		else if(state == 2){//layer 5: weight: 30840
			// if(start){cout<<"LeNet state 2 "<<endl;start=false;}
		    P_0_0_in_valid = 0;
			P_1_0_in_valid = 0;
			P_0_1_in_valid = 0;
			P_2_1_in_valid = 0;
			P_0_2_in_valid = 0;
			P_1_2_in_valid = 0;
			P_2_2_in_valid = 0;
			
			rom_rd = 1;
			rom_addr = count+2572;
			if( count >= 1 ){
				P_2_0_in_valid = 0;
				P_1_1_in_valid = 1;
				P_1_1_datain = sc_lv<16>(rom_data_out.read());
			}
			else{
				cout<<"LeNet state 2 "<<endl;
				P_2_0_in_valid = 1;
				P_1_1_in_valid = 0;
			}
			result = 0;
			output_valid = 0;
			state = (count==30840)?3:2;
			count = (count==30840)?0:count+1;
			// if(count==30840)start=true;
		}
		
		else if(state == 3){//layer 6: weight: 10164
			// if(start){cout<<"LeNet state 3 "<<endl;start=false;}
		    P_0_0_in_valid = 0;
			P_1_0_in_valid = 0;
			P_2_0_in_valid = 0;
			P_0_1_in_valid = 0;
			P_0_2_in_valid = 0;
			P_1_2_in_valid = 0;
			P_2_2_in_valid = 0;
			
			rom_rd = 1;
			rom_addr = count+33412;
			if( count >= 1 ){
				P_1_1_in_valid = 0;
				P_2_1_in_valid = 1;
				P_2_1_datain = sc_lv<16>(rom_data_out.read());
			}
			else{
				cout<<"LeNet state 3 "<<endl;
				P_1_1_in_valid = 1;
				P_2_1_in_valid = 0;
			}
			result = 0;
			output_valid = 0;
			state = (count==10164)?4:3;
			count = (count==10164)?0:count+1;
			// if(count==10164)start=true;
		}
		
		else if(state == 4){//layer 7: weight: 850
			// if(start){cout<<"LeNet state 4 "<<endl;start=false;}
		    P_0_0_in_valid = 0;
			P_1_0_in_valid = 0;
			P_2_0_in_valid = 0;
			P_0_1_in_valid = 0;
			P_1_1_in_valid = 0;
			P_1_2_in_valid = 0;
			P_2_2_in_valid = 0;
			
			rom_rd = 1;
			rom_addr = count+43576;
			if( count >= 1 ){
				P_2_1_in_valid = 0;
				P_0_2_in_valid = 1;
				P_0_2_datain = sc_lv<16>(rom_data_out.read());
			}
			else{
				cout<<"LeNet state 4 "<<endl;
				P_2_1_in_valid = 1;
				P_0_2_in_valid = 0;
			}
			result = 0;
			output_valid = 0;
			state = (count==850)?5:4;
			count = (count==850)?0:count+1;
			// if(count==850)start=true;
		}
		
		else if(state == 5){
			// if(start){cout<<"LeNet state 5 "<<endl;start=false;}
		    load_done = 1;
		    P_0_0_in_valid = 0;
			P_1_0_in_valid = 0;
			P_2_0_in_valid = 0;
			P_0_1_in_valid = 0;
		    P_1_1_in_valid = 0;
			P_2_1_in_valid = 0;
			P_1_2_in_valid = 0;
			P_2_2_in_valid = 0;
			P_0_2_in_valid = (count >= 1)?0:1;
		    state = 6;
			count = (count==1)?1:count+1;
			cout<<"LeNet state 5 "<<endl;
			// if(count==1)start=true;
		}

		else if(state == 6){//final
			// cout<<"LeNet state 6 "<<endl;
			if(start){cout<<"LeNet state 6 "<<endl;start=false;}
		    P_0_0_in_valid = 0;
			P_1_0_in_valid = 0;
			P_2_0_in_valid = 0;
			P_0_1_in_valid = 0;
		    P_1_1_in_valid = 0;
			P_2_1_in_valid = 0;
			P_1_2_in_valid = 0;
			P_2_2_in_valid = 0;
			P_0_2_in_valid = 0;
		    
		    if( P_0_2_out_valid == 1 ){
				// cout<<"PE 6 done calculation"<<endl;
			    rom_rd = 0;
			    rom_addr = 0;
			    ram_wr = 1;//read valid
			    ram_addr = 0;
			    ram_data_in = 0;
			    result = sc_int<16>(P_0_2_dataout);
			    output_valid = 1;
			}
		}
	}	
}

