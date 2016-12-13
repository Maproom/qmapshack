#cmakedefine PACKAGE ${APPLICATION_NAME}
#cmakedefine VERSION ${APPLICATION_VERSION}
#cmakedefine LOCALEDIR ${LOCALE_INSTALL_DIR}
#cmakedefine DATADIR ${SHARE_INSTALL_PREFIX}
#cmakedefine LIBDIR ${LIB_INSTALL_DIR}
#cmakedefine PLUGINDIR ${PLUGINDIR}
#cmakedefine SYSCONFDIR ${SYSCONFDIR}
#cmakedefine BINARYDIR ${BINARYDIR}
#cmakedefine SOURCEDIR ${SOURCEDIR}

/* always quote the config dir, as it *will* contain spaces on Mac OS X */
#cmakedefine CONFIGDIR "${CONFIGDIR}"

#cmakedefine SHARED_LIB_EXT ${SHARED_LIB_EXT}

#cmakedefine HAVE_STDINT_H 1
#cmakedefine HAVE_INTTYPES_H 1
#cmakedefine HAVE_BYTESWAP_H 1

#cmakedefine HAVE_BIGENDIAN 1

#cmakedefine HAVE_ARC4RANDOM 1

/* build with native file dialogs */
#define FILE_DIALOG_FLAGS ${FILEDIALOGFLAGS}

/* TODO add a check */
#define CAN_UNALIGNED 1

/* are we on a 64 bit system? */
#cmakedefine HOST_IS_64_BIT 1
