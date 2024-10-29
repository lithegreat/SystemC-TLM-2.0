#include "counter.h"

void counter::count() {
	// ############# COMPLETE THE FOLLOWING SECTION ############# //
	// The module counter should synchronously (i.e., with rising edge of clk) count from 0 to 99 (on
	// overflow it should restart with 0). Furthermore, it should be possible to reset the counter
	// asynchronously (reset is active low, i.e., when rst_n==0 the counter will reset to 0). The output
	// of the counter is required to be of type unsigned short int.

	while (true)
	{
		wait();
		if (rst_n.read() == 0)
		{
			cnt_int = 0;
		}
		else
		{
			if (cnt_int == 99)
			{
				cnt_int = 0;
			}
			else
			{
				cnt_int++;
			}
		}
		cnt.write(cnt_int);
	}
	

	// ####################### UP TO HERE ####################### //
}
