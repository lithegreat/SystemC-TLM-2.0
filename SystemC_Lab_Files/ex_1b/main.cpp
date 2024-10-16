#include "stimulus.h"
#include "counter.h"
#include "bcd_decoder.h"

int sc_main(int argc, char *argv[]) {

	sc_signal<bool> clock, reset_n;
	sc_signal<unsigned short int> count_val;
	sc_signal<char> v_hi, v_lo;

	// ############# COMPLETE THE FOLLOWING SECTION ############# //

	// ####################### UP TO HERE ####################### //

	int n_cycles;
	if(argc != 2) {
		cout << "default n_cycles = 2000" << endl;
		n_cycles = 2000;
	}
	else {
		n_cycles = atoi(argv[1]);
		cout << "n_cycles = " << n_cycles << endl;
	}

	sc_start(n_cycles, SC_NS);

	sc_close_vcd_trace_file(tf);

	return 0;
}
