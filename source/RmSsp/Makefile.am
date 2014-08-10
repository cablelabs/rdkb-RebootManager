AM_CFLAGS = -D_ANSC_LINUX
AM_CFLAGS += -D_ANSC_USER
AM_CFLAGS += -D_ANSC_LITTLE_ENDIAN_
AM_CFLAGS += -D_COSA_SIM_
AM_LDFLAGS = -L$(top_builddir)/../CcspCommonLibrary/lib
AM_LDFLAGS += -lccsp_common

AM_CPPFLAGS = -Wall -Werror
ACLOCAL_AMFLAGS = -I m4
hardware_platform = i686-linux-gnu
bin_PROGRAMS = CcspRmSsp
CcspRmSsp_CPPFLAGS = -I/var/tmp/pc-rdkb/include -I/var/tmp/pc-rdkb/include/dbus-1.0 -I$(top_builddir)/../CcspCommonLibrary/source/ccsp/custom -I$(top_builddir)/../CcspCommonLibrary/source/ccsp/include -I$(top_builddir)/../CcspCommonLibrary/source/debug_api/include -I$(top_builddir)/../CcspCommonLibrary/source/cosa/include -I$(top_builddir)/../CcspCommonLibrary/source/cosa/include/linux -I$(top_builddir)/../CcspCommonLibrary/source/ccsp/components/include -I$(top_builddir)/../CcspCommonLibrary/source/cosa/package/slap/include -I$(top_builddir)/../CcspCommonLibrary/source/util_api/ansc/include -I$(top_builddir)/../CcspCommonLibrary/source/ccsp/components/common/MessageBusHelper/include -I$(top_builddir)/../CcspCommonLibrary/source/util_api/http/include -I$(top_builddir)/../CcspCommonLibrary/source/util_api/ansc/include -I$(top_builddir)/../CcspCommonLibrary/source/util_api/tls/include -I$(top_builddir)/../CcspCommonLibrary/source/util_api/web/include -I$(top_builddir)/../CcspCommonLibrary/source/util_api/stun/include -I$(top_builddir)/source/DataModelBackEnd $(CPPFLAGS)
CcspRmSsp_SOURCES = ssp_messagebus_interface.c ssp_main.c ssp_action.c ../../source-pc/CcspRmHal.c ../../source/DataModelBackEnd/cosa_apis_reboot.c ../../source/DataModelBackEnd/plugin_main.c ../../source/DataModelBackEnd/rm_util.c ../../source/DataModelBackEnd/cosa_apis_configuration.c
CcspRmSsp_LDFLAGS =  -L$(top_builddir)/../CcspCommonLibrary/lib -lccsp_common
