#include "Cpu.h"
#include "IoModule.h"
#include "reporting.h"

using namespace sc_core;
using namespace tlm;

void Cpu::processor_thread(void) {

	while(true) {
		// ############# COMPLETE THE FOLLOWING SECTION ############# //
		// read new packet descriptor

		// Wait for packet received interrupt
        while (!packetReceived_interrupt.read()) {
            wait(packetReceived_interrupt.value_changed_event());
        }

        if (do_logging & LOG_CPU) {
            REPORT_INFO("Cpu::processor_thread", "Packet received interrupt detected.");
        }

        // Read packet descriptor from processor queue
        soc_address_t address = ADDRESS_PROCESSOR_QUEUE;
        unsigned char* data_ptr = reinterpret_cast<unsigned char*>(&m_packet_descriptor);
        unsigned int dataSize = sizeof(m_packet_descriptor);

        startTransaction(TLM_READ_COMMAND, address, data_ptr, dataSize);

        if (payload.get_response_status() != TLM_OK_RESPONSE) {
            if (do_logging & LOG_CPU) {
                REPORT_WARNING("Cpu::processor_thread", "Read transaction failed.");
            }
            continue; // Retry
        }

		// ####################### UP TO HERE ####################### //

		// ############# COMPLETE THE FOLLOWING SECTION ############# //
		// Forward the packet descriptor to an arbitrary port

		// Randomly select an output queue
        unsigned int rand_port = rand() % nMacs;
        soc_address_t output_queue_address = ADDRESS_OUTPUT_QUEUE(rand_port);

        if (do_logging & LOG_CPU) {
            std::ostringstream oss;
            oss << "Forwarding packet to output queue " << rand_port;
            REPORT_INFO("Cpu::processor_thread", oss.str());
        }

        // Write packet descriptor to output queue
        startTransaction(TLM_WRITE_COMMAND, output_queue_address, data_ptr, dataSize);

        if (payload.get_response_status() != TLM_OK_RESPONSE) {
            if (do_logging & LOG_CPU) {
                REPORT_WARNING("Cpu::processor_thread", "Write transaction failed.");
            }
            continue; // Retry
        }

        if (do_logging & LOG_CPU) {
            REPORT_INFO("Cpu::processor_thread", "Packet descriptor forwarded successfully.");
        }
		// ####################### UP TO HERE ####################### //
	}
}

// ############# COMPLETE THE FOLLOWING SECTION ############# //
// startTransaction

void Cpu::startTransaction(tlm_command command, soc_address_t address,
    unsigned char *data, unsigned int dataSize) {

    // Setup payload
    payload.set_command(command);
    payload.set_address(address);
    payload.set_data_ptr(data);
    payload.set_data_length(dataSize);
    payload.set_streaming_width(dataSize);
    payload.set_byte_enable_ptr(0);
    payload.set_dmi_allowed(false);
    payload.set_response_status(TLM_INCOMPLETE_RESPONSE);

    tlm_phase phase = BEGIN_REQ;
    sc_time delay = SC_ZERO_TIME;

    // Call nb_transport_fw
    tlm_sync_enum result = initiator_socket->nb_transport_fw(payload, phase, delay);

    if (do_logging & LOG_CPU) {
        REPORT_INFO("Cpu::startTransaction", "nb_transport_fw called.");
    }

    // Check return value and phase
    if (result != TLM_ACCEPTED && result != TLM_UPDATED) {
        REPORT_FATAL("Cpu::startTransaction", "Unexpected return value from nb_transport_fw");
    }

    if (phase != END_REQ && phase != BEGIN_REQ) {
        REPORT_FATAL("Cpu::startTransaction", "Unexpected phase after nb_transport_fw");
    }

    // Wait for delay
    wait(delay);

    // Wait for transaction to complete
    wait(transactionFinished_event);
}

// ####################### UP TO HERE ####################### //

// nb_transport_bw: implementation of the backward path callback
tlm_sync_enum Cpu::nb_transport_bw(tlm_generic_payload& transaction,
		tlm_phase& phase, sc_time& delay_time) {
	// ############# COMPLETE THE FOLLOWING SECTION ############# //

	if (do_logging & LOG_CPU) {
        REPORT_INFO("Cpu::nb_transport_bw", "nb_transport_bw called.");
    }

    // Check phase
    if (phase != BEGIN_RESP) {
        REPORT_FATAL("Cpu::nb_transport_bw", "Unexpected phase in nb_transport_bw");
    }

    // Wait for bus cycle delay
    sc_time total_delay = delay_time + CLK_CYCLE_BUS;
    wait(total_delay);

    // Update phase and notify transaction finished
    phase = END_RESP;
    delay_time = SC_ZERO_TIME;
    transactionFinished_event.notify();

    if (do_logging & LOG_CPU) {
        REPORT_INFO("Cpu::nb_transport_bw", "Transaction completed.");
    }

    return TLM_COMPLETED;
	
	// ####################### UP TO HERE ####################### //
}

unsigned int Cpu::instances = 0;
