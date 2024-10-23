#include "bcd_decoder.h"

void bcd_decoder::decode() {
	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// The bcd_decoder converts the counter value to BCD. The unit position on the output v_lo and
	// the decade v_hi are both of type char

    unsigned short int counter_value = val.read();

    char units = counter_value % 10;
    char tens = (counter_value / 10) % 10;

    lo.write(units);
    hi.write(tens);
	
	// ####################### UP TO HERE ####################### //
}
