#include <iostream>
#include <iomanip>
#include "fifo_2.h"

fifo_2::fifo_2(sc_module_name name, unsigned int fifo_size) : fifo_size(fifo_size) {
	// initialization of member variables/ports
	fifo_data = new unsigned char[fifo_size];
	wr_ptr = 0;
	rd_ptr = 0;
	fill_level = 0;
}

bool fifo_2::write_fifo(unsigned char *data, unsigned int &count) {
	bool result = false;

	unsigned int len; // amount of data written
	unsigned char *ptr; // pointer where to put data
	ptr = data;
	sc_time delay;

	if(fill_level + (int)count > fifo_size) // not enough space for all data
		len = fifo_size - fill_level; 		// none or less data will be written
	else {
		len = count;
		result = true;
	}

	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// complete process
	delay = sc_time(100, SC_NS);
	for (unsigned int i = 0; i < len; i++){wait(delay);}
	// Handle wrap-around case
    if (wr_ptr + len > fifo_size) {
		unsigned int first_chunk = fifo_size - wr_ptr;
        // First chunk: write until end of buffer
        memcpy(fifo_data + wr_ptr, ptr, first_chunk);
        wr_ptr = 0;
        // Second chunk: write remaining at start
		unsigned int second_chunk = len - first_chunk;
		memcpy(fifo_data, ptr + first_chunk, second_chunk);
		wr_ptr = second_chunk;
    } else {
        // Simple case: write in one chunk
		memcpy(fifo_data + wr_ptr, ptr, len);
		wr_ptr = (wr_ptr + len) % fifo_size;
    }

	fill_level += len;
    count = len;
	// cout << std::setw(9) << sc_time_stamp() << ": '" << name() << "'\t" << (int)len << " words have been written: 0x ";
	// cout << hex;
	// for (unsigned int j = 0; j < len; j++) {
	// 	cout << std::setw(2) << std::setfill('0') << (int)*(ptr + j) << " ";
	// }
	// cout << endl;
	// cout << dec;

	
	// ####################### UP TO HERE ####################### //
	// if(fifo_size <= 50)
	// 	output_fifo_status();
	return result;
}

bool fifo_2::read_fifo(unsigned char *data, unsigned int &count) {
	bool result = false;

	unsigned int len;	// amount of data read
	unsigned char *ptr;	// pointer where to put data
	ptr = data;
	sc_time delay;

	if(fill_level < count)	// not enough data to read
		len = fill_level;	// none or less data will be read
	else {
		len = count;
		result = true;
	}

	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// complete process
	// delay is 100 ns
	delay = sc_time(100, SC_NS);
	for (unsigned int i = 0; i < len; i++){wait(delay);}
	// Handle wrap-around case 
    if (rd_ptr + len > fifo_size) {
        // First chunk: read until end of buffer
        unsigned int first_chunk = fifo_size - rd_ptr;
		memcpy(ptr, fifo_data + rd_ptr, first_chunk);
        
        // Second chunk: read remaining from start
        unsigned int second_chunk = len - first_chunk;
		memcpy(ptr + first_chunk, fifo_data, second_chunk);
		rd_ptr = second_chunk;
    } else {
        // Simple case: read in one chunk
        memcpy(ptr, fifo_data + rd_ptr, len);
		rd_ptr = (rd_ptr + len) % fifo_size;
    }

    fill_level -= len;
    count = len;
	// cout << std::setw(9) << sc_time_stamp() << ": '" << name() << "'\t" << (int)len << " words have been read: 0x ";
	// cout << hex;
	// for (unsigned int j = 0; j < len; j++) {
	// 	cout << std::setw(2) << std::setfill('0') << (int)*(ptr + j) << " ";
	// }
	// cout << endl;
	// cout << dec;
	// ####################### UP TO HERE ####################### //
	// if(fifo_size <= 50)
	// 	output_fifo_status();
	return result;
}

// helper function to output content of FIFO
void fifo_2::output_fifo_status() {
	cout << "\tCurrent status of '" << name() << "': write address: "
			<< wr_ptr << ", read address: " << rd_ptr
			<< ", fill level: " << fill_level << endl;
	cout << "\t";
	cout << hex; // switch to hexadecimal mode;
	if(fill_level == 0) {
		for(unsigned int i = 0; i < fifo_size; i++)
			cout << "-- ";
	}
	else if(fill_level == fifo_size) {
		for(unsigned int i = 0; i < fifo_size; i++)
			cout << std::setw(2) << std::setfill('0') << (int)*(fifo_data + i)
					<< " ";
	}
	else if(wr_ptr > rd_ptr) {
		for(unsigned int i = 0; i < fifo_size; i++) {
			if((i >= rd_ptr) && (i < wr_ptr)) {
				cout << std::setw(2) << std::setfill('0')
						<< (int)*(fifo_data + i) << " ";
			}
			else
				cout << "-- ";
		}
	}
	else if(wr_ptr < rd_ptr) {
		for(unsigned int i = 0; i < fifo_size; i++) {
			if((i >= rd_ptr) || (i < wr_ptr)) {
				cout << std::setw(2) << std::setfill('0')
						<< (int)*(fifo_data + i) << " ";
			}
			else
				cout << "-- ";
		}
	}
	cout << dec << std::setfill(' ') << endl << endl;
}
