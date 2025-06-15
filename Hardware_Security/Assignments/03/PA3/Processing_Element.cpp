#include "Processing_Element.h"
#include "./sc_function_bipolar.h"
#include "./error_injection.h"


/*******************************************************************
    input:
	----------------------------------------------------------------
    PE_req_in: data transmitted to the PE is valid
	PE_ack_out: NI input buffer for receiving from PE is not full
	Data_in: the data transmitted from the NI
	
	P_data_in;
	P_in_valid;
	
	output:
	----------------------------------------------------------------
	PE_ack_in: data buffer in the PE is not full
	PE_req_out: data transmitted from the PE is valid
	Data_out: the data transmitted to the NI
	
	P_data_out;
	P_out_valid;
	
*******************************************************************/	
sc_lv<16> int_array_to_lv(int* bits) {
    sc_lv<16> lv;
    
    for (int i = 0; i < 16; ++i) {
        lv[i] = bits[i] ? '1' : '0';  // 将 1 和 0 转换为 '1' 和 '0' 字符
    }
    return lv;
}

float normalize_sc_int16(sc_int<16> x) {
    return static_cast<float>(x) / INT16_MAX; 
}
int denormalize_float(float x) {
	return static_cast<int>(x * INT16_MAX * INT16_MAX);
}

