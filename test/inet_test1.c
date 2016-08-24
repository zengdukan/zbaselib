#include "zbaselib_socket.h"
#include <assert.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
	char cmd = argv[1][0];
	int ret = 0;
	
	switch(cmd)
	{
	case 'a':
		ret = zbaselib_inet_nic_up(argv[2]);
		printf("zbaselib_inet_nic_up %s = ret:%d\n", argv[2], ret);
		break;

	case 'b':
		{
			ret = zbaselib_inet_nic_down(argv[2]);
			printf("zbaselib_inet_nic_down %s = ret:%d\n", argv[2], ret);
		}
		break;
		
	case 'c':
		{
			in_addr_t addr = zbaselib_inet_get_ifaddr(argv[2]);			
			char strip2[128] = {0};
			assert(0 == zbaselib_inet_ntop(AF_INET, (void*)&addr, strip2, sizeof(strip2)));
			printf("get %s ip:%s\n", argv[2], strip2);
		}
		break;
		
	case 'd':
		{
			in_addr_t addr  = 0;
			zbaselib_inet_pton(AF_INET, argv[3], &addr);
			ret = zbaselib_inet_set_ifaddr(argv[2], addr);
			printf("zbaselib_inet_set_ifaddr %s %s ret:%d\n", argv[2], argv[3], ret);
		}
		break;
	case 'e':
		{
			in_addr_t addr = zbaselib_inet_get_netmask(argv[2]);
			char strip2[128] = {0};
			assert(0 == zbaselib_inet_ntop(AF_INET, (void*)&addr, strip2, sizeof(strip2)));
			printf("get %s netmask:%s\n", argv[2], strip2);
		}
		break;
	case 'f':
		{			
			in_addr_t addr  = 0;
			zbaselib_inet_pton(AF_INET, argv[3], &addr);
			ret = zbaselib_inet_set_netmask(argv[2], addr);
			printf("zbaselib_inet_set_netmask %s %s ret:%d\n", argv[2], argv[3], ret);
		}
		break;
		
	case 'g':
		{
			unsigned char macbuf[6] = {0};
			ret = zbaselib_inet_get_hwaddr(argv[2], macbuf);
			printf("zbaselib_inet_get_hwaddr %s ret:%d, mac:%02x:%02x:%02x:%02x:%02x:%02x\n", argv[2], ret,
				macbuf[0],macbuf[1],macbuf[2],macbuf[3],macbuf[4],macbuf[5]);
		}
		break;
		
	case 'h':
		ret = zbaselib_inet_set_dns(argv[2], argv[3]);
		printf("zbaselib_inet_set_dns %s %s ret:%d\n", argv[2], argv[3], ret);
		break;
		
	case 'i':
		{
			in_addr_t gw = 0;
			char strip2[128] = {0};
			
			ret = zbaselib_inet_get_gateway(argv[2], &gw);
			if(ret == 0)
			{
				assert(0 == zbaselib_inet_ntop(AF_INET, (void*)&gw, strip2, sizeof(strip2)));
				printf("gateway:%s\n", strip2);
			}
			else
				printf("get gateway fail!\n");
		}
		break;

	case 'j':
		{
			in_addr_t dns[2];
			char strip2[128] = {0};
			int ret = zbaselib_inet_get_dns(dns);
			if(ret > 0)
			{
				int i = 0;
				for(i = 0; i < ret; i++)
				{
					memset(strip2, 0, sizeof(strip2));
					zbaselib_inet_ntop(AF_INET, (void*)&dns[i], strip2, sizeof(strip2));
					printf("dns%d:%s\n", i + 1, strip2);
				}
			}
			else
				printf("get dns fail:%d\n", ret);
		}
		break;
	}

	return 0;
}
