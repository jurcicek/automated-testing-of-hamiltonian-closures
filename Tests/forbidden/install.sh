#! /bin/sh

echo "I'am installing execution links"

chmod u+x run.sh
OLDPWD=`pwd`

for i in `find ./ | grep -v 'CVS' | grep -v 'Output' | grep -v '^./$'`
do
    if [ -d $i ] 
    then
	echo "Destination directory: $i"
	cd $i	

	rm -f run.sh
	rm -f forbiddengraphs_release
	rm -f forbiddengraphs_release_2
	rm -f forbiddengraphs_release_3
	rm -f forbiddengraphs_release_4
	rm -f forbiddengraphs_release_5
	rm -f forbiddengraphs_release_6
#	cp -f $OLDPWD/.cvsignore .cvsignore 
	ln -s $OLDPWD/run.sh run.sh
	ln -s $OLDPWD/forbiddengraphs_release_6 forbiddengraphs_release_6
	
	cd $OLDPWD
    fi
done

