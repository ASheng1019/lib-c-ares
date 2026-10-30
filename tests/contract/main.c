/* SPDX-License-Identifier: BSD-3-Clause */
#include <ares.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void check(int condition, int line)
{
	if (!condition) {
		printf("CARES_CONTRACT FAIL line=%d\n", line);
		exit(EXIT_FAILURE);
	}
}

#define CHECK(x) check((x), __LINE__)

int main(void)
{
	/* One A answer for example.test, with a compressed answer name. */
	static const unsigned char reply[] = {
		0x12, 0x34, 0x81, 0x80, 0, 1, 0, 1, 0, 0, 0, 0,
		7, 'e', 'x', 'a', 'm', 'p', 'l', 'e',
		4, 't', 'e', 's', 't', 0, 0, 1, 0, 1,
		0xc0, 0x0c, 0, 1, 0, 1, 0, 0, 0, 60, 0, 4,
		192, 0, 2, 42
	};
	static const unsigned char expected[] = {192, 0, 2, 42};
	ares_channel_t *channel = NULL;
	struct hostent *host = NULL;
	struct ares_options options = {0};

	CHECK(ares_library_init(ARES_LIB_INIT_ALL) == ARES_SUCCESS);
	CHECK(ares_init(&channel) == ARES_SUCCESS);
	CHECK(ares_set_servers_ports_csv(channel, "192.0.2.53:5353") ==
	      ARES_SUCCESS);
	ares_destroy(channel);
	CHECK(ares_parse_a_reply(reply, sizeof(reply), &host, NULL, NULL) ==
	      ARES_SUCCESS);
	CHECK(host && host->h_length == 4 && host->h_addr_list[0]);
	CHECK(!strcmp(host->h_name, "example.test"));
	CHECK(!memcmp(host->h_addr_list[0], expected, sizeof(expected)));
	ares_free_hostent(host);
	host = NULL;
	CHECK(ares_parse_a_reply(reply, sizeof(reply) - 1, &host, NULL, NULL) !=
	      ARES_SUCCESS);
	CHECK(!host);
	/* Qualify the poll/pipe/pthread facilities declared by this profile. */
	options.evsys = ARES_EVSYS_POLL;
	CHECK(ares_init_options(&channel, &options, ARES_OPT_EVENT_THREAD) ==
	      ARES_SUCCESS);
	ares_destroy(channel);
	ares_library_cleanup();
	puts("CARES_CONTRACT PASS channel=1 dns_parse=1 truncated=1 "
	     "poll_thread=1");
	return 0;
}
