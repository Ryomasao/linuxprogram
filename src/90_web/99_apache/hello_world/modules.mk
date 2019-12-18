mod_hello_world.la: mod_hello_world.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_hello_world.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_hello_world.la
