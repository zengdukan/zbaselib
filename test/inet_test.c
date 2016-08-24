#include "zbaselib_socket.h"
#include <assert.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
	char strip[] = "192.168.1.3";
	uint32_t ip = 0;
	char strip2[128] = {0};
	char strdstip[64] = {0};
	int dstip = 0;

	zbaselib_socket_init();

	assert(0 == zbaselib_inet_pton(AF_INET, strip, &ip));
	assert(0 == zbaselib_inet_ntop(AF_INET, (void*)&ip, strip2, sizeof(strip2)));

	assert(strcmp(strip, strip2) == 0);

	assert(zbaselib_socket_getipbyname("192.168.30.101", strdstip, &dstip) >= 0);
	printf("%s, %d\n", strdstip, dstip);

	zbaselib_socket_deinit();

	

	return 0;
}
