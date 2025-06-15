#include "NI.h"

void Network_Interface::init() {
	// ----------------read buffer init data----------------
	ifstream TrafficFile("/home/HardwareSecurity_1132/tsx1136010/HW03/PA3/traffic.txt", ios::in);
	string line, stemp = "";
	int PE_number, itemp;
	getline(TrafficFile, line);
		
	while ( true ) {
		getline(TrafficFile, line);
		istringstream tline(line);
		tline >> stemp;
		if ( stemp == "." )
			break;

		tline >> PE_number >> stemp >> itemp;
		if ( ly * NoC_size + lx == PE_number ) {
			dx = ( itemp ) % NoC_size;
			dy = ( itemp ) / NoC_size;
			// cout << "( " << sc_lv<2>(lx) << ", " << sc_lv<2>(ly) << ") to " << "( " << sc_lv<2>(dx) << ", " << sc_lv<2>(dx) << ")!\n";
		}
	}
}

/*******************************************************************
	input:
	----------------------------------------------------------------
	L_req_in: flit transmitted from the router is valid
	L_ack_out: local input buffer in the router is not full
	PE_req_in: data transmitted from the PE is valid
	PE_ack_out: data buffer in the PE is not full
	Flit_in: the flit transmitted from the router
	Data_in: the data transmitted from the PE
	
	output:
	----------------------------------------------------------------
	L_ack_in: NI input buffer for receiving from router is not full
	L_req_out: flit transmitted to the router is valid
	PE_ack_in: NI input buffer for receiving from PE is not full
	PE_req_out: data transmitted to the PE is valid
	Flit_out: the flit transmitted to the router
	Data_out: the data transmitted to the PE

*******************************************************************/		

void Network_Interface::run() {

    if(reset){
		L_ack_in = 0;
		PE_ack_in = 0;
		
		L_req_out = 0;
		PE_req_out = 0;
		
		temp1 = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
		temp2 = "XXXXXXXXXXXXXXXXXXXXXX";
		
		Flit_out = temp1;
		Data_out = temp2;
		
		NI_router_buffer = (temp1,temp1,temp1,temp1,temp1,temp1,temp1,temp1);
		NI_PE_buffer = (temp2,temp2,temp2,temp2,temp2,temp2,temp2,temp2);
		
	    NI_router_available = 0;
		NI_PE_available = 0;
		//state = 0;
	}
	else{
		
		////////////////////////// send flit to router //////////////////////////
		if(NI_PE_available > 0 && L_ack_out == 1){ //local input buffer in the router is not full
			L_req_out = 1;
			Flit_out =("01",sc_lv<2>(lx),sc_lv<2>(ly),sc_lv<2>(dx),sc_lv<2>(dy),NI_PE_buffer.range(21,0));
			for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
				NI_PE_buffer.range((22*i)+21,(22*i)+0) = NI_PE_buffer.range((22*(i+1))+21,(22*(i+1))+0);
			NI_PE_buffer.range(175,154) = "XXXXXXXXXXXXXXXXXXXXXX";
			NI_PE_available = NI_PE_available - 1;
		    /*state = 0;
			if(state == 0){
			    Flit_out =("00",sc_lv<2>(lx),sc_lv<2>(ly),sc_lv<2>(dx),sc_lv<2>(dy),"0000000000000000000000");
				state = 1;
			}
			else if(state == 1){
				Flit_out =("01",sc_lv<2>(lx),sc_lv<2>(ly),sc_lv<2>(dx),sc_lv<2>(dy),NI_PE_buffer.range(21,0));
				state = 2;
			}
			else if(state == 2){
				Flit_out =("10",sc_lv<2>(lx),sc_lv<2>(ly),sc_lv<2>(dx),sc_lv<2>(dy),"0000000000000000000000");
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
				    NI_PE_buffer.range((22*i)+21,(22*i)+0) = NI_PE_buffer.range((22*(i+1))+21,(22*(i+1))+0);
			    NI_PE_buffer.range(175,154) = "XXXXXXXXXXXXXXXXXXXXXX";
			    NI_PE_available = NI_PE_available - 1;
				state = 0;
			}*/
		}
		else{
			L_req_out = 0;
			Flit_out = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
		}

		////////////////////////// send data to PE //////////////////////////
		if(NI_router_available > 0 && PE_ack_out == 1){ //data buffer in the PE is not full
			PE_req_out = 1;
			Data_out = NI_router_buffer.range(21,0);
			for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
				NI_router_buffer.range((32*i)+31,(32*i)+0) = NI_router_buffer.range((32*(i+1))+31,(32*(i+1))+0);
			NI_router_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
			NI_router_available = NI_router_available - 1;
		}
		else{
			PE_req_out = 0;
			Data_out = "XXXXXXXXXXXXXXXXXXXXXX";
		}

		////////////////////////// receive data from PE //////////////////////////
		if(NI_PE_available < 8 && PE_req_in == 1){ //data transmitted from the PE is valid
		    NI_PE_buffer.range((22*NI_PE_available)+21,(22*NI_PE_available)+0) = Data_in.read();
		    NI_PE_available = NI_PE_available + 1;
			//cout << "layer " <<  ly*3+lx+1 << ": receive data from PE" << endl;
		}
		
		PE_ack_in = (NI_PE_available < 8)? 1 : 0;

		////////////////////////// receive flit from router //////////////////////////
		if(NI_router_available < 8 && L_req_in == 1){ //flit transmitted from the router is valid
		    NI_router_buffer.range((32*NI_router_available)+31,(32*NI_router_available)+0) = Flit_in.read();
		    NI_router_available = NI_router_available + 1;
			//cout << "layer " <<  ly*3+lx+1 << ": receive flit from router" << endl;
		}
		
		L_ack_in = (NI_router_available < 8)? 1 : 0;
	}	
}

