#include "seraphic_phoenix_am.h"

int main()
{
	// just some useless code to test 

	uint8_t buffer[256] = {0};

	ring_buffer rb;

	sp_rb_setup(&rb,buffer,256);

	TEST_ERROR("ok");

	return 0;
}
