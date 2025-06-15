#include "systemc.h"
#include "Tile.h"
#include <iostream>
#include "config.h"
#include "define.h"

using namespace std;

SC_MODULE( LeNet ) {	
	sc_in_clk clk;
	sc_in < bool > reset;
	
	sc_out < bool > rom_rd;
	sc_out < sc_uint<16> > rom_addr;
	sc_in < DATA_TYPE > rom_data_out;
	
	sc_out < bool > ram_wr;
	sc_out < sc_uint<16> > ram_addr;
	sc_in < DATA_TYPE > ram_data_out;
	sc_out < DATA_TYPE > ram_data_in;
	
	sc_out < DATA_TYPE > result;
	sc_out < bool > output_valid;
	
	sc_uint<16> count;
	sc_int<16> ifmap[784];
	sc_int<16> weight[156];
	sc_int<16> ofmap[3456];
	// sc_int<16> ifmap[30];
	// sc_int<16> weight[30];
	// sc_int<16> ofmap[30];	
	int state;
	
	////////////////////////// PE to global controller ( LeNet Block ) //////////////////////////
	sc_signal < sc_lv<16> > 	P_0_0_dataout, P_1_0_dataout, P_2_0_dataout, 
							    P_0_1_dataout, P_1_1_dataout, P_2_1_dataout,
							    P_0_2_dataout, P_1_2_dataout, P_2_2_dataout;
										
	////////////////////////// global controller ( LeNet Block ) to PE //////////////////////////								
	sc_signal < sc_lv<16> > 	P_0_0_datain, P_1_0_datain, P_2_0_datain, 
								P_0_1_datain, P_1_1_datain, P_2_1_datain,
								P_0_2_datain, P_1_2_datain, P_2_2_datain;
    
	sc_signal < bool > 	P_0_0_in_valid, P_1_0_in_valid, P_2_0_in_valid, 
						P_0_1_in_valid, P_1_1_in_valid, P_2_1_in_valid,
						P_0_2_in_valid, P_1_2_in_valid, P_2_2_in_valid;
	
	sc_signal < bool > 	P_0_0_out_valid, P_1_0_out_valid, P_2_0_out_valid, 
						P_0_1_out_valid, P_1_1_out_valid, P_2_1_out_valid,
						P_0_2_out_valid, P_1_2_out_valid, P_2_2_out_valid;
	sc_signal < bool >  load_done;
									
	sc_signal < sc_lv<FLIT_LENGTH> >	N_0_0_dataout, W_0_0_dataout, S_0_0_dataout, E_0_0_dataout, 
										N_1_0_dataout, W_1_0_dataout, S_1_0_dataout, E_1_0_dataout, 
										N_2_0_dataout, W_2_0_dataout, S_2_0_dataout, E_2_0_dataout, 
										N_0_1_dataout, W_0_1_dataout, S_0_1_dataout, E_0_1_dataout, 
										N_1_1_dataout, W_1_1_dataout, S_1_1_dataout, E_1_1_dataout, 
										N_2_1_dataout, W_2_1_dataout, S_2_1_dataout, E_2_1_dataout, 
										N_0_2_dataout, W_0_2_dataout, S_0_2_dataout, E_0_2_dataout, 
										N_1_2_dataout, W_1_2_dataout, S_1_2_dataout, E_1_2_dataout, 
										N_2_2_dataout, W_2_2_dataout, S_2_2_dataout, E_2_2_dataout; 

	sc_signal < bool >	N_0_0_ackin, W_0_0_ackin, S_0_0_ackin, E_0_0_ackin, 
						N_1_0_ackin, W_1_0_ackin, S_1_0_ackin, E_1_0_ackin, 
						N_2_0_ackin, W_2_0_ackin, S_2_0_ackin, E_2_0_ackin, 
						N_0_1_ackin, W_0_1_ackin, S_0_1_ackin, E_0_1_ackin, 
						N_1_1_ackin, W_1_1_ackin, S_1_1_ackin, E_1_1_ackin, 
						N_2_1_ackin, W_2_1_ackin, S_2_1_ackin, E_2_1_ackin, 
						N_0_2_ackin, W_0_2_ackin, S_0_2_ackin, E_0_2_ackin, 
						N_1_2_ackin, W_1_2_ackin, S_1_2_ackin, E_1_2_ackin, 
						N_2_2_ackin, W_2_2_ackin, S_2_2_ackin, E_2_2_ackin; 

	sc_signal < bool >	N_0_0_reqout, W_0_0_reqout, S_0_0_reqout, E_0_0_reqout, 
						N_1_0_reqout, W_1_0_reqout, S_1_0_reqout, E_1_0_reqout, 
						N_2_0_reqout, W_2_0_reqout, S_2_0_reqout, E_2_0_reqout, 
						N_0_1_reqout, W_0_1_reqout, S_0_1_reqout, E_0_1_reqout, 
						N_1_1_reqout, W_1_1_reqout, S_1_1_reqout, E_1_1_reqout, 
						N_2_1_reqout, W_2_1_reqout, S_2_1_reqout, E_2_1_reqout, 
						N_0_2_reqout, W_0_2_reqout, S_0_2_reqout, E_0_2_reqout, 
						N_1_2_reqout, W_1_2_reqout, S_1_2_reqout, E_1_2_reqout, 
						N_2_2_reqout, W_2_2_reqout, S_2_2_reqout, E_2_2_reqout;
	
	Tile	m_tile_0_0, m_tile_1_0, m_tile_2_0,
			m_tile_0_1, m_tile_1_1, m_tile_2_1,
			m_tile_0_2, m_tile_1_2, m_tile_2_2;

	void run();
	
	SC_CTOR( LeNet ):
		m_tile_0_0( "m_tile_0_0", 0, 0, 1 ), m_tile_1_0 ( "m_tile_1_0", 1, 0, 2 ), m_tile_2_0 ( "m_tile_2_0", 2, 0, 3 ),
		m_tile_0_1( "m_tile_0_1", 0, 1, 4 ), m_tile_1_1 ( "m_tile_1_1", 1, 1, 5 ), m_tile_2_1 ( "m_tile_2_1", 2, 1, 6 ),
		m_tile_0_2( "m_tile_0_2", 0, 2, 7 ), m_tile_1_2 ( "m_tile_1_2", 1, 2, 0 ), m_tile_2_2 ( "m_tile_2_2", 2, 2, 0 )
	{
		m_tile_0_0 (	clk, reset, P_0_0_dataout, P_0_0_out_valid, P_0_0_datain, P_0_0_in_valid, load_done,
						S_0_2_dataout, E_2_0_dataout, N_0_1_dataout, W_1_0_dataout, 
						N_0_0_dataout, W_0_0_dataout, S_0_0_dataout, E_0_0_dataout, 
						S_0_2_reqout, E_2_0_reqout, N_0_1_reqout, W_1_0_reqout, 
						S_0_2_ackin, E_2_0_ackin, N_0_1_ackin, W_1_0_ackin, 
						N_0_0_ackin, W_0_0_ackin, S_0_0_ackin, E_0_0_ackin, 
						N_0_0_reqout, W_0_0_reqout, S_0_0_reqout, E_0_0_reqout );

		m_tile_1_0 (	clk, reset, P_1_0_dataout, P_1_0_out_valid, P_1_0_datain, P_1_0_in_valid, load_done,
						S_1_2_dataout, E_0_0_dataout, N_1_1_dataout, W_2_0_dataout, 
						N_1_0_dataout, W_1_0_dataout, S_1_0_dataout, E_1_0_dataout, 
						S_1_2_reqout, E_0_0_reqout, N_1_1_reqout, W_2_0_reqout, 
						S_1_2_ackin, E_0_0_ackin, N_1_1_ackin, W_2_0_ackin, 
						N_1_0_ackin, W_1_0_ackin, S_1_0_ackin, E_1_0_ackin, 
						N_1_0_reqout, W_1_0_reqout, S_1_0_reqout, E_1_0_reqout );

		m_tile_2_0 (	clk, reset, P_2_0_dataout, P_2_0_out_valid, P_2_0_datain, P_2_0_in_valid, load_done,
						S_2_2_dataout, E_1_0_dataout, N_2_1_dataout, W_0_0_dataout, 
						N_2_0_dataout, W_2_0_dataout, S_2_0_dataout, E_2_0_dataout, 
						S_2_2_reqout, E_1_0_reqout, N_2_1_reqout, W_0_0_reqout, 
						S_2_2_ackin, E_1_0_ackin, N_2_1_ackin, W_0_0_ackin, 
						N_2_0_ackin, W_2_0_ackin, S_2_0_ackin, E_2_0_ackin, 
						N_2_0_reqout, W_2_0_reqout, S_2_0_reqout, E_2_0_reqout );

		m_tile_0_1 (	clk, reset, P_0_1_dataout, P_0_1_out_valid, P_0_1_datain, P_0_1_in_valid, load_done,
						S_0_0_dataout, E_2_1_dataout, N_0_2_dataout, W_1_1_dataout, 
						N_0_1_dataout, W_0_1_dataout, S_0_1_dataout, E_0_1_dataout, 
						S_0_0_reqout, E_2_1_reqout, N_0_2_reqout, W_1_1_reqout, 
						S_0_0_ackin, E_2_1_ackin, N_0_2_ackin, W_1_1_ackin, 
						N_0_1_ackin, W_0_1_ackin, S_0_1_ackin, E_0_1_ackin,
						N_0_1_reqout, W_0_1_reqout, S_0_1_reqout, E_0_1_reqout );

		m_tile_1_1 (	clk, reset, P_1_1_dataout, P_1_1_out_valid, P_1_1_datain, P_1_1_in_valid, load_done,
						S_1_0_dataout, E_0_1_dataout, N_1_2_dataout, W_2_1_dataout, 
						N_1_1_dataout, W_1_1_dataout, S_1_1_dataout, E_1_1_dataout, 
						S_1_0_reqout, E_0_1_reqout, N_1_2_reqout, W_2_1_reqout, 
						S_1_0_ackin, E_0_1_ackin, N_1_2_ackin, W_2_1_ackin, 
						N_1_1_ackin, W_1_1_ackin, S_1_1_ackin, E_1_1_ackin, 
						N_1_1_reqout, W_1_1_reqout, S_1_1_reqout, E_1_1_reqout );

		m_tile_2_1 (	clk, reset, P_2_1_dataout, P_2_1_out_valid, P_2_1_datain, P_2_1_in_valid, load_done,
						S_2_0_dataout, E_1_1_dataout, N_2_2_dataout, W_0_1_dataout, 
						N_2_1_dataout, W_2_1_dataout, S_2_1_dataout, E_2_1_dataout, 
						S_2_0_reqout, E_1_1_reqout, N_2_2_reqout, W_0_1_reqout, 
						S_2_0_ackin, E_1_1_ackin, N_2_2_ackin, W_0_1_ackin, 
						N_2_1_ackin, W_2_1_ackin, S_2_1_ackin, E_2_1_ackin, 
						N_2_1_reqout, W_2_1_reqout, S_2_1_reqout, E_2_1_reqout );

		m_tile_0_2 (	clk, reset, P_0_2_dataout, P_0_2_out_valid, P_0_2_datain, P_0_2_in_valid, load_done,
						S_0_1_dataout, E_2_2_dataout, N_0_0_dataout, W_1_2_dataout, 
						N_0_2_dataout, W_0_2_dataout, S_0_2_dataout, E_0_2_dataout, 
						S_0_1_reqout, E_2_2_reqout, N_0_0_reqout, W_1_2_reqout, 
						S_0_1_ackin, E_2_2_ackin, N_0_0_ackin, W_1_2_ackin, 
						N_0_2_ackin, W_0_2_ackin, S_0_2_ackin, E_0_2_ackin, 
						N_0_2_reqout, W_0_2_reqout, S_0_2_reqout, E_0_2_reqout );

		m_tile_1_2 (	clk, reset, P_1_2_dataout, P_1_2_out_valid, P_1_2_datain, P_1_2_in_valid, load_done,
						S_1_1_dataout, E_0_2_dataout, N_1_0_dataout, W_2_2_dataout, 
						N_1_2_dataout, W_1_2_dataout, S_1_2_dataout, E_1_2_dataout, 
						S_1_1_reqout, E_0_2_reqout, N_1_0_reqout, W_2_2_reqout, 
						S_1_1_ackin, E_0_2_ackin, N_1_0_ackin, W_2_2_ackin, 
						N_1_2_ackin, W_1_2_ackin, S_1_2_ackin, E_1_2_ackin, 
						N_1_2_reqout, W_1_2_reqout, S_1_2_reqout, E_1_2_reqout );

		m_tile_2_2 (	clk, reset, P_2_2_dataout, P_2_2_out_valid, P_2_2_datain, P_2_2_in_valid, load_done,
						S_2_1_dataout, E_1_2_dataout, N_2_0_dataout, W_0_2_dataout, 
						N_2_2_dataout, W_2_2_dataout, S_2_2_dataout, E_2_2_dataout, 
						S_2_1_reqout, E_1_2_reqout, N_2_0_reqout, W_0_2_reqout, 
						S_2_1_ackin, E_1_2_ackin, N_2_0_ackin, W_0_2_ackin, 
						N_2_2_ackin, W_2_2_ackin, S_2_2_ackin, E_2_2_ackin, 
						N_2_2_reqout, W_2_2_reqout, S_2_2_reqout, E_2_2_reqout );

		SC_METHOD( run );
		sensitive << clk.neg();

	}
	/*SC_CTOR( LeNet ) :
		m_tile_0_0( "m_tile_0_0", 0, 0, 1 ), m_tile_1_0 ( "m_tile_1_0", 1, 0, 2 ), m_tile_2_0 ( "m_tile_2_0", 2, 0, 3 ),
		m_tile_0_1( "m_tile_0_1", 0, 1, 4 ), m_tile_1_1 ( "m_tile_1_1", 1, 1, 5 ), m_tile_2_1 ( "m_tile_2_1", 2, 1, 6 ),
		m_tile_0_2( "m_tile_0_2", 0, 2, 7 ), m_tile_1_2 ( "m_tile_1_2", 1, 2, 0 ), m_tile_2_2 ( "m_tile_2_2", 2, 2, 0 ) 
	{
		m_tile_0_0 (	clk, reset, P_0_0_dataout, P_0_0_out_valid, P_0_0_datain, P_0_0_in_valid, load_done,
						S_0_2_dataout, E_2_0_dataout, N_0_1_dataout, W_1_0_dataout, 
						N_0_0_dataout, W_0_0_dataout, S_0_0_dataout, E_0_0_dataout, 
						S_0_2_reqout, E_2_0_reqout, N_0_1_reqout, W_1_0_reqout, 
						S_0_2_ackin, E_2_0_ackin, N_0_1_ackin, W_1_0_ackin, 
						N_0_0_ackin, W_0_0_ackin, S_0_0_ackin, E_0_0_ackin, 
						N_0_0_reqout, W_0_0_reqout, S_0_0_reqout, E_0_0_reqout );

		m_tile_1_0 (	clk, reset, P_1_0_dataout, P_1_0_out_valid, P_1_0_datain, P_1_0_in_valid, load_done,
						S_1_2_dataout, E_0_0_dataout, N_1_1_dataout, W_2_0_dataout, 
						N_1_0_dataout, W_1_0_dataout, S_1_0_dataout, E_1_0_dataout, 
						S_1_2_reqout, E_0_0_reqout, N_1_1_reqout, W_2_0_reqout, 
						S_1_2_ackin, E_0_0_ackin, N_1_1_ackin, W_2_0_ackin, 
						N_1_0_ackin, W_1_0_ackin, S_1_0_ackin, E_1_0_ackin, 
						N_1_0_reqout, W_1_0_reqout, S_1_0_reqout, E_1_0_reqout );

		m_tile_2_0 (	clk, reset, P_2_0_dataout, P_2_0_out_valid, P_2_0_datain, P_2_0_in_valid, load_done,
						S_2_2_dataout, E_1_0_dataout, N_2_1_dataout, W_0_0_dataout, 
						N_2_0_dataout, W_2_0_dataout, S_2_0_dataout, E_2_0_dataout, 
						S_2_2_reqout, E_1_0_reqout, N_2_1_reqout, W_0_0_reqout, 
						S_2_2_ackin, E_1_0_ackin, N_2_1_ackin, W_0_0_ackin, 
						N_2_0_ackin, W_2_0_ackin, S_2_0_ackin, E_2_0_ackin, 
						N_2_0_reqout, W_2_0_reqout, S_2_0_reqout, E_2_0_reqout );

		m_tile_0_1 (	clk, reset, P_0_1_dataout, P_0_1_out_valid, P_0_1_datain, P_0_1_in_valid, load_done,
						S_0_0_dataout, E_2_1_dataout, N_0_2_dataout, W_1_1_dataout, 
						N_0_1_dataout, W_0_1_dataout, S_0_1_dataout, E_0_1_dataout, 
						S_0_0_reqout, E_2_1_reqout, N_0_2_reqout, W_1_1_reqout, 
						S_0_0_ackin, E_2_1_ackin, N_0_2_ackin, W_1_1_ackin, 
						N_0_1_ackin, W_0_1_ackin, S_0_1_ackin, E_0_1_ackin,
						N_0_1_reqout, W_0_1_reqout, S_0_1_reqout, E_0_1_reqout );

		m_tile_1_1 (	clk, reset, P_1_1_dataout, P_1_1_out_valid, P_1_1_datain, P_1_1_in_valid, load_done,
						S_1_0_dataout, E_0_1_dataout, N_1_2_dataout, W_2_1_dataout, 
						N_1_1_dataout, W_1_1_dataout, S_1_1_dataout, E_1_1_dataout, 
						S_1_0_reqout, E_0_1_reqout, N_1_2_reqout, W_2_1_reqout, 
						S_1_0_ackin, E_0_1_ackin, N_1_2_ackin, W_2_1_ackin, 
						N_1_1_ackin, W_1_1_ackin, S_1_1_ackin, E_1_1_ackin, 
						N_1_1_reqout, W_1_1_reqout, S_1_1_reqout, E_1_1_reqout );

		m_tile_2_1 (	clk, reset, P_2_1_dataout, P_2_1_out_valid, P_2_1_datain, P_2_1_in_valid, load_done,
						S_2_0_dataout, E_1_1_dataout, N_2_2_dataout, W_0_1_dataout, 
						N_2_1_dataout, W_2_1_dataout, S_2_1_dataout, E_2_1_dataout, 
						S_2_0_reqout, E_1_1_reqout, N_2_2_reqout, W_0_1_reqout, 
						S_2_0_ackin, E_1_1_ackin, N_2_2_ackin, W_0_1_ackin, 
						N_2_1_ackin, W_2_1_ackin, S_2_1_ackin, E_2_1_ackin, 
						N_2_1_reqout, W_2_1_reqout, S_2_1_reqout, E_2_1_reqout );

		m_tile_0_2 (	clk, reset, P_0_2_dataout, P_0_2_out_valid, P_0_2_datain, P_0_2_in_valid, load_done,
						S_0_1_dataout, E_2_2_dataout, N_0_0_dataout, W_1_2_dataout, 
						N_0_2_dataout, W_0_2_dataout, S_0_2_dataout, E_0_2_dataout, 
						S_0_1_reqout, E_2_2_reqout, N_0_0_reqout, W_1_2_reqout, 
						S_0_1_ackin, E_2_2_ackin, N_0_0_ackin, W_1_2_ackin, 
						N_0_2_ackin, W_0_2_ackin, S_0_2_ackin, E_0_2_ackin, 
						N_0_2_reqout, W_0_2_reqout, S_0_2_reqout, E_0_2_reqout );

		m_tile_1_2 (	clk, reset, P_1_2_dataout, P_1_2_out_valid, P_1_2_datain, P_1_2_in_valid, load_done,
						S_1_1_dataout, E_0_2_dataout, N_1_0_dataout, W_2_2_dataout, 
						N_1_2_dataout, W_1_2_dataout, S_1_2_dataout, E_1_2_dataout, 
						S_1_1_reqout, E_0_2_reqout, N_1_0_reqout, W_2_2_reqout, 
						S_1_1_ackin, E_0_2_ackin, N_1_0_ackin, W_2_2_ackin, 
						N_1_2_ackin, W_1_2_ackin, S_1_2_ackin, E_1_2_ackin, 
						N_1_2_reqout, W_1_2_reqout, S_1_2_reqout, E_1_2_reqout );

		m_tile_2_2 (	clk, reset, P_2_2_dataout, P_2_2_out_valid, P_2_2_datain, P_2_2_in_valid, load_done,
						S_2_1_dataout, E_1_2_dataout, N_2_0_dataout, W_0_2_dataout, 
						N_2_2_dataout, W_2_2_dataout, S_2_2_dataout, E_2_2_dataout, 
						S_2_1_reqout, E_1_2_reqout, N_2_0_reqout, W_0_2_reqout, 
						S_2_1_ackin, E_1_2_ackin, N_2_0_ackin, W_0_2_ackin, 
						N_2_2_ackin, W_2_2_ackin, S_2_2_ackin, E_2_2_ackin, 
						N_2_2_reqout, W_2_2_reqout, S_2_2_reqout, E_2_2_reqout );
						
		SC_METHOD( run );
		sensitive << clk.neg();
    }*/
};

