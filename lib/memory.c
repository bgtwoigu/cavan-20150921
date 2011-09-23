// Fuang.Cao <cavan.cfa@gmail.com> Wed Apr 13 09:53:53 CST 2011

#include <cavan.h>
#include <cavan/memory.h>

void *mem_copy(void *dest, const void *src, size_t size)
{
	const void *end = src + size;

	while (src < end)
	{
		*(char *)dest++ = *(const char *)src++;
	}

	return dest;
}

void *mem_copy16(void *dest, const void *src, size_t size)
{
	void *end_dest;

	if (((u32)dest & 1) != ((u32)src & 1))
	{
		return text_ncopy(dest, src, size);
	}

	end_dest = dest + size;

	while ((u32)src & 1)
	{
		*(char *)dest++ = *(const char *)src++;
		size--;
	}

	dest = text_copy16(dest, src, size >> 1);

	while (dest < end_dest)
	{
		*(char *)dest++ = *(const char *)src++;
	}

	return dest;
}

void *mem_copy32(void *dest, const void *src, size_t size)
{
	void *end_dest;

	if (((u32)dest & 3) != ((u32)src & 3))
	{
		return mem_copy16(dest, src, size);
	}

	end_dest = dest + size;

	while ((u32)dest & 3)
	{
		*(char *)dest++ = *(const char *)src++;
		size--;
	}

	dest = text_copy32(dest, src, size >> 2);

	while (dest < end_dest)
	{
		*(char *)dest++ = *(const char *)src++;
	}

	return dest;
}

void *mem_copy64(void *dest, const void *src, size_t size)
{
	void *end_dest;

	if (((u32)dest & 7) != ((u32)src & 7))
	{
		return mem_copy32(dest, src, size);
	}

	end_dest = dest + size;

	while ((u32)dest & 7)
	{
		*(char *)dest++ = *(const char *)src++;
		size--;
	}

	dest = text_copy64(dest, src, size >> 3);

	while (dest < end_dest)
	{
		*(char *)dest++ = *(const char *)src++;
	}

	return dest;
}

void *mem_set16(void *mem, int value, size_t size)
{
	void *end_mem = mem + size;
	u16 value16;

	while ((u32)mem & 1)
	{
		*(u8 *)mem++ = value;
		size--;
	}

	text_set8((u8 *)&value16, value, sizeof(value16));
	mem = text_set16(mem, value16, size >> 1);

	while (mem < end_mem)
	{
		*(u8 *)mem++ = value;
	}

	return mem;
}

void *mem_set32(void *mem, int value, size_t size)
{
	void *end_mem = mem + size;
	u32 value32;

	while ((u32)mem & 3)
	{
		*(u8 *)mem++ = value;
		size--;
	}

	text_set8((u8 *)&value32, value, sizeof(value32));
	mem = text_set32(mem, value32, size >> 2);

	while (mem < end_mem)
	{
		*(u8 *)mem++ = value;
	}

	return mem;
}

void *mem_set64(void *mem, int value, size_t size)
{
	void *end_mem = mem + size;
	u64 value64;

	while ((u32)mem & 7)
	{
		*(u8 *)mem++ = value;
		size--;
	}

	text_set8((u8 *)&value64, value, sizeof(value64));
	mem = text_set64(mem, value64, size >> 3);

	while (mem < end_mem)
	{
		*(u8 *)mem++ = value;
	}

	return mem;
}

void bits_set(void *mem, int start, int end, u32 value)
{
	int i;

	for (i = end; i >= start; i--)
	{
		int offset;
		char *mem8;

		mem8 = mem + (i >> 3);
		offset = i & 7;

		*mem8 &= ~(1 << offset);
		*mem8 |= (value & 1) << offset;

		print_char(value & 1 ? '1' : '0');

		value >>= 1;
	}
}

void mem_build_kmp_array(const void *sub, off_t *step, const size_t size)
{
	int i, j, k;

	for (i = 1, step[0] = -1; i < size; i++)
	{
		j = 1;
		k = 0;

		while (j < i)
		{
			if (((char *)sub)[j] == ((char *)sub)[k])
			{
				j++;
				k++;
			}
			else if (step[k] == -1)
			{
				j++;
				k = 0;
			}
			else
			{
				k = step[k];
			}
		}

		if (((char *)sub)[k] == ((char *)sub)[i])
		{
			step[i] = step[k];
		}
		else
		{
			step[i] = k;
		}
	}
}

void *__mem_kmp_find(const void *mem, const void *sub, const size_t memlen, const size_t sublen, const off_t *step)
{
	int i, j;

	i = j = 0;
	while (i < memlen && j < sublen)
	{
		if (((char *)mem)[i] == ((char *)sub)[j])
		{
			i++;
			j++;
		}
		else if (step[j] == -1)
		{
			i++;
			j = 0;
		}
		else
		{
			j = step[j];
		}
	}

	if (j < sublen)
	{
		return NULL;
	}

	return (void *)(mem + i - sublen);
}

void *mem_kmp_find(const void *mem, const void *sub, const size_t memlen, const size_t sublen)
{
	off_t step[sublen];

	mem_build_kmp_array(sub, step, sublen);

	return __mem_kmp_find(mem, sub, memlen, sublen, step);
}

size_t __mem_delete_char(const void *mem_in, void *mem_out, const size_t size, const char c)
{
	const void *mem_end;
	void *mem_bak;

	mem_bak = mem_out;
	mem_end = mem_in + size;

	while (mem_in < mem_end)
	{
		if (*(const char *)mem_in == c)
		{
			mem_in++;
			continue;
		}

		*(char *)mem_out++ = *(const char *)mem_in++;
	}

	return mem_out - mem_bak;
}

void number_swap8(u8 *num1, u8 *num2)
{
	u8 temp;

	temp = *num1;
	*num1 = *num2;
	*num2 = temp;
}

void number_swap16(u16 *num1, u16 *num2)
{
	u16 temp;

	temp = *num1;
	*num1 = *num2;
	*num2 = temp;
}

void number_swap32(u32 *num1, u32 *num2)
{
	u32 temp;

	temp = *num1;
	*num1 = *num2;
	*num2 = temp;
}

int mem_is_set(const void *mem, int value, size_t size)
{
	const void *mem_end;

	for (mem_end = mem + size; mem < mem_end && *(u8 *)mem == value; mem++);

	return mem >= mem_end;
}

int mem_is_noset(const void *mem, int value, size_t size)
{
	const void *mem_end;

	for (mem_end = mem + size; mem < mem_end && *(u8 *)mem == value; mem++);

	return mem < mem_end;
}

u16 checksum16(const u16 *buff, size_t size)
{
	const u16 *buff_end;
	u32 checksum;

	for (checksum = 0, buff_end = buff + (size >> 1); buff < buff_end; buff++)
	{
		checksum += *buff;
	}

	if (size & 1)
	{
		checksum += *(u8 *)buff;
	}

	checksum = (checksum >> 16) + (checksum & 0xFFFF);

	return (checksum + (checksum >> 16)) & 0xFFFF;
}
