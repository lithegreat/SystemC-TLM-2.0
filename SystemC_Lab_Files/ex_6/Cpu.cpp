#include "Cpu.h"
#include "IoModule.h"
#include "reporting.h"

using namespace sc_core;
using namespace tlm;

void Cpu::processor_thread(void) {

	while(true) {
		// ############# COMPLETE THE FOLLOWING SECTION ############# //
		// read new packet descriptor
		wait(packetReceived_interrupt.value_changed_event());
		while (packetReceived_interrupt.read() == true) {
			startTransaction(TLM_READ_COMMAND, 0x10000000, (unsigned char *) &m_packet_descriptor, sizeof(packet_descriptor));
		
		// ####################### UP TO HERE ####################### //

		// ############# COMPLETE THE FOLLOWING SECTION ############# //
		// Forward the packet descriptor to an arbitrary port
			soc_address_t output_address = 0x20000000 + (rand() % 4) * 0x10000000;
			startTransaction(TLM_WRITE_COMMAND, output_address, (unsigned char *) &m_packet_descriptor, sizeof(packet_descriptor));
		}
		// ####################### UP TO HERE ####################### //
	}
}

// ############# COMPLETE THE FOLLOWING SECTION ############# //
// startTransaction
void Cpu::startTransaction(tlm_command command, soc_address_t address, unsigned char *data, unsigned int dataSize) {
	while (true) {
		payload.set_command(command);
		payload.set_address(address);
		payload.set_data_ptr(data);
		payload.set_data_length(dataSize);

		tlm_phase phase = BEGIN_REQ;
		sc_time delay_time = SC_ZERO_TIME;
		tlm_sync_enum resp = initiator_socket->nb_transport_fw(payload, phase, delay_time);
		
		wait(transactionFinished_event);

		if (resp != TLM_UPDATED) {
			cout << "Error: " << "Transaction not updated correctly." << endl;
			exit(1);
		}

		if (phase != END_REQ) {
			cout << "Error: " << "Transaction phase not updated correctly." << endl;
			exit(1);
		}

		if (payload.get_response_status() == TLM_OK_RESPONSE || !payload.is_response_error()) {
			break;
		}
	}
	

}
// ####################### UP TO HERE ####################### //

// nb_transport_bw: implementation of the backward path callback
tlm_sync_enum Cpu::nb_transport_bw(tlm_generic_payload& transaction,
		tlm_phase& phase, sc_time& delay_time) {
	// ############# COMPLETE THE FOLLOWING SECTION ############# //

	// check whether the phase is set appropriately when nb_transport_bw is called.
	if (phase != BEGIN_RESP) {
		cout << "Error: " << "Phase not set correctly." << endl;
		exit(1);
	}
	// The bus transfer should take one bus cycle (CLK_CYCLE_BUS). Update the delay
	delay_time += sc_time(CLK_CYCLE_BUS);

	transactionFinished_event.notify(delay_time);
	phase = END_RESP;
	return TLM_COMPLETED;


	// ####################### UP TO HERE ####################### //
}

unsigned int Cpu::instances = 0;
