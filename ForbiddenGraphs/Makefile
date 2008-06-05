# forbiddengraphs_release.sh

all: forbiddengraphs_debug.sh
	chmod u+x $?
	chmod u+x ../fmake.sh

	@for i in $?; \
	do \
	  echo; \
	  echo Parsed target: $$i; \
	  echo; \	  
	  sh $$i; \
	done

chmod: forbiddengraphs_debug.sh forbiddengraphs_release.sh
	chmod u+x $?
	chmod u+x ../fmake.sh
	ln -s ../Source
