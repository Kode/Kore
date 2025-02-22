#!/usr/bin/env bash

. `dirname "$0"`/tools/platform.sh
MAKE="`dirname "$0"`/tools/$KORE_PLATFORM/kmake$KORE_EXE_SUFFIX"

if [ -f "$MAKE" ]; then
	exec $MAKE "$@"
else 
	echo "kmake was not found, please run the get_dlc script."
	exit 1
fi
