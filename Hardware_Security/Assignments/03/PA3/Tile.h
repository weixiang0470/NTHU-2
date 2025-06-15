#ifndef DEFINE_TILE
#define DEFINE_TILE

#include "systemc.h"
#include "Processing_Element.h"
#include "NI.h"
#include "Router.h"
#include "config.h"
#include <iostream>

using namespace std;

SC_MODULE( Tile ) {
	
	sc_in < bool > clk;
	sc_in < bool > reset;
	
	sc_out < sc_lv<16> > P_data_out;
	sc_out < bool > P_out_valid;
	
	sc_in < sc_lv<16> > P_data_in;
	sc_in < bool > P_in_valid;
	sc_in < bool > load_done;
	
	sc_in  < sc_lv<FLIT_LENGTH> > N_data_in,  W_data_in,  S_data_in,  E_data_in;
	sc_out < sc_lv<FLIT_LENGTH> > N_data_out, W_data_out, S_data_out, E_data_out;
	
	sc_in < bool > N_req_in,  W_req_in,  S_req_in,  E_req_in;
	sc_in < bool > N_ack_out, W_ack_out, S_ack_out, E_ack_out;
	
	sc_out < bool > N_ack_in,  W_ack_in,  S_ack_in,  E_ack_in;
	sc_out < bool > N_req_out, W_req_out, S_req_out, E_req_out;
	
	// Wire in tile
	sc_signal < sc_lv<FLIT_LENGTH> > L_data_in, L_data_out;
	sc_signal < bool > L_req_in, L_ack_out, L_ack_in, L_req_out;
	
	sc_signal < sc_lv<DATA_LENGTH> > PE_data_in, PE_data_out;
	sc_signal < bool > PE_req_in, PE_ack_out;
	sc_signal < bool > PE_ack_in, PE_req_out;

	SC_HAS_PROCESS( Tile );
	
	private:
		int lx;
		int ly;
		int layer;
		Router m_router;
		Network_Interface m_ni;
		Processing_Element m_pe;
		
	public:
		Tile( sc_module_name name, int _lx, int _ly, int _layer): 
			sc_module( name ), lx( _lx ), ly( _ly ), layer( _layer ),
			m_router( "m_router", _lx, _ly ),
			m_ni( "m_ni", _lx, _ly ),
			m_pe( "m_pe", _lx, _ly, _layer )
		{
			
			m_router( clk, reset,
					  N_data_in,  W_data_in,  S_data_in,  E_data_in,  L_data_in,
					  N_data_out, W_data_out, S_data_out, E_data_out, L_data_out,
					  N_req_in,   W_req_in,   S_req_in,   E_req_in,   L_req_in,
					  N_ack_out,  W_ack_out,  S_ack_out,  E_ack_out,  L_ack_out, 
					  N_ack_in,   W_ack_in,   S_ack_in,   E_ack_in,   L_ack_in, 
					  N_req_out,  W_req_out,  S_req_out,  E_req_out,  L_req_out );
					
			m_ni( clk, reset, 
				  L_data_out, PE_data_out,
				  L_data_in, PE_data_in,
				  L_req_out, L_ack_in,  
				  L_ack_out,  L_req_in,
				  PE_req_out, PE_ack_in,
				  PE_ack_out, PE_req_in );
		
			m_pe( clk, reset,
				  P_data_out, P_out_valid, P_data_in, P_in_valid, load_done,
				  PE_data_in, PE_data_out,
				  PE_req_in, PE_ack_out,
				  PE_ack_in, PE_req_out );
		
		}
};

#endif

