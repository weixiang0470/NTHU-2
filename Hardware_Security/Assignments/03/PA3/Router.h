#ifndef DEFINE_ROUTER
#define DEFINE_ROUTER
#define ROUTER_DEFENCE	0
#define ROUTER_ATTACK	1
// #define HEADER			2
#define DATA			3

#include "systemc.h"
#include "config.h"
#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <utility>
#include <time.h>

using namespace std;

SC_MODULE( Router ) {
	
	sc_in < bool > clk;
	sc_in < bool > reset;
		
	sc_in  < sc_lv<FLIT_LENGTH> > N_data_in,  W_data_in,  S_data_in,  E_data_in, L_data_in;
	sc_out < sc_lv<FLIT_LENGTH> > N_data_out, W_data_out, S_data_out, E_data_out, L_data_out;
	
	sc_in < bool > N_req_in,  W_req_in,  S_req_in,  E_req_in,  L_req_in;
	sc_in < bool > N_ack_out, W_ack_out, S_ack_out, E_ack_out, L_ack_out;
	
	sc_out < bool > N_ack_in,  W_ack_in,  S_ack_in,  E_ack_in,  L_ack_in;
	sc_out < bool > N_req_out, W_req_out, S_req_out, E_req_out, L_req_out;
	
	/////////////////////////////////////////////////
	sc_lv< FLIT_LENGTH*8 > East_input_buffer; 
	sc_lv< FLIT_LENGTH*8 > West_input_buffer;
	sc_lv< FLIT_LENGTH*8 > North_input_buffer;
	sc_lv< FLIT_LENGTH*8 > South_input_buffer;
	sc_lv< FLIT_LENGTH*8 > Local_input_buffer;
	
    sc_uint< 4 > East_buffer_available;
	sc_uint< 4 > West_buffer_available;
	sc_uint< 4 > North_buffer_available;
	sc_uint< 4 > South_buffer_available;
	sc_uint< 4 > Local_buffer_available;
	
	bool East_to_West;
	bool East_to_North;
	bool East_to_South;
	bool East_to_Local;
	bool West_to_East;
	bool West_to_North;
	bool West_to_South;
	bool West_to_Local;
	bool North_to_West;
	bool North_to_East;
	bool North_to_South;
	bool North_to_Local;
	bool South_to_West;
	bool South_to_North;
	bool South_to_East;
	bool South_to_Local;
	bool Local_to_West;
	bool Local_to_North;
	bool Local_to_South;
	bool Local_to_East;
	
	sc_lv<FLIT_LENGTH> temp;
	
	

	/////////////////////////////////////////////////
	
	SC_HAS_PROCESS( Router );
	
	private:
	    void run();
		int lx;
		int ly;

		/*Key Storation*/
		vector< pair<int,string> >keyTable;
		sc_lv<30> tmpKey;

		/*Key Loading*/
		void init();

		/*Encryption / Decryption Method*/
		sc_lv<30> getKey(sc_lv<4> dst);
		sc_lv<32> xoring(sc_lv<32> input_buffer, sc_lv<30> key);
		
		/*Attack Function*/
		sc_lv<32> attack(sc_lv<32> input_buffer);

	public:
		Router( sc_module_name name, int _lx, int _ly ): 
			sc_module( name ), lx( _lx ), ly( _ly ) 
		{
			init();
			SC_METHOD( run );
		    sensitive << clk.neg();
		}
};

#endif

