/*
 * File:		testNetwork.c
 * Author:		Fuang.Cao <cavan.cfa@gmail.com>
 * Created:		2013-08-22 09:48:48
 *
 * Copyright (c) 2013 Fuang.Cao <cavan.cfa@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <cavan.h>
#include <cavan/tcp_proxy.h>
#include <cavan/network.h>

static void *network_client_recv_handler(void *data)
{
	struct network_client *client = data;

	while (1)
	{
		u32 value;
		ssize_t rdlen;

		println("client recv -");

		rdlen = client->recv(client, &value, sizeof(value));
		if (rdlen != sizeof(value))
		{
			pr_red_info("client->recv");
			break;
		}

		println("client recv %d", value);
	}

	return NULL;
}

static int network_client_test_base(struct network_client *client)
{
	int ret;
	u32 value;
	pthread_t thread_recv;

	pthread_create(&thread_recv, NULL, network_client_recv_handler, client);

	for (value = 0; value < 2000; value++)
	{
		println("client send %d", value);

		ret = client->send(client, &value, sizeof(value));
		if (ret != sizeof(value))
		{
			pr_red_info("client->send");
			goto out_network_client_close;
		}

		msleep(10);
	}

	msleep(1000);

	ret = 0;
out_network_client_close:
	network_client_close(client);
	pthread_join(thread_recv, NULL);

	return ret;
}

static int network_client_test(const char *url)
{
	int ret;
	struct network_client client;

	ret = network_client_open2(&client, url, CAVAN_NET_FLAG_TALK | CAVAN_NET_FLAG_SYNC);
	if (ret < 0)
	{
		pr_red_info("network_client_open");
		return -EFAULT;
	}

	return network_client_test_base(&client);
}

static int network_service_test(const char *url)
{
	int ret;
	struct network_client client;
	struct network_service service;

	ret = network_service_open2(&service, url, 0);
	if (ret < 0)
	{
		pr_red_info("network_service_open");
		return ret;
	}

	ret = service.accept(&service, &client);
	if (ret < 0)
	{
		pr_red_info("service.accept");
		goto out_network_service_close;
	}

	ret = network_client_test_base(&client);

out_network_service_close:
	network_service_close(&service);
	return ret;
}

static int network_url_test(const char *_url)
{
	struct network_url url;

	if (network_url_parse(&url, _url) == NULL)
	{
		pr_red_info("web_proxy_parse_url");
	}
	else
	{
		println("protocol = %s", url.protocol);
		println("hostname = %s", url.hostname);
		println("port = %d", url.port);
		println("pathname = %s", url.pathname);
		println("url = %s", network_url_tostring(&url, NULL, 0, NULL));
	}

	return 0;
}

static int network_dump_data(const char *url)
{
	int ret;
	struct network_client client;
	struct network_service service;

	ret = network_service_open2(&service, url, 0);
	if (ret < 0)
	{
		pr_red_info("network_service_open2");
		return ret;
	}

	while (1)
	{
		ret = network_service_accept(&service, &client);
		if (ret < 0)
		{
			pr_red_info("network_service_accept");
			goto out_network_service_close;
		}

		while (1)
		{
			char buff[1024];

			ret = client.recv(&client, buff, sizeof(buff));
			if (ret <= 0)
			{
				pr_red_info("client->recv");
				break;
			}

			print_ntext(buff, ret);
		}

		network_client_close(&client);
	}

out_network_service_close:
	network_service_close(&service);
	return ret;
}

static int network_test_send(const char *url, const char *pathname)
{
	int ret;
	struct network_client client;

	ret = network_client_open2(&client, url, CAVAN_NET_FLAG_TALK | CAVAN_NET_FLAG_SYNC);
	if (ret < 0)
	{
		pr_red_info("network_client_open");
		return -EFAULT;
	}

	ret = network_client_send_file2(&client, pathname, 0);
	if (ret < 0)
	{
		pr_red_info("network_client_send_file2");
	}
	else
	{
		char buff[1024];

		ret = client.recv(&client, buff, sizeof(buff));
		if (ret > 0)
		{
			print_ntext(buff, ret);
		}
		else
		{
			pr_red_info("client.recv");
		}
	}

	network_client_close(&client);

	return ret;
}

int main(int argc, char *argv[])
{
	assert(argc > 2);

	if (strcmp(argv[1], "client") == 0)
	{
		return network_client_test(argv[2]);
	}
	else if (strcmp(argv[1], "service") == 0)
	{
		while (network_service_test(argv[2]) >= 0);
	}
	else if (strcmp(argv[1], "url") == 0)
	{
		return network_url_test(argv[2]);
	}
	else if (strcmp(argv[1], "dump") == 0)
	{
		return network_dump_data(argv[2]);
	}
	else if (strcmp(argv[1], "send") == 0)
	{
		assert(argc > 3);

		return network_test_send(argv[2], argv[3]);
	}
	else
	{
		pr_red_info("unknown command %s", argv[1]);
	}

	return 0;
}
