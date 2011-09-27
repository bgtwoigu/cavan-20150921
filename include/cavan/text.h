#pragma once

#include <stdio.h>

#define PRINT_BUFFER_LEN	KB(2)
#define FLAG_SIGNED			(1 << 9)
#define FLAG_PREFIX			(1 << 10)

#define IS_NUMBER(c)		((c) >= '0' && (c) <= '9')
#define IS_FLOAT(c)			((c) == '.' || IS_NUMBER(c))

#define IS_LOWERCASE(c)		((c) >= 'a' && (c) <= 'z')
#define IS_UPPERCASE(c)		((c) >= 'A' && (c) <= 'Z')
#define IS_LETTER(c)		(IS_LOWERCASE(c) || IS_UPPERCASE(c))

#define DEFAULT_PATH_VALUE	"/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:/mybin"

#define value_trans(value) \
	({ \
		__typeof(value) __tmp = value; \
		text_ntrans((char *)&__tmp, sizeof(__tmp)); \
		__tmp = __tmp; \
	})

size_t text_len(const char *text);
char *text_cat(char *dest, const char *src);
char *text_cat2(char *dest, char *srcs[], size_t count);
char *text_vcat3(char *dest, size_t count, va_list ap);
char *text_cat3(char *dest, size_t count, ...);
char *text_cat4(char *dest, char *srcs[]);
char *text_vcat5(char *dest, va_list ap);
char *text_cat5(char *dest, ...);

char *text_copy(char *dest, const char *src);
char *text_ncopy(char *dest, const char *src, size_t count);
u16 *text_copy16(u16 *dest, const u16 *src, size_t count);
u32 *text_copy32(u32 *dest, const u32 *src, size_t count);
u64 *text_copy64(u64 *dest, const u64 *src, size_t count);

void text_show(const char *text, size_t count);
void text_show16(const u16 *text, size_t count);
void text_show32(const u32 *text, size_t count);
void text_show64(const u64 *text, size_t count);

char *text_find(const char *text, char c);
char *text_nfind(const char *text, char c);
char *text_ntrans(char *text, size_t size);

int text_cmp(const char *text1, const char *text2);
int text_hcmp(const char *text1, const char *text2);
int text_lhcmp(const char *text1, const char *text2);
int text_rhcmp(const char *text1, const char *text2);
int text_tcmp(const char *text1, const char *text2);
int text_ltcmp(const char *text1, const char *text2);
int text_rtcmp(const char *text1, const char *text2);
int text_ncmp(const char *text1, const char *text2, size_t size);

static inline char *text_trans(char *text)
{
	return text_ntrans(text, text_len(text));
}

void *text_set8(u8 *text, u8 value, size_t count);
void *text_set16(u16 *text, u16 value, size_t count);
void *text_set32(u32 *text, u32 value, size_t count);
void *text_set64(u64 *text, u64 value, size_t count);

char *text_ndup(const char *text, size_t size);
char *text_dup(const char *text);
char *text_tok(char *text, const char *delim);

int char2value(char c);
int prefix2base(const char *prefix, const char **prefix_ret);
u64 text2value_unsigned(const char *text, const char **text_ret, int base);
s64 text2value(const char *text, const char **text_ret, int base);

void text_reverse_simple(char *start, char *end);
void text_reverse1(char *p1, char *p2);
void text_reverse2(char *text, size_t size);
void text_reverse3(char *text);

char *reverse_value2text_base2(u64 value, char *buff, size_t size);
char *reverse_value2text_base4(u64 value, char *buff, size_t size);
char *reverse_value2text_base8(u64 value, char *buff, size_t size);
char *reverse_value2text_base16(u64 value, char *buff, size_t size);
char *reverse_value2text_base32(u64 value, char *buff, size_t size);
char *reverse_value2text_all(u64 value, char *buff, size_t size, int base);
char *simple_value2text_reverse(u64 value, char *buff, size_t size, int base);
char *simple_value2text_unsigned(u64 value, char *buff, size_t size, int base);
char *simple_value2text(s64 value, char *buff, size_t size, int base);

char *__value2text(s64 value, char *text, int size, char fill, int flag);
char *value2text(u64 value, int flag);

u64 text2size(const char *text, const char **text_ret);
char *__size2text(u64 size, char *buff);
char *size2text(u64 size);

char *__text_basename(char *buff, const char *path);
char *text_basename(const char *path);

char *__text_dirname(char *buff, const char *path);
char *text_dirname(const char *path);

