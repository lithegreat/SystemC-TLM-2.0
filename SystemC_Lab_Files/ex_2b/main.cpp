#include "consumer.h"
#include "producer.h"
#include "fifo_1b.h"

int sc_main(int argc, char *argv[]) {
	// the following instruction generates a clock signal with clock named
	// "clock" with a period of 100 ns, a duty cycle of 50%, and a falling edge
	// after 50 ns
	sc_clock clk("clock", 100, SC_NS, 0.5, 50, SC_NS, false);

	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// fill in the required commands to instantiate and connect producer, fifo,
	// and consumer
	sc_signal<unsigned char> d_in;
	sc_signal<unsigned char> d_out;
	sc_signal<bool> wr_en;
	sc_signal<bool> rd_en;
	sc_signal<bool> full;
	sc_signal<bool> valid;

    // Instantiate modules
    producer prod("Producer");
    consumer cons("Consumer");
    fifo_1b fifo("FIFO", 5);

    // Connect ports
	prod.clk(clk);
	prod.d_out(d_in);
	prod.wr_en(wr_en);
	prod.full(full);

	cons.clk(clk);
	cons.d_in(d_out);
	cons.rd_en(rd_en);
	cons.valid(valid);

	fifo.clk(clk);
	fifo.d_in(d_in);
	fifo.wr_en(wr_en);
	fifo.full(full);
	fifo.d_out(d_out);
	fifo.rd_en(rd_en);
	fifo.valid(valid);
	// ####################### UP TO HERE ####################### //

	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// fill in code to generate traces that can be used to observe the
	// functionality of the model with the waveform viewer gtkwave
	sc_trace_file *tf = sc_create_vcd_trace_file("fifo_1b");
	sc_trace(tf, clk, "clk");
	sc_trace(tf, rd_en, "rd_en");
	sc_trace(tf, cons.fetch, "fetch");
	sc_trace(tf, valid, "fifo_valid");
	sc_trace(tf, fifo.fill_level, "fifo_fill_level");
	sc_trace(tf, full, "fifo_full");
	sc_trace(tf, d_in, "fifo_in");
	sc_trace(tf, d_out, "fifo_out");
	sc_trace(tf, prod.send, "send");
	sc_trace(tf, wr_en, "wr_en");	
	// ####################### UP TO HERE ####################### //

	sc_time sim_dur = sc_time(5000, SC_NS);
	if(argc != 2) {
		cout << "Default simulation time = " << sim_dur << endl;
	}
	else {
		sim_dur = sc_time(atoi(argv[1]), SC_NS);
		cout << "Simulation time = " << sim_dur << endl;
	}

	// start simulation
	sc_start(sim_dur);

	sc_close_vcd_trace_file(tf);

	return 0;
}
