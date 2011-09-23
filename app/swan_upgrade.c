#include <cavan.h>
#include <cavan/swan_upgrade.h>
#include <cavan/device.h>
#include <cavan/swan_pkg.h>
#include <cavan/swan_dev.h>

static void show_usage(void)
{
	println("Usage:");
	println("swan_upgrade -c [-f output_file] [-d file_dir]");
	println("swan_upgrade -x [-f upgrade_file] [-d output_dir]");
	println("--help");
	println("--label, --volume: specify vfat partition volume label");
	println("--i200, --I200: set machine to i200");
	println("--i600, --I600: set machine to i600");
	println("--type: i200, i600");
	println("--wipe-vfat: no, yes, 0, 1");
	println("--wipe-data: no, yes, 0, 1");
	println("--wipe-cache: no, yes, 0, 1");
	println("--check-version: no, yes, 0, 1");
	println("--shrink, --resize: no, yes, 0, 1");
	println("--skip, --exclude: exclude some images");
}

int main(int argc, char *argv[])
{
	int option_index;
	int c;
	char dir_path[128];
	char file_path[128];
	int (*handle)(const char *, const char *);
	struct option long_option[] =
	{
		{
			.name = "label",
			.has_arg = required_argument,
			.flag = NULL,
			.val = 0,
		},
		{
			.name = "volume",
			.has_arg = required_argument,
			.flag = NULL,
			.val = 0,
		},
		{
			.name = "i200",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 1,
		},
		{
			.name = "I200",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 1,
		},
		{
			.name = "i600",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 2,
		},
		{
			.name = "I600",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 2,
		},
		{
			.name = "modem",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 3,
		},
		{
			.name = "wipe-vfat",
			.has_arg = optional_argument,
			.flag = NULL,
			.val = 4,
		},
		{
			.name = "wipe-data",
			.has_arg = optional_argument,
			.flag = NULL,
			.val = 5,
		},
		{
			.name = "wipe-cache",
			.has_arg = optional_argument,
			.flag = NULL,
			.val = 6,
		},
		{
			.name = "check-version",
			.has_arg = optional_argument,
			.flag = NULL,
			.val = 7,
		},
		{
			.name = "exclude",
			.has_arg = required_argument,
			.flag = NULL,
			.val = 8,
		},
		{
			.name = "skip",
			.has_arg = required_argument,
			.flag = NULL,
			.val = 8,
		},
		{
			.name = "resize",
			.has_arg = required_argument,
			.flag = NULL,
			.val = 9,
		},
		{
			.name = "shrink",
			.has_arg = required_argument,
			.flag = NULL,
			.val = 9,
		},
		{
			.name = "type",
			.has_arg = required_argument,
			.flag = NULL,
			.val = 't',
		},
		{
			.name = "help",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'h',
		},
		{
		},
	};

	println_green("Upgrade Program \"" __FILE__ "\" Build Date: " __DATE__  " "  __TIME__);

	if (argc == 1)
	{
		return auto_upgrade(NULL);
	}

	if (argc == 2 && argv[1][0] != '-')
	{
		if (file_test(argv[1], "f") == 0)
		{
			return auto_upgrade(argv[1]);
		}
	}

	dir_path[0] = 0;
	file_path[0] = 0;
	handle = NULL;

	while ((c = getopt_long(argc, argv, "vVcCuUxXSsf:F:d:D:hHt:T:", long_option, &option_index)) != EOF)
	{
		switch (c)
		{
		case 0:
			strcpy(swan_vfat_volume, optarg);
			println("new label is: %s", optarg);
			break;

		case 1:
			swan_machine_type = SWAN_BOARD_I200;
			break;

		case 2:
			swan_machine_type = SWAN_BOARD_I600;
			break;

		case 3:
			swan_machine_type = SWAN_BOARD_I200;
			swan_package_flags |= SWAN_FLAGS_MODEM_UPGRADE;
			break;

		case 4:
			if (text_bool_value(optarg))
			{
				println_blue("Format VFAT partition");
				swan_mkfs_mask |= MKFS_MASK_VFAT;
			}
			else
			{
				println_blue("Don't format VFAT partition");
				swan_mkfs_mask &= ~MKFS_MASK_VFAT;
			}
			break;

		case 5:
			if (text_bool_value(optarg))
			{
				println_blue("Format userdata partition");
				swan_mkfs_mask |= MKFS_MASK_USERDATA;
			}
			else
			{
				println_blue("Don't format userdata partition");
				swan_exclude_images[swan_exclude_image_count++] = SWAN_IMAGE_USERDATA;
				swan_mkfs_mask &= ~MKFS_MASK_USERDATA;
			}
			break;

		case 6:
			if (text_bool_value(optarg))
			{
				println_blue("Format cache partition");
				swan_mkfs_mask |= MKFS_MASK_CACHE;
			}
			else
			{
				println_blue("Don't format cache partition");
				swan_mkfs_mask &= ~MKFS_MASK_CACHE;
			}
			break;

		case 7:
			if (text_bool_value(optarg))
			{
				println_blue("Open version check");
				swan_upgrade_flags |= UPGRADE_FLAG_CHECK_VERSION;
			}
			else
			{
				println_blue("Close version check");
				swan_upgrade_flags &= ~UPGRADE_FLAG_CHECK_VERSION;
			}
			break;

		case 8:
			swan_exclude_images[swan_exclude_image_count] = get_swan_image_type_by_name(optarg);
			if (swan_exclude_images[swan_exclude_image_count] == SWAN_IMAGE_UNKNOWN)
			{
				error_msg("Unknown image type \"%s\"", optarg);
				return -EINVAL;
			}
			else
			{
				swan_exclude_image_count++;
			}
			println_blue("Exclude image: \"%s\"", optarg);
			break;

		case 9:
			if (text_bool_value(optarg))
			{
				println_blue("Don't shrink images when package");
				swan_need_shrink = 1;
			}
			else
			{
				println_blue("Shrink images when package");
				swan_need_shrink = 0;
			}
			break;

		case 't':
		case 'T':
			if (text_rhcmp(optarg, "i200") == 0 || text_rhcmp(optarg, "I200") == 0)
			{
				swan_machine_type = SWAN_BOARD_I200;
			}
			else if (text_rhcmp(optarg, "i600") == 0 || text_rhcmp(optarg, "I600") == 0)
			{
				swan_machine_type = SWAN_BOARD_I600;
			}
			else
			{
				swan_machine_type = SWAN_BOARD_UNKNOWN;
			}
			break;

		case 'f':
		case 'F':
			strcpy(file_path, optarg);
			break;

		case 'd':
		case 'D':
			strcpy(dir_path, optarg);
			break;

		case 'c':
		case 'C':
			handle = package;
			break;

		case 'u':
		case 'U':
			handle = upgrade;
			break;

		case 's':
		case 'S':
			handle = upgrade_simple;
			break;

		case 'x':
		case 'X':
			handle = unpack;
			break;

		case 'v':
		case 'V':
			println("Program Build Date: %s %s", __DATE__, __TIME__);
			return 0;

		case 'h':
		case 'H':
			show_usage();
			return 0;

		default:
			error_msg("Unknown option %c", c);
			show_usage();
			return -EINVAL;
		}
	}

	if (handle == NULL)
	{
		error_msg("No function handle this action");
		show_usage();
		return -EINVAL;
	}

	if (file_path[0] == 0)
	{
		for (option_index = optind; option_index < argc; option_index++)
		{
			if (file_test(argv[option_index], "f") == 0)
			{
				strcpy(file_path, argv[option_index]);
			}
		}

		if (file_path[0] == 0)
		{
			strcpy(file_path, UPGRADE_FILE_NAME);
		}
	}

	if (dir_path[0] == 0)
	{
		for (option_index = optind; option_index < argc; option_index++)
		{
			if (file_test(argv[option_index], "d") == 0 || file_test(argv[option_index], "e") < 0)
			{
				strcpy(dir_path, argv[option_index]);
			}
		}

		if (dir_path[0] == 0)
		{
			strcpy(dir_path, ".");
		}
	}

	return handle(file_path, dir_path);
}