char *__get_ntext(const char *src, char *dest, int start, int count);
char *get_ntext(const char *src, int start, int count);
char *text_header(const char *text, int count);
char *__get_text_region(const char *src, char *dest, int start, int end);
char *get_text_region(const char *src, int start, int end);

void system_sync(void);

char *__mem2text(const void *mem, char *buff, int size);
char *mem2text(const void *mem, int size);

char *__to_abs_path(const char *rel_path, char *abs_path, size_t size);
char *to_abs_path(const char *rel_path);
char *__to_abs_path_directory(const char *rel_path, char *abs_path, size_t size);
char *to_abs_directory(const char *rel_path);
char *__to_abs_path2(const char *rel_path, char *abs_path, size_t size);
char *to_abs_path2(const char *rel_path);
char *__prettify_pathname(const char *src_path, char *dest_path, size_t size);
char *prettify_pathname(const char *src_path);

char *text_path_cat(char *pathname, const char *dirname, const char *basename);

char *__text_delete_char(const char *text_in, char *text_out, char c);
char *__text_delete_sub(const char *text_in, char *text_out, const char *sub, const size_t sublen);

void *mem_kmp_find(const void *mem, const void *sub, size_t memlen, size_t sublen);
char *text_find_line(const char *text, int index);
char *text_get_line(const char *text, char *buff, int index);
int text_version_cmp(const char *v1, const char *v2, char sep);

int text_has_char(const char *text, char c);
char *text_skip_chars(const char *text, const char *chars);

char *text_to_uppercase(const char *src, char *dest);
char *text_to_lowercase(const char *src, char *dest);
char *text_to_nuppercase(const char *src, char *dest, size_t size);
char *text_to_nlowercase(const char *src, char *dest, size_t size);
int text_cmp_nocase(const char *text1, const char *text2);
int text_ncmp_nocase(const char *text1, const char *text2, size_t size);
int text_bool_value(const char *text);

__printf_format_20__ char *vformat_text (char *buff, const char *fmt, va_list args);
__printf_format_12__ char *format_text(const char *fmt, ...);

__printf_format_10__ int vsystem_command(const char *command, va_list ap);
__printf_format_12__ int system_command(const char *command, ...);
__printf_format_23__ int system_command_retry(int count, const char *command, ...);
__printf_format_23__ int system_command_path(const char *path, const char *command, ...);

__printf_format_10__ FILE *vpipe_command(const char *command, va_list ap);
__printf_format_12__ FILE *pipe_command(const char *command, ...);
__printf_format_10__ FILE *vpipe_command_verbose(const char *command, va_list ap);
__printf_format_12__ FILE *pipe_command_verbose(const char *command, ...);
__printf_format_23__ FILE *pipe_command_path(const char *path, const char *command, ...);

__printf_format_30__ ssize_t vbuff_command(char *buff, size_t bufflen, const char *command, va_list ap);
__printf_format_12__ char *buff_command(const char *command, ...);
__printf_format_34__ ssize_t buff_command2(char *buff, size_t bufflen, const char *command, ...);
__printf_format_23__ char *buff_command_path(const char *path, const char *command, ...);
__printf_format_45__ ssize_t buff_command_path2(const char *path, char *buff, size_t bufflen, const char *command, ...);

void *__mac_address_tostring(const void *mac, size_t maclen, void *buff);
char *mac_address_tostring(const void *mac, size_t maclen);

int text_is_number(const char *text);
int text_is_float(const char *text);
int text_is_uppercase(const char *text);
int text_is_lowercase(const char *text);
int text_is_letter(const char *text);

// ============================================================

static inline char *text_kmp_find(const char *buff, const char *sub)
{
	return mem_kmp_find(buff, sub, text_len(buff), text_len(sub));
}

static inline char *text_delete_char(char *text, char c)
{
	return __text_delete_char(text, text, c);
}

static inline char *text_delete_sub(char *text, const char *sub)
{
	return __text_delete_sub(text, text, sub, text_len(sub));
}

static inline char value2char(int index)
{
	if (index < 10)
	{
		return index + '0';
	}

	return index - 10 + 'A';
}

static inline int is_empty_character(char c)
{
	return text_has_char(" \t\n\r", c);
}

static inline char char2uppercase(char c)
{
	return IS_LOWERCASE(c) ? c - 'a' + 'A' : c;
}

static inline char char2lowercase(char c)
{
	return IS_UPPERCASE(c) ? c - 'A' + 'a' : c;
}

