#include <stdio.h>
#include <string.h>
#include "zbaselib_md5.h"
#include <assert.h>

const char HEX[16] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'a', 'b',
	'c', 'd', 'e', 'f'
};

void byteToHex(unsigned char digest[16], char dig_out[32])
{
	int i = 0;
	for (i = 0; i < 16; ++i) {
		int t = (int)digest[i];
		int a = t / 16;
		int b = t % 16;
		dig_out[2*i] = HEX[a];
		dig_out[2*i + 1] = HEX[b];
	}
}

int main(int argc, char* argv[])
{
	/*
	VsMD5Context md5;
	unsigned char digest[16] = {0};
	char dig_out[33] = {0};

	MD5_Init(&md5);
	char str[] = "111";

	MD5_Update(&md5, str, strlen(str)); 
	MD5_Final(digest, &md5);
	byteToHex(digest, dig_out);
	printf("%s\n", dig_out);
*/
	zbaselib_md5_t md5;
	u_char digest[16] = {0};
	char str[] = "111";
	char dig_out[33] = {0};
	
	zbaselib_md5_init(&md5);
	zbaselib_md5_update(&md5, str, strlen(str));
//	zbaselib_md5_update(&md5, argv[1], strlen(argv[1]));
	zbaselib_md5_final(digest, &md5);
	byteToHex(digest, dig_out);
	printf("%s\n", dig_out);


	return 0;
}