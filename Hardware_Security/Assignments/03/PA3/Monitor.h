#include "systemc.h"
#include "define.h"
#include <iostream>
#include <fstream>

using namespace std;

SC_MODULE( Monitor ) {	
	sc_in_clk clk;
	sc_in < bool > rst;
	sc_in < DATA_TYPE > data_in;
	sc_in < bool > data_valid;
	
	void monitor();
	
	int x, cycle;
	ofstream fout; 
	
	SC_CTOR( Monitor )
	{
		x = 0;
		cycle = 0;
		
		SC_METHOD( monitor );
		sensitive << clk.pos();
		
		// avoid conducting SC_METHOD at time 0
		dont_initialize();	

		fout.open("/home/HardwareSecurity_1132/tsx1136010/HW03/PA3/export/HW3_result.txt", ios::out);
		
	}

	// destructor
	~Monitor() {
		fout.close();
	}
};

