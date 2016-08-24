#include "zbaselib_socket.h"

void print_interface_info(uv_interface_address_t iface) {
  char buf[512] = {0};
  size_t buf_size = sizeof(buf);
  const char* r;

  fprintf(stderr, "%s (%s)\n", iface.name, iface.is_internal ? "internal" : "external");
  if (iface.address.address4.sin_family == AF_INET) {

    r = inet_ntop(AF_INET, &iface.address.address4.sin_addr, buf, buf_size);
    if (!r) perror("ip4_name");
    fprintf(stderr, "  IPv4 address: %s\n", buf);

  } else if (iface.address.address4.sin_family == AF_INET6) {

    r = inet_ntop(AF_INET6, &iface.address.address6.sin6_addr, buf, buf_size);
    if (!r) perror("ip6_name");
    fprintf(stderr, "  IPv6 address: %s\n", buf);

  }

  fprintf(stderr, "\n");
}


int main(int argc, char* argv[])
{
	uv_interface_address_t* ifaddrs = NULL;
	int count = 0;

	if(uv_interface_addresses(&ifaddrs, &count) < 0)
	{
		printf("get ifaddrs fail\n");
	}
	else
	{
		int i = 0;
		for(i = 0; i < count; i++)
		{
			print_interface_info(ifaddrs[i]);
		}

		uv_free_interface_addresses(ifaddrs, count);
	}

	getchar();

	return 0;
}

