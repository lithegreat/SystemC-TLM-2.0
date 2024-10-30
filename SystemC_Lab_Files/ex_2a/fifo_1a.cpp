#include <iomanip>
#include "fifo_1a.h"

fifo_1a::fifo_1a(sc_module_name name, unsigned int fifo_size) : sc_module(name), fifo_size(fifo_size) {
	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// register processes
	SC_THREAD(write_fifo);
    sensitive << clk.pos();
    SC_THREAD(read_fifo);
    sensitive << clk.pos();
	// ####################### UP TO HERE ####################### //

	SC_METHOD(set_flags);
	sensitive << rd_ptr << wr_ptr;

	// initialization of member variables/ports
	fifo_data = new unsigned char[fifo_size];
	rd_ptr.write(0);
	wr_ptr.write(0);
	fill_level = 0;

	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// initialize output ports
	full.initialize(false);
    empty.initialize(true);
	d_out.initialize(-1);
	// ####################### UP TO HERE ####################### //
}

void fifo_1a::write_fifo() {
	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	while (true) {
			wait();
			if (wr_en.read() && !full.read()) {
				fifo_data[wr_ptr.read()] = d_in.read();
				wr_ptr.write((wr_ptr.read() + 1) % fifo_size);
				fill_level++;
				std::cout << "@" << sc_time_stamp() << " Written: " << d_in.read() << " at position " << wr_ptr.read() << std::endl;
			}
		}
	// ####################### UP TO HERE ####################### //
}

void fifo_1a::read_fifo() {
	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	while (true) {
			wait();
			if (rd_en.read() && !empty.read()) {
				d_out.write(fifo_data[rd_ptr.read()]);
				rd_ptr.write((rd_ptr.read() + 1) % fifo_size);
				fill_level--;
				std::cout << "@" << sc_time_stamp() << " Read: " << d_out.read() << " from position " << rd_ptr.read() << std::endl;
			}
		}
	// ####################### UP TO HERE ####################### //
}

void fifo_1a::set_flags() {
	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// set 'full' & 'empty' flags according to fill level
    full.write(fill_level == fifo_size);
    empty.write(fill_level == 0);
	// ####################### UP TO HERE ####################### //
}
