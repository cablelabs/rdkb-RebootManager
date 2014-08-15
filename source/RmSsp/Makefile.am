AM_CFLAGS = -D_ANSC_LINUX
AM_CFLAGS += -D_ANSC_USER
AM_CFLAGS += -D_ANSC_LITTLE_ENDIAN_
AM_CFLAGS += -D_COSA_SIM_
AM_LDFLAGS = -lccsp_common

AM_CPPFLAGS = -Wall -Werror
ACLOCAL_AMFLAGS = -I m4
hardware_platform = i686-linux-gnu
bin_PROGRAMS = CcspRmSsp
CcspRmSsp_CPPFLAGS = -I$(top_srcdir)/source/DataModelBackEnd $(CPPFLAGS)
CcspRmSsp_SOURCES = ssp_messagebus_interface.c ssp_main.c ssp_action.c ../../source-pc/CcspRmHal.c ../../source/DataModelBackEnd/cosa_apis_reboot.c ../../source/DataModelBackEnd/plugin_main.c ../../source/DataModelBackEnd/rm_util.c ../../source/DataModelBackEnd/cosa_apis_configuration.c
CcspRmSsp_LDFLAGS =  -lccsp_common
