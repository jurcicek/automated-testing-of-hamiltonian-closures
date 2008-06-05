# hamiltoncycle_release.sh

all: hamiltoncycle_debug.sh
	chmod u+x $?
	chmod u+x ../fmake.sh

	@for i in $?; \
	do \
	  echo; \
	  echo Parsed target: $$i; \
	  echo; \
	  sh $$i; \
	done

chmod: hamiltoncycle_debug.sh hamiltoncycle_release.sh
	chmod u+x $?
	chmod u+x ../fmake.sh
	ln -s ../Source
