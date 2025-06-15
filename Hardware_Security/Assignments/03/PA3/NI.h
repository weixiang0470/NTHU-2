#ifndef DEFINE_NI
#define DEFINE_NI

#include "systemc.h"
#include "config.h"
#include <iostream>
#include <sstream>

using namespace std;

SC_MODULE( Network_Interface ) {
	
	sc_in < bool > clk;
	sc_in < bool > reset;
	
	sc_in < sc_lv<FLIT_LENGTH> > Flit_in;
	sc_in < sc_lv<DATA_LENGTH> > Data_in;
	sc_out < sc_lv<FLIT_LENGTH> > Flit_out;
	sc_out < sc_lv<DATA_LENGTH> > Data_out;
	
	sc_in < bool > L_req_in, L_ack_out;
	sc_out < bool > L_ack_in, L_req_out;
	
	sc_in < bool > PE_req_in, PE_ack_out;
	sc_out < bool > PE_ack_in, PE_req_out;
	
	/////////////////////////////////////////////////
	sc_lv<FLIT_LENGTH> temp1;
	sc_lv<DATA_LENGTH> temp2;
	sc_lv< FLIT_LENGTH*8 > NI_PE_buffer;
	sc_lv< FLIT_LENGTH*8 > NI_router_buffer;
	sc_uint< 4 > NI_PE_available;
    sc_uint< 4 > NI_router_available;
	int state;
	
	/////////////////////////////////////////////////
	
	SC_HAS_PROCESS( Network_Interface );
	
	private:
		void init();
		void run();
		//current
		int lx;
		int ly;
		//destination
		int dx;
		int dy;
		
	public:
		Network_Interface( sc_module_name name, int _lx, int _ly ):
			sc_module( name ), lx( _lx ), ly( _ly )
		{
			init();
			SC_METHOD( run );
		    sensitive << clk.neg();
		}
};

#endif

