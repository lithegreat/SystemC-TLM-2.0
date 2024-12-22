#include "Cpu.h"
#include "IoModule.h"
#include "reporting.h"

using namespace sc_core;
using namespace tlm;

void Cpu::processor_thread(void)
{
    while (true)
    {
        // read new packet descriptor
        wait(packetReceived_interrupt.value_changed_event());
        while (packetReceived_interrupt.read() == true)
        {
            // 1) Read the packet descriptor
            MEASURE_TRANSFER_TIME(
                startTransaction(TLM_READ_COMMAND, 0x10000000, (unsigned char *)&m_packet_descriptor, sizeof(packet_descriptor)););
            // 2) Read the packet header (plus timestamp, size)
            unsigned int headerSize = sizeof(sc_time) + sizeof(uint64_t) + IpPacket::MINIMAL_IP_HEADER_LENGTH;
            MEASURE_TRANSFER_TIME(
                startTransaction(TLM_READ_COMMAND, m_packet_descriptor.baseAddress, (unsigned char *)&m_packet_header, headerSize););

            bool valid;
            MEASURE_PROCESSING_TIME(
                valid = verifyHeaderIntegrity(m_packet_header);
                wait(CPU_VERIFY_HEADER_CYCLES * CLK_CYCLE_CPU););

            // 3) Process the packet
            if (valid)
            {
                unsigned int portId;
                MEASURE_PROCESSING_TIME(
                    portId = makeNHLookup(m_packet_header);
                    wait(CPU_IP_LOOKUP_CYCLES * CLK_CYCLE_CPU););
                MEASURE_PROCESSING_TIME(
                    decrementTTL(m_packet_header);
                    wait(CPU_DECREMENT_TTL_CYCLES * CLK_CYCLE_CPU););
                MEASURE_PROCESSING_TIME(
                    updateChecksum(m_packet_header);
                    wait(CPU_UPDATE_CHECKSUM_CYCLES * CLK_CYCLE_CPU););
                // 4) Write the updated header back to memory
                MEASURE_TRANSFER_TIME(
                    startTransaction(TLM_WRITE_COMMAND, m_packet_descriptor.baseAddress, (unsigned char *)&m_packet_header, headerSize););

                // 5) Forward the packet descriptor to the appropriate output port
                soc_address_t output_address = 0x20000000 + (portId * 0x10000000);
                MEASURE_TRANSFER_TIME(
                    startTransaction(TLM_WRITE_COMMAND, output_address, (unsigned char *)&m_packet_descriptor, sizeof(packet_descriptor)););
            }
            else
            {
                // Discard the packet
                MEASURE_TRANSFER_TIME(
                    startTransaction(TLM_WRITE_COMMAND, 0x10000000, (unsigned char *)&m_packet_descriptor, sizeof(packet_descriptor)););
            }
        }
    }
}

// ############# COMPLETE THE FOLLOWING SECTION ############# //
// startTransaction
void Cpu::startTransaction(tlm_command command, soc_address_t address, unsigned char *data, unsigned int dataSize)
{
    payload.set_command(command);
    payload.set_address(address);
    payload.set_data_ptr(data);
    payload.set_data_length(dataSize);

    tlm_phase phase = BEGIN_REQ;
    sc_time delay_time = SC_ZERO_TIME;
    tlm_sync_enum resp = initiator_socket->nb_transport_fw(payload, phase, delay_time);

    wait(transactionFinished_event);

    if (resp != TLM_UPDATED)
    {
        cout << "Error: " << "Transaction not updated correctly." << endl;
        exit(1);
    }

    if (phase != END_REQ)
    {
        cout << "Error: " << "Transaction phase not updated correctly." << endl;
        exit(1);
    }

    if (payload.get_response_status() != TLM_OK_RESPONSE || payload.is_response_error())
    {
        cout << "Error: " << "Transaction not completed successfully." << endl;
        exit(1);
    }
}
// ####################### UP TO HERE ####################### //

// nb_transport_bw: implementation of the backward path callback
tlm_sync_enum Cpu::nb_transport_bw(tlm_generic_payload &transaction,
                                   tlm_phase &phase, sc_time &delay_time)
{
    // ############# COMPLETE THE FOLLOWING SECTION ############# //

    // check whether the phase is set appropriately when nb_transport_bw is called.
    if (phase != BEGIN_RESP)
    {
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
