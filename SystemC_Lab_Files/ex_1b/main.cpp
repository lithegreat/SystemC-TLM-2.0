#include "stimulus.h"
#include "counter.h"
#include "bcd_decoder.h"

int sc_main(int argc, char *argv[]) {

	sc_signal<bool> clock, reset_n;
	sc_signal<unsigned short int> count_val;
	sc_signal<char> v_hi, v_lo;

	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// The top-level file main.cpp is partly provided. You need to instantiate and connect your counter
	// module here. Furthermore, you need to trace the signals as described before.

	stimulus stim("stim");
	stim.clk(clock);
	stim.rst_n(reset_n);

	counter cnt("cnt");
	cnt.clk(clock);
	cnt.rst_n(reset_n);
	cnt.cnt(count_val);

	bcd_decoder bcd("bcd");
	bcd.val(count_val);
	bcd.hi(v_hi);
	bcd.lo(v_lo);

	sc_trace_file *tf = sc_create_vcd_trace_file("traces");
	sc_trace(tf, clock, "clock");
	sc_trace(tf, reset_n, "reset_n");
	sc_trace(tf, count_val, "count_val");
	sc_trace(tf, v_hi, "v_hi");
	sc_trace(tf, v_lo, "v_lo");


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
