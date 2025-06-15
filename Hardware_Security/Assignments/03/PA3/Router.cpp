#include "Router.h"
/********************************************************************************************	
	input: 
	-----------------------------------------------------------------------------------------
	N_data_in: the flit transmitted to the North port
	W_data_in: the flit transmitted to the West port
	S_data_in: the flit transmitted to the South port
	E_data_in: the flit transmitted to the East port
	L_data_in: the flit transmitted to the Local port
	
	N_req_in: flit transmitted to the North port is valid
	W_req_in: flit transmitted to the West port is valid
	S_req_in: flit transmitted to the South port is valid
	E_req_in: flit transmitted to the East port is valid
	L_req_in: flit transmitted to the Local port is valid
	
	N_ack_out: input buffer that the flit from the North port transmitted to is not full
	W_ack_out: input buffer that the flit from the West port transmitted to is not full
	S_ack_out: input buffer that the flit from the South port transmitted to is not full
	E_ack_out: input buffer that the flit from the East port transmitted to is not full
	L_ack_out: input buffer that the flit from the Local port transmitted to is not full
	
	output: 
	-----------------------------------------------------------------------------------------
	N_data_out: the flit transmitted from the North port
	W_data_out: the flit transmitted from the West port
	S_data_out: the flit transmitted from the South port
	E_data_out: the flit transmitted from the East port
	L_data_out: the flit transmitted from the Local port
	
	N_ack_in: North input buffer is not full
	W_ack_in: West input buffer is not full
	S_ack_in: South input buffer is not full
	E_ack_in: East input buffer is not full
	L_ack_in: Local input buffer is not full
	
	N_req_out: flit transmitted from the North port is valid
	W_req_out: flit transmitted from the West port is valid
	S_req_out: flit transmitted from the South port is valid
	E_req_out: flit transmitted from the East port is valid
	L_req_out: flit transmitted from the Local port is valid

********************************************************************************************/
void Router::run() {
	
    if(reset){
		N_ack_in = 0;
	    W_ack_in = 0;
	    S_ack_in = 0;
	    E_ack_in = 0;
	    L_ack_in = 0;

        N_req_out = 0;
		W_req_out = 0;
		S_req_out = 0;
		E_req_out = 0;
		L_req_out = 0;
		
		temp = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
		
		N_data_out = temp;
	    W_data_out = temp;
	    S_data_out = temp;
	    E_data_out = temp;
	    L_data_out = temp;
		
		North_input_buffer = (temp,temp,temp,temp,temp,temp,temp,temp);
		West_input_buffer = (temp,temp,temp,temp,temp,temp,temp,temp);
		South_input_buffer = (temp,temp,temp,temp,temp,temp,temp,temp);
		East_input_buffer = (temp,temp,temp,temp,temp,temp,temp,temp);
		Local_input_buffer = (temp,temp,temp,temp,temp,temp,temp,temp);
		
		East_buffer_available = 0;
	    West_buffer_available = 0;
	    North_buffer_available = 0;
	    South_buffer_available = 0;
	    Local_buffer_available = 0;
	}
	else{
		////////////////////////// routing computing unit for East input buffer //////////////////////////
		if (East_buffer_available > 0 && E_ack_out == 1 ){
			/*(2, 1) to (0, 2)*/
			#ifdef ROUTER_DEFENCE
			tmpKey = getKey(East_input_buffer.range(25, 22));
			East_input_buffer = xoring(East_input_buffer, tmpKey);
			#endif

			#ifdef ROUTER_ATTACK
			East_input_buffer = attack(East_input_buffer);
			#endif 

			#ifdef ROUTER_DEFENCE
			East_input_buffer = xoring(East_input_buffer, tmpKey);
			#endif			

			if (sc_int<5>(sc_uint<32>(East_input_buffer).range(25,24) - lx) > 0) //East
				cout << "Error" << endl;
			else if (sc_int<5>(sc_uint<32>(East_input_buffer).range(25,24) - lx) < 0){ //West
				East_to_West = 1;
				East_to_North = 0;
				East_to_South = 0;
				East_to_Local = 0;
			}
			else if (sc_int<5>(sc_uint<32>(East_input_buffer).range(23,22) - ly) > 0){ //South
				East_to_West = 0;
				East_to_North = 0;
				East_to_South = 1;
				East_to_Local = 0;
			}
			else if (sc_int<5>(sc_uint<32>(East_input_buffer).range(23,22) - ly) < 0){ //North
				East_to_West = 0;
				East_to_North = 1;
				East_to_South = 0;
				East_to_Local = 0;
			}
			else{  //Local
				East_to_West = 0;
				East_to_North = 0;
				East_to_South = 0;
				East_to_Local = 1;
			}
		}
		else if (East_buffer_available <= 0 && E_ack_out == 1 ){ //empty
			East_to_West = 0;
			East_to_North = 0;
			East_to_South = 0;
			East_to_Local = 0;
		}
			
		////////////////////////// routing computing unit for West input buffer //////////////////////////
		if (West_buffer_available > 0 && W_ack_out == 1 ){
			/*(0, 1) to (1, 1)*/
			if (sc_int<5>(sc_uint<32>(West_input_buffer).range(25,24) - lx) > 0){ //East
				West_to_East = 1;
				West_to_North = 0;
				West_to_South = 0;
				West_to_Local = 0;
			}
			else if (sc_int<5>(sc_uint<32>(West_input_buffer).range(25,24) - lx) < 0) //West
				cout << "Error" << endl;
			else if (sc_int<5>(sc_uint<32>(West_input_buffer).range(23,22) - ly) > 0){ //South
				West_to_East = 0;
				West_to_North = 0;
				West_to_South = 1;
				West_to_Local = 0;
			}
			else if (sc_int<5>(sc_uint<32>(West_input_buffer).range(23,22) - ly) < 0){ //North
				West_to_East = 0;
				West_to_North = 1;
				West_to_South = 0;
				West_to_Local = 0;
			}
			else{ //Local
				West_to_East = 0;
				West_to_North = 0;
				West_to_South = 0;
				West_to_Local = 1;
			}
		}
		else if (West_buffer_available <= 0 && W_ack_out == 1 ){ //empty
			West_to_East = 0;
			West_to_North = 0;
			West_to_South = 0;
			West_to_Local = 0;
		}
			
		////////////////////////// routing computing unit for North input buffer //////////////////////////
		if (North_buffer_available > 0 && N_ack_out == 1 ){
			/*Empty( No mapping )*/
			if (sc_int<5>(sc_uint<32>(North_input_buffer).range(25,24) - lx) > 0){ //East
				North_to_East = 1;
				North_to_West = 0;
				North_to_South = 0;
				North_to_Local = 0;
			}
			else if (sc_int<5>(sc_uint<32>(North_input_buffer).range(25,24) - lx) < 0){ //West
				North_to_East = 0;
				North_to_West = 1;
				North_to_South = 0;
				North_to_Local = 0;
			}
			else if (sc_int<5>(sc_uint<32>(North_input_buffer).range(23,22) - ly) > 0){ //South
				North_to_East = 0;
				North_to_West = 0;
				North_to_South = 1;
				North_to_Local = 0;
			}
			else if (sc_int<5>(sc_uint<32>(North_input_buffer).range(23,22) - ly) < 0) //North
				cout << "Error" << endl;
			else{ //Local
				North_to_East = 0;
				North_to_West = 0;
				North_to_South = 0;
				North_to_Local = 1;
			}
		}
		else if (North_buffer_available <= 0 && N_ack_out == 1 ){ //empty
			North_to_East = 0;
			North_to_West = 0;
			North_to_South = 0;
			North_to_Local = 0;
		}
			
		////////////////////////// routing computing unit for South input buffer //////////////////////////
		if (South_buffer_available > 0 && S_ack_out == 1 ){
			/*Empty( No mapping )*/
			if (sc_int<5>(sc_uint<32>(South_input_buffer).range(25,24) - lx) > 0){ //East
				South_to_East = 1;
				South_to_West = 0;
				South_to_North = 0;
				South_to_Local = 0;
			}
			else if (sc_int<5>(sc_uint<32>(South_input_buffer).range(25,24) - lx) < 0){ //West
				South_to_East = 0;
				South_to_West = 1;
				South_to_North = 0;
				South_to_Local = 0;
			}
			else if (sc_int<5>(sc_uint<32>(South_input_buffer).range(23,22) - ly) > 0) //South
				cout << "Error" << endl;
			else if (sc_int<5>(sc_uint<32>(South_input_buffer).range(23,22) - ly) < 0){ //North
				South_to_East = 0;
				South_to_West = 0;
				South_to_North = 1;
				South_to_Local = 0;
			}
			else{ //Local
				South_to_East = 0;
				South_to_West = 0;
				South_to_North = 0;
				South_to_Local = 1;
			}
		}
		else if (South_buffer_available <= 0 && S_ack_out == 1 ){ //empty
			South_to_East = 0;
			South_to_West = 0;
			South_to_North = 0;
			South_to_Local = 0;
		}
			
		////////////////////////// routing computing unit for Local input buffer //////////////////////////
		if (Local_buffer_available > 0 && L_ack_out == 1 ){
			/*Empty( No mapping )*/
			if (sc_int<5>(sc_uint<32>(Local_input_buffer).range(25,24) - lx) > 0){ //East
				Local_to_East = 1;
				Local_to_West = 0;
				Local_to_North = 0;
				Local_to_South = 0;
			}
			else if (sc_int<5>(sc_uint<32>(Local_input_buffer).range(25,24) - lx) < 0){ //West
				Local_to_East = 0;
				Local_to_West = 1;
				Local_to_North = 0;
				Local_to_South = 0;
			}
			else if (sc_int<5>(sc_uint<32>(Local_input_buffer).range(23,22) - ly) > 0){ //South
				Local_to_East = 0;
				Local_to_West = 0;
				Local_to_North = 0;
				Local_to_South = 1;
			}
			else if (sc_int<5>(sc_uint<32>(Local_input_buffer).range(23,22) - ly) < 0){ //North
				Local_to_East = 0;
				Local_to_West = 0;
				Local_to_North = 1;
				Local_to_South = 0;
			}
			else //Local
				cout << "Error" << endl;
		}
		else if (Local_buffer_available <= 0 && L_ack_out == 1 ){ //empty
			Local_to_East = 0;
			Local_to_West = 0;
			Local_to_North = 0;
			Local_to_South = 0;
		}

		////////////////////////// send flit to East output port //////////////////////////
		if(E_ack_out == 1){
			if(North_to_East == 1){
				E_req_out = 1;
				E_data_out = North_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					North_input_buffer.range((32*i)+31,(32*i)+0) = North_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				North_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				North_buffer_available = North_buffer_available - 1;
			}
		    else if(West_to_East == 1){
				E_req_out = 1;
				E_data_out = West_input_buffer.range(31,0);			
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					West_input_buffer.range((32*i)+31,(32*i)+0) = West_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				West_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				West_buffer_available = West_buffer_available - 1;
			}
		    else if(South_to_East == 1){
				E_req_out = 1;
				E_data_out = South_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					South_input_buffer.range((32*i)+31,(32*i)+0) = South_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				South_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				South_buffer_available = South_buffer_available - 1;
			}
		    else if(Local_to_East == 1){
				E_req_out = 1;
				E_data_out = Local_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					Local_input_buffer.range((32*i)+31,(32*i)+0) = Local_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				Local_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				Local_buffer_available = Local_buffer_available - 1;
			}
		    else{
				E_req_out = 0;
				E_data_out = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
			}
		}
		else{
			E_req_out = 0;
			E_data_out = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
		}
			
		////////////////////////// send flit to West output port //////////////////////////
		if(W_ack_out == 1){
		    if(North_to_West == 1){
				W_req_out = 1;
				W_data_out = North_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					North_input_buffer.range((32*i)+31,(32*i)+0) = North_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				North_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				North_buffer_available = North_buffer_available - 1;
			}
		    else if(South_to_West == 1){
				W_req_out = 1;
				W_data_out = South_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					South_input_buffer.range((32*i)+31,(32*i)+0) = South_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				South_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				South_buffer_available = South_buffer_available - 1;
			}
		    else if(East_to_West == 1){
				W_req_out = 1;
				W_data_out = East_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					East_input_buffer.range((32*i)+31,(32*i)+0) = East_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				East_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				East_buffer_available = East_buffer_available - 1;
			}
		    else if(Local_to_West == 1){
				W_req_out = 1;
				W_data_out = Local_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					Local_input_buffer.range((32*i)+31,(32*i)+0) = Local_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				Local_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				Local_buffer_available = Local_buffer_available - 1;
			}
		    else{
				W_req_out = 0;
				W_data_out = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
			}	
		}
		else{
			W_req_out = 0;
			W_data_out = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
		}
		
		////////////////////////// send flit to North output port //////////////////////////
		if(N_ack_out == 1){
		    if(West_to_North == 1){
				N_req_out = 1;
				N_data_out = West_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					West_input_buffer.range((32*i)+31,(32*i)+0) = West_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				West_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				West_buffer_available = West_buffer_available - 1;
			}
		    else if(South_to_North == 1){
				N_req_out = 1;
				N_data_out = South_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					South_input_buffer.range((32*i)+31,(32*i)+0) = South_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				South_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				South_buffer_available = South_buffer_available - 1;
			}
		    else if(East_to_North == 1){
				N_req_out = 1;
				N_data_out = East_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					East_input_buffer.range((32*i)+31,(32*i)+0) = East_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				East_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				East_buffer_available = East_buffer_available - 1;
			}
		    else if(Local_to_North == 1){
				N_req_out = 1;
				N_data_out = Local_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					Local_input_buffer.range((32*i)+31,(32*i)+0) = Local_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				Local_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				Local_buffer_available = Local_buffer_available - 1;
			}
	    	else{
				N_req_out = 0;
				N_data_out = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
			}
		}
		else{
			N_req_out = 0;
			N_data_out = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
		}
			
	    ////////////////////////// send flit to South output port //////////////////////////
		if(S_ack_out == 1){
	    	if(North_to_South == 1){
				S_req_out = 1;
				S_data_out = North_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					North_input_buffer.range((32*i)+31,(32*i)+0) = North_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				North_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				North_buffer_available = North_buffer_available - 1;
			}
	    	else if(West_to_South == 1){
				S_req_out = 1;
				S_data_out = West_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					West_input_buffer.range((32*i)+31,(32*i)+0) = West_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				West_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				West_buffer_available = West_buffer_available - 1;
			}
	    	else if(East_to_South == 1){
				S_req_out = 1;
				S_data_out = East_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					East_input_buffer.range((32*i)+31,(32*i)+0) = East_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				East_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				East_buffer_available = East_buffer_available - 1;
			}
	    	else if(Local_to_South == 1){
				S_req_out = 1;
				S_data_out = Local_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					Local_input_buffer.range((32*i)+31,(32*i)+0) = Local_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				Local_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				Local_buffer_available = Local_buffer_available - 1;
			}
	    	else{
				S_req_out = 0;
				S_data_out = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
			}
		}
		else{
			S_req_out = 0;
			S_data_out = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
		}

	    ////////////////////////// send flit to Local output port //////////////////////////
		if(L_ack_out == 1){
			if(North_to_Local == 1){
				L_req_out = 1;
				L_data_out = North_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					North_input_buffer.range((32*i)+31,(32*i)+0) = North_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				North_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				North_buffer_available = North_buffer_available - 1;
			}
		    else if(West_to_Local == 1){
				L_req_out = 1;
				L_data_out = West_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					West_input_buffer.range((32*i)+31,(32*i)+0) = West_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				West_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				West_buffer_available = West_buffer_available - 1;
			}
		    else if(South_to_Local == 1){
				L_req_out = 1;
				L_data_out = South_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					South_input_buffer.range((32*i)+31,(32*i)+0) = South_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				South_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				South_buffer_available = South_buffer_available - 1;
			}
	    	else if(East_to_Local == 1){
				L_req_out = 1;
				L_data_out = East_input_buffer.range(31,0);
				for ( int i = 0 ; i < 7 ; i++ ) //shift data in buffer
					East_input_buffer.range((32*i)+31,(32*i)+0) = East_input_buffer.range((32*(i+1))+31,(32*(i+1))+0);
				East_input_buffer.range(255,224) = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
				East_buffer_available = East_buffer_available - 1;
			}
	    	else{
				L_req_out = 0;
				L_data_out = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
			}
		}
		else{
			L_req_out = 0;
			L_data_out = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
		}
			
		////////////////////////// receive flit to input buffer //////////////////////////
		if(East_buffer_available < 8 && E_req_in == 1){
		    East_input_buffer.range((32*East_buffer_available)+31,(32*East_buffer_available)+0) = E_data_in.read();
			East_buffer_available = East_buffer_available + 1;
		}
		if(West_buffer_available < 8 && W_req_in == 1){
			West_input_buffer.range((32*West_buffer_available)+31,(32*West_buffer_available)+0) = W_data_in.read();
			West_buffer_available = West_buffer_available + 1;
		}
		if(North_buffer_available < 8 && N_req_in == 1){
			North_input_buffer.range((32*North_buffer_available)+31,(32*North_buffer_available)+0) = N_data_in.read();
			North_buffer_available = North_buffer_available + 1;
		}
        if(South_buffer_available < 8 && S_req_in == 1){
			South_input_buffer.range((32*South_buffer_available)+31,(32*South_buffer_available)+0) = S_data_in.read();
			South_buffer_available = South_buffer_available + 1;
		}
		if(Local_buffer_available < 8 && L_req_in == 1){
		    Local_input_buffer.range((32*Local_buffer_available)+31,(32*Local_buffer_available)+0) = L_data_in.read();
		    Local_buffer_available = Local_buffer_available + 1;
		}
			
		N_ack_in = (North_buffer_available < 8)? 1 : 0;
		W_ack_in = (West_buffer_available < 8) ? 1 : 0;
		S_ack_in = (South_buffer_available < 8)? 1 : 0;
		E_ack_in = (East_buffer_available < 8) ? 1 : 0;
		L_ack_in = (Local_buffer_available < 8)? 1 : 0;
	}
}

