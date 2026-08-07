/* always quote the config dir, as it *will* contain spaces on Mac OS X */
#cmakedefine CONFIGDIR "${CONFIGDIR}"

#cmakedefine HAVE_STDINT_H 1
#cmakedefine HAVE_INTTYPES_H 1

#cmakedefine HAVE_BIGENDIAN 1

#cmakedefine HAVE_ARC4RANDOM 1

/* TODO add a check */
#define CAN_UNALIGNED 1

/* are we on a 64 bit system? */
#cmakedefine HOST_IS_64_BIT 1
