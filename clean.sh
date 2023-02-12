#!/bin/sh
set -e
. ./config.sh

echo config done
for PROJECT in $PROJECTS; do
  echo $PROJECT
  (cd $PROJECT && $MAKE clean)
done

rm -rf sysroot 
rm -rf isodir
rm -rf jurasos.iso