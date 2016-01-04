#!/bin/bash


if [ "" == "$1" ]
then
	echo "USAGE: $0 <name> <type> where <name> is the name to use for new test project (in CamelCase) and <type> (optional, default = test) is either test or stress"
	exit 1
fi

TYPE="test"
if [ "" != "$2" ]
then
	TYPE="$2"
fi

TYPE=$(echo "$TYPE" | tr '[:upper:]' '[:lower:]')

ORIGINAL="$1"
LOWER=$(echo "$ORIGINAL" | tr '[:upper:]' '[:lower:]')
UPPER=$(echo "$ORIGINAL" | tr '[:lower:]' '[:upper:]')
NEW="$TYPE$ORIGINAL"
TEMPLATE="${TYPE}Template"
echo "TYPE=$TYPE"
echo "NEW=$NEW"
echo "TEMPLATE=$TEMPLATE"
echo "NAME=$ORIGINAL ($UPPER, $LOWER)"
cp -a "$TEMPLATE"  "$NEW"

rename "s/Template/$ORIGINAL/" "$NEW"/*

for n in "$NEW"/*
do
	echo "$n"
	sed  -i  "s/TEMPLATE/$UPPER/g" "$n"
	sed  -i  "s/template/$LOWER/g" "$n"
	sed  -i  "s/Template/$ORIGINAL/g" "$n"
done

