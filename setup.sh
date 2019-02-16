#!/bin/sh

OPT="-DPRINT_VARIABLES=FALSE"

for i in "$@"
do
	case $i in
		-v|--verbose)
			OPT="-DPRINT_VARIABLES=TRUE"
	esac
done

cd build/ && cmake $OPT -G Xcode ..

for i in "$@"
do
	case $i in
		-o|--open)
			open erm.xcodeproj
	esac
	case $i in
		-f|--fastrun)
			xcodebuild && Debug/ERM
	esac
done