/*Load key mapping table from file*/
void Router::init()
{	
	// StringStream
	ifstream KeyMappingTable("/home/HardwareSecurity_1132/tsx1136010/HW03/PA3/key.txt", ios::in);
	string line, stemp = "", key;
	int node;
	getline(KeyMappingTable, line);

	pair<int,string> keyMap;
	while(1)
	{
		getline(KeyMappingTable, line);
		istringstream tline(line);
		tline >> stemp;
		if(stemp == ".")break;

		tline >> node >> stemp >> key;
		keyMap = make_pair(node, key);
		keyTable.push_back(keyMap);

		// Key string to sc_bv type: sc_bv<len>(key.c_str())
	}
}

sc_lv<30> Router::getKey(sc_lv<4> dst)
{
	/*Id computing = ( X + Y * NoC_size )*/
	sc_uint<32>Id;
	Id = sc_uint<32>(dst).range(3, 2) + sc_uint<32>(dst).range(1, 0) * NoC_size;

	/*Get dst's key*/
	sc_lv<30>key;
	for(int i = 0; i < keyTable.size(); i++)
		if(keyTable[ i ].first == int(Id)) key = sc_lv<30>(keyTable[ i ].second.c_str());

	return key;
}

/*Encrypt or Decrypt function*/
sc_lv<32> Router::xoring(sc_lv<32> input_buffer, sc_lv<30> key)
{
	// cout << "Ori: " << input_buffer.range(29, 0) << endl;
	// cout << "Key: " << key << endl;

	/*XORing*/
	for(int i = 29; i >= 0; i--)
	{
		if(key.range(i, i) == input_buffer.range(i, i))
			input_buffer.range(i, i) = 0;
		else input_buffer.range(i, i) = 1;
	}

	// cout << "Xor: " << input_buffer.range(29, 0) << endl;
	return input_buffer;
}

