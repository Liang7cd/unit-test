# -*- Makefile -*-

top_builddir = ..
builddir     = .
top_srcdir   = ../..
srcdir       = ../../extensions
ksourcedir   = 
prefix       = /home/l/share/unit_testing/iptables/iptables-1.8.4/build/install
exec_prefix  = ${prefix}
libdir       = ${exec_prefix}/lib
libexecdir   = ${exec_prefix}/libexec
xtlibdir     = ${exec_prefix}/lib/xtables

AR                 = ar
CC                 = arm-fullhanv3-linux-uclibcgnueabi-gcc
CCLD               = ${CC}
CFLAGS             = -g -O2
CPPFLAGS           = 
LDFLAGS            = 
regular_CFLAGS     = -Wall -Waggregate-return -Wmissing-declarations 	-Wmissing-prototypes -Wredundant-decls -Wshadow -Wstrict-prototypes 	-Wlogical-op 	-Winline -pipe
regular_CPPFLAGS   =  -D_REENTRANT 	-DXTABLES_LIBDIR=\"${xtlibdir}\" -DXTABLES_INTERNAL
kinclude_CPPFLAGS  = 

AM_CFLAGS       = ${regular_CFLAGS}
AM_CPPFLAGS     = ${regular_CPPFLAGS} -I${top_builddir}/include -I${top_builddir} -I${top_srcdir}/include -I${top_srcdir} ${kinclude_CPPFLAGS} ${CPPFLAGS}  
AM_DEPFLAGS     = -Wp,-MMD,$(@D)/.$(@F).d,-MT,$@
AM_LDFLAGS      = -Wl,--no-undefined

ifeq (${V},)
AM_LIBTOOL_SILENT = --silent
AM_VERBOSE_CC     = @echo "  CC      " $@;
AM_VERBOSE_CCLD   = @echo "  CCLD    " $@;
AM_VERBOSE_CXX    = @echo "  CXX     " $@;
AM_VERBOSE_CXXLD  = @echo "  CXXLD   " $@;
AM_VERBOSE_AR     = @echo "  AR      " $@;
AM_VERBOSE_GEN    = @echo "  GEN     " $@;
endif

#
#	Wildcard module list
#
pfx_build_mod := $(patsubst ${srcdir}/libxt_%.c,%,$(sort $(wildcard ${srcdir}/libxt_*.c)))
# pfb_build_mod := $(patsubst ${srcdir}/libebt_%.c,%,$(sort $(wildcard ${srcdir}/libebt_*.c)))
# pfa_build_mod := $(patsubst ${srcdir}/libarpt_%.c,%,$(sort $(wildcard ${srcdir}/libarpt_*.c)))
pfx_symlinks  := NOTRACK state
 pf4_build_mod := $(patsubst ${srcdir}/libipt_%.c,%,$(sort $(wildcard ${srcdir}/libipt_*.c)))
# pf6_build_mod := $(patsubst ${srcdir}/libip6t_%.c,%,$(sort $(wildcard ${srcdir}/libip6t_*.c)))
pfx_build_mod := $(filter-out  connlabel ,${pfx_build_mod})
pfb_build_mod := $(filter-out  connlabel  limit mark nflog mangle,${pfb_build_mod})
pfa_build_mod := $(filter-out  connlabel  mangle,${pfa_build_mod})
pf4_build_mod := $(filter-out  connlabel ,${pf4_build_mod})
pf6_build_mod := $(filter-out  connlabel ,${pf6_build_mod})
pfx_objs      := $(patsubst %,libxt_%.o,${pfx_build_mod})
pfb_objs      := $(patsubst %,libebt_%.o,${pfb_build_mod})
pfa_objs      := $(patsubst %,libarpt_%.o,${pfa_build_mod})
pf4_objs      := $(patsubst %,libipt_%.o,${pf4_build_mod})
pf6_objs      := $(patsubst %,libip6t_%.o,${pf6_build_mod})
pfx_solibs    := $(patsubst %,libxt_%.so,${pfx_build_mod})
pfb_solibs    := $(patsubst %,libebt_%.so,${pfb_build_mod})
pfa_solibs    := $(patsubst %,libarpt_%.so,${pfa_build_mod})
pf4_solibs    := $(patsubst %,libipt_%.so,${pf4_build_mod})
pf6_solibs    := $(patsubst %,libip6t_%.so,${pf6_build_mod})
pfx_symlink_files := $(patsubst %,libxt_%.so,${pfx_symlinks})


#
# Building blocks
#
targets := libext.a libext4.a libext6.a libext_ebt.a libext_arpt.a matches.man targets.man
targets_install :=
 libext_objs := ${pfx_objs}
 libext_ebt_objs := ${pfb_objs}
 libext_arpt_objs := ${pfa_objs}
 libext4_objs := ${pf4_objs}
 libext6_objs := ${pf6_objs}
