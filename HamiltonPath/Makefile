# hamiltonpath_release.sh

all: hamiltonpath_debug.sh
	chmod u+x $?
	chmod u+x ../fmake.sh

	@for i in $?; \
	do \
	  echo; \
	  echo Parsed target: $$i; \
	  echo; \
	  sh $$i; \
	done

chmod: hamiltonpath_debug.sh hamiltonpath_release.sh
	chmod u+x $?
	chmod u+x ../fmake.sh
	ln -s ../Source
