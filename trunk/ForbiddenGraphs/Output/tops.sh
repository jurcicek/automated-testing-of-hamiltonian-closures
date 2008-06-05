#! /bin/sh

for i in `find ./*.fg.dot`
do
	echo Procesed $i to $i.ps
	dot -Tps $i -o $i.ps
done