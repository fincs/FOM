package: all
	@[ -d __pack ] || mkdir -p __pack
	@cp $(TARGET).fx2 __pack
	@cp $(TARGET).fomreg __pack
	@fartool __pack $(TARGET).fom
	@rm -rf __pack

install: all
	@cp $(TARGET).fx2 $(FEOSDEST)/data/FeOS/lib/$(TARGET).fx2 || exit 1
	@$(FEOSUSERLIB)/FOM/bin/fomutil reg $(TARGET).fomreg || exit 1
