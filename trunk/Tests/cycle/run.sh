#! /bin/sh

hostname > hostname.log

rm -f nohup.out
nohup `pwd`/hamiltoncycle_release &

