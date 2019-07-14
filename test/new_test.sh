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
	echo "    <type> is either test, stress or profile (optional, default = test)"
	echo ""
	exit 1
fi

if ! hash rename 2>/dev/null
then
	echo "Could not find rename command. Please install it with 'sudo apt install rename' or similar before using this script. Aborting..."
	exit 1
fi

TYPE="test"
if [ "" != "$2" ]
then
	TYPE="$2"
fi


case $TYPE in
	test ) ;;
	stress ) ;;
	profile ) ;;
	*) echo "Unknown test type: '$TYPE', Aborting..."; exit 1; ;;
esac

cd $(dirname $0)

TYPE=$(echo "$TYPE" | tr '[:upper:]' '[:lower:]')

ORIGINAL="$1"
LOWER=$(echo "$ORIGINAL" | tr '[:upper:]' '[:lower:]')
UPPER=$(echo "$ORIGINAL" | tr '[:lower:]' '[:upper:]')
NEW="$TYPE$ORIGINAL"
TEMPLATE="${TYPE}Template"
TEMPLATE_DIR="templates/${TEMPLATE}"
echo "======================================================"
echo "== TYPE=$TYPE"
echo "== NEW=$NEW"
echo "== TEMPLATE=$TEMPLATE"
echo "== NAME=$ORIGINAL ($UPPER, $LOWER)"
echo "======================================================"

cp -a "$TEMPLATE_DIR"  "$NEW"

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
