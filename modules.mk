mod_ifdefine_extended.la: mod_ifdefine_extended.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_ifdefine_extended.lo -ljson
DISTCLEAN_TARGETS = modules.mk
shared =  mod_ifdefine_extended.la
