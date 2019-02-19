#!/bin/sh

OPT="-DPRINT_VARIABLES=FALSE"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

for i in "$@"
do
	case $i in
		-v|--verbose)
			OPT="-DPRINT_VARIABLES=TRUE"
	esac
done

cd ${DIR}/build/ && cmake $OPT -G Xcode ..

for i in "$@"
do
	case $i in
		-o|--open)
			open erm.xcodeproj
	esac
	case $i in
		-f|--fastrun)
			xcodebuild -target ERM && cd Debug/ && ./ERM
	esac
done
