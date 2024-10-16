#ifndef __PRODUCER
#define __PRODUCER

// ############# COMPLETE THE FOLLOWING SECTION ############# //
// include the necessary header files

// ####################### UP TO HERE ####################### //

SC_MODULE(producer) {

public:
	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// declare the port of the module

	// ####################### UP TO HERE ####################### //

private:
	// event for communication between producer() and send_trigger()
	sc_event send_event;

private:
	// process declaration
	void producer_proc();
	void send_trigger();

public:
	// constructor
	SC_CTOR(producer) {
		// process registration
		SC_THREAD(producer_proc);
		SC_THREAD(send_trigger);
	}
};

#endif // __PRODUCER
