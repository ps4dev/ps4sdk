.DEFAULT_GOAL := all

%:
	$(MAKE) -C base $@ $?
	$(MAKE) -C common $@ $?
	$(MAKE) -C extension $@ $?
	$(MAKE) -C core $@ $?
