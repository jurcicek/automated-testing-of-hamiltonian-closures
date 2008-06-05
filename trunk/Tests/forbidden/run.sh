#! /bin/sh

hostname > hostname.log

rm -f nohup.out
nohup `pwd`/forbiddengraphs_release_6 &

