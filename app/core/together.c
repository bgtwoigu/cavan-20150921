// This File Is Automatically Generated By FuangCao
// Fuang.Cao <cavan.cfa@gmail.com>
// Create date: 2011-09-24 03:01:54

#include <cavan.h>
#include <cavan/command.h>
#include <cavan_map.h>

const struct cavan_command_map cmd_map_table[] =
{
#include <cavan_map.c>
};

int main(int argc, char *argv[])
{
	return find_and_exec_command(cmd_map_table, ARRAY_SIZE(cmd_map_table), argc, argv);
}
