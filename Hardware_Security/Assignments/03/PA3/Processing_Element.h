#ifndef DEFINE_PE
#define DEFINE_PE

#include "systemc.h"
#include "config.h"
#include <iostream>
#include <string.h>

using namespace std;

SC_MODULE( Processing_Element ) {
	
	sc_in < bool > clk;
	sc_in < bool > reset; 
	
	sc_out < sc_lv<16> > P_data_out;
	sc_out < bool > P_out_valid;
	
	sc_in < sc_lv<16> > P_data_in;
	sc_in < bool > P_in_valid;
	sc_in < bool > load_done;
	
	sc_in < sc_lv<DATA_LENGTH> > Data_in;
	sc_out < sc_lv<DATA_LENGTH> > Data_out;
	
	sc_in < bool > PE_req_in, PE_ack_out;
	sc_out < bool > PE_ack_in, PE_req_out;
	
	/////////////////////////////////////////////////
	sc_lv<DATA_LENGTH> temp;
	sc_int<16> ifmap[3456];//max: 6*24*24
	sc_int<16> weight[30840];//max: 16*4*4*120+120
	sc_int<16> ofmap[3456];//max: 6*24*24
	// sc_int<16> ifmap[30];//max: 6*24*24
	// sc_int<16> weight[30];//max: 16*4*4*120+120
	// sc_int<16> ofmap[30];//max: 6*24*24

	sc_uint<16> count;
	int state;
	/////////////////////////////////////////////////
	
	SC_HAS_PROCESS( Processing_Element );
	
	private:
		void run();
		int lx;
		int ly;
		int layer;
		
	public:
		Processing_Element( sc_module_name name, int _lx, int _ly, int _layer ): 
			sc_module( name ), lx( _lx ), ly( _ly ), layer( _layer )
		{
			SC_METHOD( run );
			sensitive << clk.neg();
		}
};

#endif

