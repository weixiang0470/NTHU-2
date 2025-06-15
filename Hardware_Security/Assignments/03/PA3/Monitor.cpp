#include "Monitor.h"

void Monitor:: monitor() {
	if ( rst ) {
		x = 0;
	}
	else if ( data_valid ) {


		#ifdef fixed_DATA_TYPE
			cout << x << ": " << double(data_in.read())/1024 << "!" << endl;
			fout << x << ": " << double(data_in.read())/1024 << endl;
		#else
			cout << x << ": " << data_in.read() << "!" << endl;
			fout << x << ": " << data_in.read() << endl;
		#endif
		
		x = x + 1;
		if ( x == 10 )
			sc_stop();
	}
}