# targets += ${pfx_solibs} ${pfb_solibs} ${pf4_solibs} ${pf6_solibs} ${pfa_solibs} ${pfx_symlink_files}
# targets_install += ${pfx_solibs} ${pfb_solibs} ${pf4_solibs} ${pf6_solibs} ${pfa_solibs}
# symlinks_install := ${pfx_symlink_files}

.SECONDARY:

.PHONY: all install clean distclean FORCE

all: ${targets}

install: ${targets_install} ${symlinks_install}
	@mkdir -p "${DESTDIR}${xtlibdir}";
	if test -n "${targets_install}"; then \
		install -pm0755 ${targets_install} "${DESTDIR}${xtlibdir}/"; \
	fi;
	if test -n "${symlinks_install}"; then \
		cp -P ${symlinks_install} "${DESTDIR}${xtlibdir}/"; \
	fi;

clean:
	rm -f *.o *.oo *.so *.a {matches,targets}.man initext.c initext4.c initext6.c initextb.c initexta.c;
	rm -f .*.d .*.dd;

distclean: clean

init%.o: init%.c
	${AM_VERBOSE_CC} ${CC} ${AM_CPPFLAGS} ${AM_DEPFLAGS} ${AM_CFLAGS} -D_INIT=$*_init ${CFLAGS} -o $@ -c $<;

-include .*.d


#
#	Shared libraries
#
lib%.so: lib%.oo
	${AM_VERBOSE_CCLD} ${CCLD} ${AM_LDFLAGS} ${LDFLAGS} -shared -o $@ $< -L../libxtables/.libs -lxtables ${$*_LIBADD};

lib%.oo: ${srcdir}/lib%.c
	${AM_VERBOSE_CC} ${CC} ${AM_CPPFLAGS} ${AM_DEPFLAGS} ${AM_CFLAGS} -D_INIT=lib$*_init -DPIC -fPIC ${CFLAGS} -o $@ -c $<;

libxt_NOTRACK.so: libxt_CT.so
	ln -fs $< $@
libxt_state.so: libxt_conntrack.so
	ln -fs $< $@

# Need the LIBADDs in iptables/Makefile.am too for libxtables_la_LIBADD
xt_RATEEST_LIBADD   = -lm
xt_statistic_LIBADD = -lm
xt_connlabel_LIBADD = 

#
#	Static bits
#
#	If static building is disabled, libext*.a will still be generated,
#	but will be empty. This is good since we can do with less case
#	handling code in the Makefiles.
#
lib%.o: ${srcdir}/lib%.c
	${AM_VERBOSE_CC} ${CC} ${AM_CPPFLAGS} ${AM_DEPFLAGS} ${AM_CFLAGS} -DNO_SHARED_LIBS=1 -D_INIT=lib$*_init ${CFLAGS} -o $@ -c $<;

libext.a: initext.o ${libext_objs}
	${AM_VERBOSE_AR} ${AR} crs $@ $^;

libext_ebt.a: initextb.o ${libext_ebt_objs}
	${AM_VERBOSE_AR} ${AR} crs $@ $^;

libext_arpt.a: initexta.o ${libext_arpt_objs}
	${AM_VERBOSE_AR} ${AR} crs $@ $^;

libext4.a: initext4.o ${libext4_objs}
	${AM_VERBOSE_AR} ${AR} crs $@ $^;

libext6.a: initext6.o ${libext6_objs}
	${AM_VERBOSE_AR} ${AR} crs $@ $^;

initext_func  := $(addprefix xt_,${pfx_build_mod})
initextb_func := $(addprefix ebt_,${pfb_build_mod})
initexta_func := $(addprefix arpt_,${pfa_build_mod})
initext4_func := $(addprefix ipt_,${pf4_build_mod})
initext6_func := $(addprefix ip6t_,${pf6_build_mod})

.initext.dd: FORCE
	@echo "${initext_func}" >$@.tmp; \
	cmp -s $@ $@.tmp || mv $@.tmp $@; \
	rm -f $@.tmp;

.initextb.dd: FORCE
	@echo "${initextb_func}" >$@.tmp; \
	cmp -s $@ $@.tmp || mv $@.tmp $@; \
	rm -f $@.tmp;

.initexta.dd: FORCE
	@echo "${initexta_func}" >$@.tmp; \
	cmp -s $@ $@.tmp || mv $@.tmp $@; \
	rm -f $@.tmp;

.initext4.dd: FORCE
	@echo "${initext4_func}" >$@.tmp; \
	cmp -s $@ $@.tmp || mv $@.tmp $@; \
	rm -f $@.tmp;

