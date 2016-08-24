#include "zbaselib.h"

int main(int argc, char* argv[])
{
	long msec = 0;
	struct timeval tv = {0};

	for (; ;)
	{
		zbaselib_gettimeofday(&tv);
		msec = zbaselib_tv_to_msec(&tv);

		zbaselib_sleep_msec(100);

		zbaselib_gettimeofday(&tv);

		printf("%lld\n", zbaselib_tv_to_msec(&tv) - msec);
	}

	return 0;
}