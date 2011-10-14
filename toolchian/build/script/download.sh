#!/bin/bash

TEMP_DOWNLOAD_PATH="/tmp"

function wget_download()
{
	wget -P ${TEMP_DOWNLOAD_PATH} -t 5 -w 10 -c $1 || return 1

	return 0
}

function tftp_download()
{
	cavan-tftp_copy --ip=${SERVER_IP} --port=${SERVER_PORT} -r $1 ${TEMP_DOWNLOAD_PATH} || return 1

	return 0
}

function download_main()
{
	local pkg_name pkg_path

	pkg_name="$(basename $1)"
	pkg_path="${TEMP_DOWNLOAD_PATH}/${pkg_name}"

	if [ "${DOWNLOAD_WAY}" = "tftp" ]
	then
		tftp_download ${SERVER_SOURCE}/${pkg_name} ||
		{
			rm ${pkg_path} -rfv
			return 1
		}
	else
		wget_download $1 ||
		{
			rm ${pkg_path} -rfv
			return 1
		}
	fi

	mv ${pkg_path} . -v || return 1

	return 0
}

download_main $* || exit 1
