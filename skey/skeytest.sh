#!/bin/sh

out=`./key -p johndoe 88 ka9q2`
if test "${out}" != "NOLL AMRA FEE HOST BELA DEFT"; then
    echo ${out}
    exit 1
else
    echo ${out}
    exit 0;
fi