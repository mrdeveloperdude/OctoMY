#!/bin/bash


if [ "" == "$1" ]
then
	echo "$0: Create a new test from template."
	echo ""
	echo "NOTE: Will add the generated files to git"
	echo "NOTE: Will add reference to the new test to test.pro"
	echo ""
	echo "USAGE: $0 <name> <type>"
	echo ""
	echo "  where"
	echo ""
	echo "    <name> is the name to use for new test project (in CamelCase)"
	echo ""
	echo "  and"
	echo ""
	echo "    <type> is either test or stress (optional, default = test)"
	echo ""
	exit 1
fi


TYPE="test"
if [ "" != "$2" ]
then
	TYPE="$2"
fi

cd $(dirname $0)

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


from="\tmessage(ADDING BASIC TESTS TO BUILD)\n\tTEST_PROJECTS+= \\\\\n"
to="${from}\t\ttest$ORIGINAL \\\\\n"

sed  -i -z  "s/$from/$to/g" 'test.pro'

git add "$NEW"/* "test.pro"