bool SC_EN = false;
int a_max=0,a_min=0;
int b_max=0,b_min=0;
bool start1=true,start2=true,start3=true;
void Processing_Element::run() {
	
	// cout<<"normalize_sc_int16="<<normalize_sc_int16(1000)<<" denomalize="<<denormalize_float(normalize_sc_int16(1000))<<endl;
    if(reset){
		PE_ack_in = 0;
		PE_req_out = 0;
        P_out_valid = 0;
		temp = "XXXXXXXXXXXXXXXXXXXXXX";
		Data_out = temp;
		P_data_out = temp;
		
		count = 0;
		state = 0;
		// cout << "PE Reset Done\n";
	}
	else{
		PE_ack_in = 1;

		if(state == 0){//receive data: weight
			PE_req_out = 0;
			if(P_in_valid == 1){

				
				if(layer == 1){
				    if(count <= 783)
				        ifmap[count] = sc_int<16>(P_data_in.read());
				    else if(count <= 939)
				        weight[count-784] = sc_int<16>(P_data_in.read());
				    state = (count == 939)?1:0;
				    count = (count == 939)?0:count+1;
			    }
				else if(layer == 3){
				    weight[count] = sc_int<16>(P_data_in.read());
				    state = (count == 2415)?1:0;
				    count = (count == 2415)?0:count+1;
				}
				else if(layer == 5){
				    weight[count] = sc_int<16>(P_data_in.read());
				    state = (count == 30839)?1:0;
				    count = (count == 30839)?0:count+1;
				}
				else if(layer == 6){
				    weight[count] = sc_int<16>(P_data_in.read());
				    state = (count == 10163)?1:0;
				    count = (count == 10163)?0:count+1;
				}
				else if(layer == 7){
				    weight[count] = sc_int<16>(P_data_in.read());
				    state = (count == 849)?1:0;
				    count = (count == 849)?0:count+1;
				}
			}
			else{
				if((layer == 2)||(layer == 4)){
					state = 1;
				    count = 0;
				}
			}
			if(start1){cout <<"layer "<<layer<< " PE State 0 Done"<<endl;start1=false;}
		}
		
		if(state == 1){//receive data: ifmap
            PE_req_out = 0;
			if(PE_req_in == 1){
                if(layer == 2){
					ifmap[count] = sc_int<16>(Data_in.read());
					state = (count == 3455)?2:1;//3455
				    count = (count == 3455)?0:count+1;
			    }
				else if(layer == 3){
					ifmap[count] = sc_int<16>(Data_in.read());
					state = (count == 863)?2:1;//863
				    count = (count == 863)?0:count+1;
				}
				else if(layer == 4){
					ifmap[count] = sc_int<16>(Data_in.read());
					state = (count == 1023)?2:1;//1023
				    count = (count == 1023)?0:count+1;
				}
				else if(layer == 5){
					ifmap[count] = sc_int<16>(Data_in.read());
					state = (count == 255)?2:1;//255
				    count = (count == 255)?0:count+1;
				}
				else if(layer == 6){
					ifmap[count] = sc_int<16>(Data_in.read());
					state = (count == 119)?2:1;//119
				    count = (count == 119)?0:count+1;//119
				}
				else if(layer == 7){
					ifmap[count] = sc_int<16>(Data_in.read());
					state = (count == 83)?2:1;//83
				    count = (count == 83)?0:count+1;
				}
			}
			else{
				if(layer == 1){
					state = 2;
				    count = 0;
				}
			}
			if(start2){cout <<"layer "<<layer<< " PE State 2 Done"<<endl;start2=false;}
			// cout << "PE State 1 Done\n";
		}
		
		else if(state == 2){//compute
			PE_req_out = 0;
			if(layer == 1){//convolution
				for( int i = 0; i < 6 ; i++ ){
				    for( int j = 0; j < 24 ; j++ ){
					    for( int k = 0; k < 24 ; k++ ){
						    ofmap[24*24*i+24*j+k] = 0;
						    for( int m = 0; m < 5 ; m++ ){
							    for( int n = 0; n < 5 ; n++ ){
									// a*b
									// a_max = max(static_cast<int>(ifmap[28*(j+m)+1*(k+n)]),a_max);
									// a_min = min(static_cast<int>(ifmap[28*(j+m)+1*(k+n)]),a_min);
									// b_max = max(static_cast<int>(weight[(5*5+1)*i+5*m+n]),b_max);
									// b_min = min(static_cast<int>(weight[(5*5+1)*i+5*m+n]),b_min);
								    ofmap[24*24*i+24*j+k] = ofmap[24*24*i+24*j+k] + (ifmap[28*(j+m)+1*(k+n)] * weight[(5*5+1)*i+5*m+n])/1024;
							    }
						    }
							if((ofmap[24*24*i+24*j+k] + weight[(5*5+1)*i+25]) >= 0)
					            ofmap[24*24*i+24*j+k] = ofmap[24*24*i+24*j+k] + weight[(5*5+1)*i+25];
					        else
					            ofmap[24*24*i+24*j+k] = 0;
					    }
				    }
			    }
			}
		    if(layer == 2){//max_pooling
				for( int i = 0; i < 6 ; i++ ){
				    for( int j = 0; j < 12 ; j++ ){
					    for( int k = 0; k < 12 ; k++ ){
						    ofmap[12*12*i+12*j+k] = 0;
						    for( int m = 0; m < 2 ; m++ ){
							    for( int n = 0; n < 2 ; n++ ){
									if(ofmap[12*12*i+12*j+k] < ifmap[24*24*i+24*(2*j+m)+(2*k+n)])
									    ofmap[12*12*i+12*j+k] = ifmap[24*24*i+24*(2*j+m)+(2*k+n)];
							    }
						    }
					    }
				    }
			    }
			}
			if(layer == 3){//convolution
				for( int i = 0; i < 16 ; i++ ){
				    for( int j = 0; j < 8 ; j++ ){
					    for( int k = 0; k < 8 ; k++ ){
						    ofmap[8*8*i+8*j+k] = 0;
							for( int l = 0; l < 6 ; l++ ){
						        for( int m = 0; m < 5 ; m++ ){
							        for( int n = 0; n < 5 ; n++ ){
										// a*b
										// a_max = max(static_cast<int>(ifmap[12*12*l+12*(j+m)+1*(k+n)]),a_max);
										// a_min = min(static_cast<int>(ifmap[12*12*l+12*(j+m)+1*(k+n)]),a_min);
										// b_max = max(static_cast<int>(weight[(5*5*6+1)*i+5*5*l+5*m+n]),b_max);
										// b_min = min(static_cast<int>(weight[(5*5*6+1)*i+5*5*l+5*m+n]),b_min);
								        ofmap[8*8*i+8*j+k] = ofmap[8*8*i+8*j+k] + (ifmap[12*12*l+12*(j+m)+1*(k+n)] * weight[(5*5*6+1)*i+5*5*l+5*m+n])/1024;
							        }
						        }
							}
							if((ofmap[8*8*i+8*j+k] + weight[(5*5*6+1)*i+150]) >= 0)
					            ofmap[8*8*i+8*j+k] = ofmap[8*8*i+8*j+k] + weight[(5*5*6+1)*i+150];
					        else
					            ofmap[8*8*i+8*j+k] = 0;
					    }
				    }
			    }
			}
			if(layer == 4){//max_pooling with flatten
				for( int i = 0; i < 4 ; i++ ){
				    for( int j = 0; j < 4 ; j++ ){
					    for( int k = 0; k < 16 ; k++ ){
						    ofmap[4*16*i+16*j+k] = 0;
						    for( int m = 0; m < 2 ; m++ ){
							    for( int n = 0; n < 2 ; n++ ){
									if(ofmap[4*16*i+16*j+k] < ifmap[8*8*k+8*(2*i+m)+(2*j+n)])
									    ofmap[4*16*i+16*j+k] = ifmap[8*8*k+8*(2*i+m)+(2*j+n)];
							    }
						    }
					    }
				    }
			    }
			}

			// for( int i = 0; i < 4 ; i++ ){
			// 	    for( int j = 0; j < 4 ; j++ ){
			// 		    for( int k = 0; k < 16 ; k++ ){
			// 			    ofmap[4*16*i+16*j+k] = 0;
			// 			    for( int m = 0; m < 2 ; m++ ){
			// 				    for( int n = 0; n < 2 ; n++ ){
			// 						int* bits = error_injection(4*16*i+16*j+k,4096);
			// 						sc_lv<16> lv = int_array_to_lv(bits);
			// 						ofmap[i] = lv.to_int();
			// 						// if(i%10==0)cout<<sc_lv<16>(ofmap[ i ]) << endl;
			// 						delete[] bits;
			// 						// if(ofmap[4*16*i+16*j+k] < ifmap[8*8*k+8*(2*i+m)+(2*j+n)])
			// 						//     ofmap[4*16*i+16*j+k] = ifmap[8*8*k+8*(2*i+m)+(2*j+n)];
			// 				    }
			// 			    }
			// 		    }
			// 	    }
			//     }

			if(layer == 5){//Dense
				for( int i = 0; i < 120 ; i++ ){
					ofmap[i] = 0;
					for( int j = 0; j < 256 ; j++ ){
						// a*b
						// a_max = max(static_cast<int>(ifmap[j]),a_max);
						// a_min = min(static_cast<int>(ifmap[j]),a_min);
						// b_max = max(static_cast<int>(weight[(256+1)*i+j]),b_max);
						// b_min = min(static_cast<int>(weight[(256+1)*i+j]),b_min);
						ofmap[i] = ofmap[i] + (ifmap[j] * weight[(256+1)*i+j])/1024;
				    }
					if((ofmap[i] + weight[(256+1)*i+256]) >= 0){
					    ofmap[i] = ofmap[i] + weight[(256+1)*i+256];
						// a_max = max(static_cast<int>(ofmap[i]),a_max);
						}
					else
					    ofmap[i] = 0;
			    }

				/*Attack computing*/
				// srand( time(NULL) );
				for( int i = 0; i < 120 ; i++ ){
					// for( int j = 0; j < 256 ; j++ ){
						// sc_int<16> o_val = ;
						int* bits = error_injection(ofmap[i],2048);
						// cout<<"bits get"<<endl;
						// for(int aaa =0;aaa<16;aaa++)cout<<bits[aaa];
						sc_lv<16> lv = int_array_to_lv(bits);
						ofmap[i] = lv.to_int();
						// if(i%10==0)cout<<sc_lv<16>(ofmap[ i ]) << endl;
						delete[] bits;
				    // }
			    }
				// int random_atk = rand() % 120;
				// int random_atk2 = rand() % 120;
				// cout << "ATK Index: " << random_atk << ", Original: " << ofmap[ random_atk ] << endl;
				// cout << sc_lv<16>(ofmap[ random_atk ]) << endl;

				

				// ofmap[ random_atk ].range(14, 14) = 1;
				// ofmap[ random_atk2 ].range(14, 14) = 1;

				// cout << "Change: " << ofmap[ random_atk ] << endl;
				// cout << sc_lv<16>(ofmap[ random_atk ]) << endl;
			}
			if(layer == 6){//Dense
				for( int i = 0; i < 84 ; i++ ){
					ofmap[i] = 0;
					for( int j = 0; j < 120 ; j++ ){
						//normalised  normalize_sc_int16(); 9080 for a, 950 for b in all layer
						// 4500 for a, 540 for b in layer 6
						float a = ifmap[j]/4500.0f;
						float b = weight[(120+1)*i+j]/540.0f;
						a_max = max(static_cast<int>(ifmap[j]),a_max);
						a_min = min(static_cast<int>(ifmap[j]),a_min);
						b_max = max(static_cast<int>(weight[(120+1)*i+j]),b_max);
						b_min = min(static_cast<int>(weight[(120+1)*i+j]),b_min);
						// denormalize_float()
						// cout<<"a="<<ifmap[j]<<" nor="<<a<<" b="<<weight[(120+1)*i+j]<<" nor="<<b<<endl;
						sc_int<16> sc_result = (bipolar_mul(a, b, 2048)*4500.0f*540.0f)/1024;
						sc_int<16> bc_result = (ifmap[j] * weight[(120+1)*i+j])/1024;
						// cout<<"Real result="<<bc_result<<" sc_result="<<sc_result<<endl;
						SC_EN = true;
						if(SC_EN)ofmap[i] = ofmap[i] + sc_result;
						else ofmap[i] = ofmap[i] + bc_result;
						
				    }
					if((ofmap[i] + weight[(120+1)*i+120]) >= 0)
					    ofmap[i] = ofmap[i] + weight[(120+1)*i+120];
					else
					    ofmap[i] = 0;
			    }
			}
			if(layer == 7){//Dense
				for( int i = 0; i < 10 ; i++ ){
					ofmap[i] = 0;
					for( int j = 0; j < 84 ; j++ ){
						// a*b
						// a_max = max(static_cast<int>(ifmap[j]),a_max);
						// a_min = min(static_cast<int>(ifmap[j]),a_min);
						// b_max = max(static_cast<int>(weight[(84+1)*i+j]),b_max);
						// b_min = min(static_cast<int>(weight[(84+1)*i+j]),b_min);
						ofmap[i] = ofmap[i] + (ifmap[j] * weight[(84+1)*i+j])/1024;
				    }
					ofmap[i] = ofmap[i] + weight[(84+1)*i+84];
			    }
			}
			state = 3;
			count = 0;
			if(start3){cout <<"layer "<<layer<< " PE State 2 Done"<<endl;start3=false;}
			// cout << "PE State 2 Done\n";
		}
		
		else if(state == 3){// send data using NoC to next layer
	        P_out_valid = 0;
			if(PE_ack_out == 1){
				if(layer ==1){
					if(load_done == 1){
						PE_req_out = 1;
					    if(count == 0)
					        cout << "layer 1 complete" << endl;
					    Data_out = sc_lv<22>(ofmap[count]);
			            state = (count == 3455)?4:3;//3455
				        count = (count == 3455)?0:count+1;
					}
				}
				else if(layer ==2){
					PE_req_out = 1;
					if(count == 0)
					    cout << "layer 2 complete" << endl;
					Data_out = sc_lv<22>(ofmap[count]);
			        state = (count == 863)?4:3;//863
				    count = (count == 863)?0:count+1;
				}
				if(layer ==3){
					PE_req_out = 1;
					if(count == 0)
					    cout << "layer 3 complete" << endl;
					Data_out = sc_lv<22>(ofmap[count]);
			        state = (count == 1023)?4:3;//1023
				    count = (count == 1023)?0:count+1;
				}
				if(layer ==4){
					PE_req_out = 1;
					if(count == 0)
					    cout << "layer 4 complete" << endl;
					Data_out = sc_lv<22>(ofmap[count]);
			        state = (count == 255)?4:3;//255
				    count = (count == 255)?0:count+1;
				}
				if(layer ==5){
					PE_req_out = 1;
					if(count == 0)
					    cout << "layer 5 complete" << endl;
					Data_out = sc_lv<22>(ofmap[count]);
			        state = (count == 119)?4:3;//119
				    count = (count == 119)?0:count+1;
				}
				if(layer ==6){
					PE_req_out = 1;
					if(count == 0)
					    cout << "layer 6 complete" << endl;
					Data_out = sc_lv<22>(ofmap[count]);
			        state = (count == 83)?4:3;//83
				    count = (count == 83)?0:count+1;
				}
				if(layer ==7){
					if(count == 0){
						cout << "layer 7 complete" << endl;
						cout << "finish" << endl;
						cout<<"a = "<<a_max<<" "<<a_min<<endl;
						cout<<"b = "<<b_max<<" "<<b_min<<endl;
					}
					PE_req_out = 0;
					P_out_valid = 1;
					P_data_out = sc_int<16>(ofmap[count]);
			        state = (count == 9)?4:3;
				    count = (count == 9)?0:count+1;
				}
			}
			else{
				PE_req_out = 0;
				Data_out = "XXXXXXXXXXXXXXXXXXXXXX";
			}
		}
		else if(state == 4){//idle
			// cout <<"layer "<<layer<< " PE State 4\n";
	        PE_req_out = 0;
	        P_out_valid = 0;
		}
	}
}
