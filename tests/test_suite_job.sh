#!/bin/sh

#-----------where are we?
case "$0" in
/*) dir=`dirname $0` ;;
*)  dir=`dirname $PWD/$0`;;
esac
dir=`dirname $dir`
echo "Found myself in $dir"
#-----------


# need this for our real username
export USER=`basename $CONDOR_TMP`

# setup
source /grid/fermiapp/products/common/etc/setups.sh
cd $dir
setup -P -r $dir -M ups -m ifdhc.table ifdhc

cd $dir/tests &&  echo "I am in $dir/tests"

echo "Environment:"
echo "-------------"
printenv
echo "-------------"

python cpTests.py

#python testSuite.py

