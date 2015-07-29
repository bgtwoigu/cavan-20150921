/*
 * File:		mux.c
 * Author:		Fuang.Cao <cavan.cfa@gmail.com>
 * Created:		2015-07-28 11:43:37
 *
 * Copyright (c) 2015 Fuang.Cao <cavan.cfa@gmail.com>
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
#include <cavan/mux.h>

static int cavan_mux_recv_thread_handler(struct cavan_thread *thread, void *data)
{
	ssize_t rdlen;
	struct cavan_mux *mux = data;
	char buff[CAVAN_MUX_MUTT + sizeof(struct cavan_mux_package)];

	rdlen = mux->recv(mux, buff, sizeof(buff));
	if (rdlen < 0)
	{
		pr_red_info("mux->recv");
		return rdlen;
	}

	return cavan_mux_write_recv_data(mux, buff, rdlen);
}

int cavan_mux_init(struct cavan_mux *mux, void *data)
{
	int i;
	int ret;
	struct cavan_thread *thread;

	if (mux->send == NULL || mux->recv == NULL)
	{
		pr_red_info("mux->send = %p, mux->recv = %p", mux->send, mux->recv);
		return -EINVAL;
	}

	cavan_lock_init(&mux->lock, false);
	mux->private_data = data;
	mux->packages = NULL;
	mux->port_max = 0;

	for (i = 0; i < NELEM(mux->links); i++)
	{
		mux->links[i] = NULL;
	}

	thread = &mux->recv_thread;
	thread->name = "MUX RECEIVE";
	thread->handler = cavan_mux_recv_thread_handler;
	thread->wake_handker = NULL;
	ret = cavan_thread_run(thread, mux);
	if (ret < 0)
	{
		pr_red_info("cavan_thread_run");
		goto out_cavan_lock_deinit;
	}

	return 0;

out_cavan_lock_deinit:
	cavan_lock_deinit(&mux->lock);
	return ret;
}

void cavan_mux_deinit(struct cavan_mux *mux)
{
	struct cavan_mux_package_raw *head;

	cavan_thread_stop(&mux->recv_thread);

	cavan_lock_acquire(&mux->lock);

	head = mux->packages;
	while (head)
	{
		struct cavan_mux_package_raw *next = head->next;

		free(head);
		head = next;
	}

	cavan_lock_release(&mux->lock);

	cavan_lock_deinit(&mux->lock);
}

void cavan_mux_append_package(struct cavan_mux *mux, struct cavan_mux_package_raw *package)
{
	struct cavan_mux_package_raw **head;

	cavan_lock_acquire(&mux->lock);

	for (head = &mux->packages; *head && (*head)->length < package->length; head = &(*head)->next);

	package->next = *head;
	*head = package;

	cavan_lock_release(&mux->lock);
}

struct cavan_mux_package_raw *cavan_mux_dequeue_package(struct cavan_mux *mux, size_t length)
{
	struct cavan_mux_package_raw **head;
	struct cavan_mux_package_raw *package;

	cavan_lock_acquire(&mux->lock);

	head = &mux->packages;

	while (*head && (*head)->length < length)
	{
		head = &(*head)->next;
	}

	package = *head;
	if (package)
	{
		*head = package->next;
	}

	cavan_lock_release(&mux->lock);

	return package;
}

void cavan_mux_show_packages(struct cavan_mux *mux)
{
	struct cavan_mux_package_raw *package;

	cavan_lock_acquire(&mux->lock);

	for (package = mux->packages; package; package = package->next)
	{
		println("length = %d", package->length);
	}

	cavan_lock_release(&mux->lock);
}

struct cavan_mux_package *cavan_mux_package_alloc(struct cavan_mux *mux, size_t length)
{
	struct cavan_mux_package *package;
	struct cavan_mux_package_raw *package_raw;

	package_raw = cavan_mux_dequeue_package(mux, length);
	if (package_raw == NULL)
	{
		package_raw = malloc(sizeof(struct cavan_mux_package_raw) + length);
		if (package_raw == NULL)
		{
			pr_error_info("malloc");
			return NULL;
		}

		package_raw->length = length;
	}

	package = &package_raw->package;
	package->length = length;

	return package;
}

void cavan_mux_package_free(struct cavan_mux *mux, struct cavan_mux_package *package)
{
	cavan_mux_append_package(mux, MEMBER_TO_STRUCT(package, struct cavan_mux_package_raw, package));
}

ssize_t cavan_mux_link_send_data(struct cavan_mux_link *link, const void *buff, size_t size)
{
	ssize_t wrlen;
	struct cavan_mux_package *package;
	struct cavan_mux *mux = link->mux;

	package = cavan_mux_package_alloc(mux, size);
	if (package == NULL)
	{
		pr_red_info("cavan_mux_package_alloc");
		return -ENOMEM;
	}

	package->src_port = link->local_port;
	package->dest_port = link->remote_port;
	memcpy(package->data, buff, size);
	wrlen = mux->send(mux, package, cavan_mux_package_get_whole_length(package));

	cavan_mux_package_free(mux, package);

	return wrlen;
}

int cavan_mux_add_link(struct cavan_mux *mux, struct cavan_mux_link *link, u16 port)
{
	struct cavan_mux_link **head = mux->links + (cavan_mux_link_head_index(port));

	while ((*head) && (*head)->local_port < port)
	{
		head = &(*head)->next;
	}

	if ((*head) && (*head)->local_port == port)
	{
		pr_red_info("port %d already exists!", port);
		return -EINVAL;
	}

	link->local_port = port;
	link->mux = mux;

	link->next = *head;
	*head = link;

	if (link->local_port > mux->port_max)
	{
		mux->port_max = link->local_port;
	}

	return 0;
}

struct cavan_mux_link *cavan_mux_find_link(struct cavan_mux *mux, u16 port)
{
	struct cavan_mux_link *link;

	for (link = mux->links[cavan_mux_link_head_index(port)]; link && link->local_port != port; link = link->next)
	{
		println("local_port = %d", link->local_port);
	}

	return link;
}

void cavan_mux_remove_link(struct cavan_mux *mux, struct cavan_mux_link *link)
{
	struct cavan_mux_link **head = mux->links + (cavan_mux_link_head_index(link->local_port));

	if (link == *head)
	{
		*head = link->next;
	}
	else
	{
		struct cavan_mux_link *prev;

		for (prev = *head; prev && prev->next != link; prev = prev->next);

		if (prev)
		{
			prev->next = link->next;
		}
	}
}

int cavan_mux_bind(struct cavan_mux *mux, struct cavan_mux_link *link, u16 port)
{
	if (port == 0)
	{
		for (port = mux->port_max + 1; cavan_mux_add_link(mux, link, port) < 0; port++);

		return 0;
	}
	else
	{
		return cavan_mux_add_link(mux, link, port);
	}
}

int cavan_mux_write_recv_package(struct cavan_mux *mux, const struct cavan_mux_package *package)
{
	struct cavan_mux_link *link;

	link = cavan_mux_find_link(mux, package->dest_port);
	if (link == NULL)
	{
		pr_red_info("invalid port %d", package->dest_port);
		return -EINVAL;
	}

	link->remote_port = package->src_port;

	return link->on_received(link, package->data, package->length);
}

int cavan_mux_write_recv_data(struct cavan_mux *mux, const void *buff, size_t size)
{
	const struct cavan_mux_package *package = buff;

	if (size != cavan_mux_package_get_whole_length(package))
	{
		pr_red_info("invalid length");
		return -EINVAL;
	}

	return cavan_mux_write_recv_package(mux, package);
}
