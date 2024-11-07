#ifndef __FIFO_IF
#define __FIFO_IF

// ############# COMPLETE THE FOLLOWING SECTION ############# //
#include "systemc.h"
class fifo_if : public sc_interface {
    public:
        virtual bool write_fifo(unsigned char *data, unsigned int &count) = 0;
        virtual bool read_fifo(unsigned char *data, unsigned int &count) = 0;
};
// ####################### UP TO HERE ####################### //
#endif // __FIFO_IF
