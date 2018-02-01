ODP_VISIBILITY
ODP_ATOMIC

# linux-generic PCAP support is not relevant as the code doesn't use
# linux-generic pktio at all. And DPDK has its own PCAP support anyway
AM_CONDITIONAL([HAVE_PCAP], [false])
m4_include([platform/linux-dpdk/m4/odp_pthread.m4])
ODP_TIMER
ODP_OPENSSL
m4_include([platform/linux-dpdk/m4/odp_schedule.m4])

##########################################################################
# DPDK build variables
##########################################################################
DPDK_DRIVER_DIR=/usr/lib/$(uname -m)-linux-gnu
AS_CASE($host_cpu, [x86_64], [DPDK_CPPFLAGS="-msse4.2"])
if test ${DPDK_DEFAULT_DIR} = 1; then
    DPDK_CFLAGS="-include /usr/include/dpdk/rte_config.h"
    DPDK_CPPFLAGS="$DPDK_CPPFLAGS -I/usr/include/dpdk"
else
    DPDK_DRIVER_DIR=$SDK_INSTALL_PATH/lib
    DPDK_CFLAGS="-include $SDK_INSTALL_PATH/include/rte_config.h"
    DPDK_CPPFLAGS="$DPDK_CPPFLAGS -I$SDK_INSTALL_PATH/include"
fi

# Check if we should link against the static or dynamic DPDK library
AC_ARG_ENABLE([shared-dpdk],
	[  --enable-shared-dpdk    link against the shared DPDK library],
	[if test "x$enableval" = "xyes"; then
		shared_dpdk=true
	fi])
AM_CONDITIONAL([SHARED_DPDK], [test x$shared_dpdk = xtrue])

##########################################################################
# Save and set temporary compilation flags
##########################################################################
OLD_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$DPDK_CPPFLAGS $CPPFLAGS"

##########################################################################
# Check for DPDK availability
##########################################################################
AC_CHECK_HEADERS([rte_config.h], [],
    [AC_MSG_FAILURE(["can't find DPDK headers"])])

##########################################################################
# In case of static linking DPDK pmd drivers are not linked unless the
# --whole-archive option is used. No spaces are allowed between the
# --whole-arhive flags.
##########################################################################
if test "x$shared_dpdk" = "xtrue"; then
    DPDK_LIBS="-Wl,--no-as-needed,-ldpdk,-as-needed -ldl -lm -lpcap"
else
    AS_VAR_SET([DPDK_PMDS], [-Wl,--whole-archive,])
    for filename in "$DPDK_DRIVER_DIR"/librte_pmd_*.a; do
        cur_driver=`basename "$filename" .a | sed -e 's/^lib//'`
        # rte_pmd_nfp has external dependencies which break linking
        if test "$cur_driver" = "rte_pmd_nfp"; then
            echo "skip linking rte_pmd_nfp"
        else
            AS_VAR_APPEND([DPDK_PMDS], [-l$cur_driver,])
        fi
    done
    AS_VAR_APPEND([DPDK_PMDS], [--no-whole-archive])
    DPDK_LIBS="-L$DPDK_DRIVER_DIR/ -ldpdk -lpthread -ldl -lm -lpcap"
fi

AC_SUBST([DPDK_CFLAGS])
AC_SUBST([DPDK_CPPFLAGS])
AC_SUBST([DPDK_LIBS])
AC_SUBST([DPDK_PMDS])

##########################################################################
# Restore old saved variables
##########################################################################
CPPFLAGS=$OLD_CPPFLAGS

AC_CONFIG_FILES([platform/linux-dpdk/Makefile
		 platform/linux-dpdk/libodp-dpdk.pc
		 platform/linux-dpdk/include/odp/api/plat/static_inline.h
		 platform/linux-dpdk/test/Makefile
		 platform/linux-dpdk/test/validation/api/pktio/Makefile])