.initext6.dd: FORCE
	@echo "${initext6_func}" >$@.tmp; \
	cmp -s $@ $@.tmp || mv $@.tmp $@; \
	rm -f $@.tmp;

initext.c: .initext.dd
	${AM_VERBOSE_GEN}
	@( \
	echo "" >$@; \
	for i in ${initext_func}; do \
		echo "extern void lib$${i}_init(void);" >>$@; \
	done; \
	echo "void init_extensions(void);" >>$@; \
	echo "void init_extensions(void)" >>$@; \
	echo "{" >>$@; \
	for i in ${initext_func}; do \
		echo  " ""lib$${i}_init();" >>$@; \
	done; \
	echo "}" >>$@; \
	);

initextb.c: .initextb.dd
	${AM_VERBOSE_GEN}
	@( \
	echo "" >$@; \
	for i in ${initextb_func}; do \
		echo "extern void lib$${i}_init(void);" >>$@; \
	done; \
	echo "void init_extensionsb(void);" >>$@; \
	echo "void init_extensionsb(void)" >>$@; \
	echo "{" >>$@; \
	for i in ${initextb_func}; do \
		echo  " ""lib$${i}_init();" >>$@; \
	done; \
	echo "}" >>$@; \
	);

initexta.c: .initexta.dd
	${AM_VERBOSE_GEN}
	@( \
	echo "" >$@; \
	for i in ${initexta_func}; do \
		echo "extern void lib$${i}_init(void);" >>$@; \
	done; \
	echo "void init_extensionsa(void);" >>$@; \
	echo "void init_extensionsa(void)" >>$@; \
	echo "{" >>$@; \
	for i in ${initexta_func}; do \
		echo  " ""lib$${i}_init();" >>$@; \
	done; \
	echo "}" >>$@; \
	);

initext4.c: .initext4.dd
	${AM_VERBOSE_GEN}
	@( \
	echo "" >$@; \
	for i in ${initext4_func}; do \
		echo "extern void lib$${i}_init(void);" >>$@; \
	done; \
	echo "void init_extensions4(void);" >>$@; \
	echo "void init_extensions4(void)" >>$@; \
	echo "{" >>$@; \
	for i in ${initext4_func}; do \
		echo  " ""lib$${i}_init();" >>$@; \
	done; \
	echo "}" >>$@; \
	);

initext6.c: .initext6.dd
	${AM_VERBOSE_GEN}
	@( \
	echo "" >$@; \
	for i in ${initext6_func}; do \
		echo "extern void lib$${i}_init(void);" >>$@; \
	done; \
	echo "void init_extensions6(void);" >>$@; \
	echo "void init_extensions6(void)" >>$@; \
	echo "{" >>$@; \
	for i in ${initext6_func}; do \
		echo " ""lib$${i}_init();" >>$@; \
	done; \
	echo "}" >>$@; \
	);

#
#	Manual pages
#
ex_matches = $(shell echo ${1} | LC_ALL=POSIX grep -Eo '\b[[:lower:][:digit:]_]+\b')
ex_targets = $(shell echo ${1} | LC_ALL=POSIX grep -Eo '\b[[:upper:][:digit:]_]+\b')
man_run    = \
	${AM_VERBOSE_GEN} \
	for ext in $(sort ${1}); do \
		f="${srcdir}/libxt_$$ext.man"; \
		if [ -f "$$f" ]; then \
			echo -e "\t+ $$f" >&2; \
			echo ".SS $$ext"; \
			cat "$$f" || exit $$?; \
		fi; \
		f="${srcdir}/libip6t_$$ext.man"; \
		if [ -f "$$f" ]; then \
			echo -e "\t+ $$f" >&2; \
			echo ".SS $$ext (IPv6-specific)"; \
			cat "$$f" || exit $$?; \
		fi; \
		f="${srcdir}/libipt_$$ext.man"; \
		if [ -f "$$f" ]; then \
			echo -e "\t+ $$f" >&2; \
			echo ".SS $$ext (IPv4-specific)"; \
			cat "$$f" || exit $$?; \
		fi; \
	done >$@;

matches.man: .initext.dd .initextb.dd .initexta.dd .initext4.dd .initext6.dd $(wildcard ${srcdir}/lib*.man)
	$(call man_run,$(call ex_matches,${pfx_build_mod} ${pfb_build_mod} ${pfa_build_mod} ${pf4_build_mod} ${pf6_build_mod} ${pfx_symlinks}))

targets.man: .initext.dd .initextb.dd .initexta.dd .initext4.dd .initext6.dd $(wildcard ${srcdir}/lib*.man)
	$(call man_run,$(call ex_targets,${pfx_build_mod} ${pfb_build_mod} ${pfa_build_mod} ${pf4_build_mod} ${pf6_build_mod} ${pfx_symlinks}))
