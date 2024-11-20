#include <iomanip>
#include "fifo_1b.h"

fifo_1b::fifo_1b(sc_module_name name, unsigned int fifo_size) : fifo_size(fifo_size) {
	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// register process
	SC_METHOD(read_write_fifo);
	sensitive << clk.pos();
	// ####################### UP TO HERE ####################### //

	// initialization of member variables/ports
	fifo_data = new unsigned char[fifo_size];
	rd_ptr.write(0);
	wr_ptr.write(0);
	fill_level = 0;

	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// initialize output ports
	full.initialize(false);
	valid.initialize(false);
	d_out.initialize(-1);
	// ####################### UP TO HERE ####################### //
}

void fifo_1b::read_write_fifo() {
	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	bool write = wr_en.read() && !full.read() && fill_level > 0;
	bool read = rd_en.read() && valid.read();
	bool write_through = wr_en.read() && fill_level == 0;
	bool update_out = rd_en.read() && valid.read() && (fill_level > 1 || (read && write && fill_level == 1));

	if (write_through) {
		d_out.write(d_in.read());
		valid.write(true);
		fill_level++;
		// cout << "@" << sc_time_stamp() << " Write through: " << (int)d_in.read() << endl;
	}
	if (write) {
		fifo_data[wr_ptr.read()] = d_in.read();
		wr_ptr.write((wr_ptr.read() + 1) % fifo_size);
		fill_level++;
		// cout << "@" << sc_time_stamp() << " Written: " << (int)d_in.read() << " at position " << wr_ptr.read() << endl;
	}
	if (read) {
		if (fill_level == 1) {
			fill_level = 0;
			valid.write(false);
		}
		else {
			rd_ptr.write((rd_ptr.read() + 1) % fifo_size);
			fill_level--;
		}
		// cout << "@" << sc_time_stamp() << " Read: " << (int)d_out.read() << " from position " << rd_ptr.read() << endl;
	}
	if (update_out) {
		d_out.write(fifo_data[rd_ptr.read()]);
		// cout << "@" << sc_time_stamp() << " Update out: " << (int)d_out.read() << " from position " << rd_ptr.read() << endl;
	}

	full.write(fill_level == (fifo_size + 1));
	// cout << "@" << sc_time_stamp() << " Fill level: " << fill_level << endl;

	// ####################### UP TO HERE ####################### //
}