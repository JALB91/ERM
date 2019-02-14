#!/bin/sh

cd build/ && cmake -G Xcode ..

for i in "$@"
do
	case $i in
		-o|--open)
			open erm.xcodeproj
	esac
	case $i in
		-f|--fastrun)
			xcodebuild && Debug/erm
	esac
done