/*Attack function*/
sc_lv<32> Router::attack(sc_lv<32> input_buffer)
{	
	/*Victim node specification*/
	sc_uint<32>spec_pos[ 2 ];
	spec_pos[ 0 ] = 0, spec_pos[ 1 ] = 2;

	/*Malicious payload*/
	sc_lv<22>payload;
	payload = "000000000000000000000";

	/*Malicious accomplice*/
	sc_lv<2>mx, my;
	mx = "00", my = "01";

	if(lx == 1 && ly == 1)
	{
		/*Eavesdropping*/
		// cout << "From: [ " << sc_uint<32>(input_buffer).range(29, 28)
		//  << ", " << sc_uint<32>(input_buffer).range(27, 26) << " ] To: [ ";
		// cout << sc_uint<32>(input_buffer).range(25, 24) 
		// << ", " << sc_uint<32>(input_buffer).range(23, 22) << " ]\n";

		/*Data integrity attack*/
		if( (sc_uint<32>(input_buffer).range(25, 24) == spec_pos[ 0 ] &&
		sc_uint<32>(input_buffer).range(23, 22) == spec_pos[ 1 ]) )
		{
			#ifdef HEADER
			cout << "Original: " << sc_uint<32>(input_buffer).range(25, 24) 
			<< sc_uint<32>(input_buffer).range(23, 22) << endl;
			
			input_buffer.range(25, 24) = mx;
			input_buffer.range(23, 22) = my;

			cout << "Change: " << sc_uint<32>(input_buffer).range(25, 24) 
			<< sc_uint<32>(input_buffer).range(23, 22) << endl;
			#endif

			#ifdef DATA
			cout << "Original: " << sc_uint<32>(input_buffer).range(21, 0) << endl;
			cout << input_buffer.range(21, 0) << endl;

			input_buffer.range(21, 0) = payload;

			cout << "Change: " << sc_uint<32>(input_buffer).range(21, 0) << endl;
			cout << input_buffer.range(21, 0) << endl;
			#endif	
		}
	}
	return input_buffer;
}
