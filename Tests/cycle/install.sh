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
	rm -f hamiltoncycle_release
#	cp -f $OLDPWD/.cvsignore .cvsignore 
	ln -s $OLDPWD/run.sh run.sh
	ln -s $OLDPWD/hamiltoncycle_release hamiltoncycle_release
	
	cd $OLDPWD
    fi
done

