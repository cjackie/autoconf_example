#!/bin/sh

out=`./key -p 111 88 ka9q2`
if test "${out}" != "BOO MALT WART FINK ONTO ANA"; then
    echo ${out}
    exit 1
else
    echo ${out}
    exit 0;
fi