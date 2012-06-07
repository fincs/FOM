
all:
	@make --no-print-directory -f fomlib.mk
	@make --no-print-directory -C bin/fomutil_src host
	@make --no-print-directory -C bin/fomutil_src ds

clean:
	@make --no-print-directory -f fomlib.mk clean
	@make --no-print-directory -C bin/fomutil_src hostclean
	@make --no-print-directory -C bin/fomutil_src dsclean

install:
	@make --no-print-directory -f fomlib.mk install
	@make --no-print-directory -C bin/fomutil_src host
	@make --no-print-directory -C bin/fomutil_src dsinstall
