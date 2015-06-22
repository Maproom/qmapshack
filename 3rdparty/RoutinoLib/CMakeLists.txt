
option(ROUTINO_BUILD_SLIM_VERSION "Routino: Build slim version" OFF)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty/Routino/src
)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -Dinline=__inline)
endif(MSVC)

if(UNIX)
add_definitions(-Wall -std=c99 -ffast-math -D_FILE_OFFSET_BITS=64 -D_POSIX_C_SOURCE=200809L -pedantic)
endif(UNIX)

add_definitions(-DROUTINO_DATADIR="./" -DLIBROUTINO)

if(ROUTINO_BUILD_SLIM_VERSION)
    add_definitions(-DSLIM=1)
else(ROUTINO_BUILD_SLIM_VERSION)
    add_definitions(-DSLIM=0)
endif(ROUTINO_BUILD_SLIM_VERSION)

########### routino library ###########
set(APPNAME routino)

set(SRCS_${APPNAME}
    routino.c
    binout.c
    ../Routino/src/fakes.c
    ../Routino/src/files.c
    ../Routino/src/logging.c
    ../Routino/src/nodes.c
    ../Routino/src/optimiser.c
    ../Routino/src/profiles.c
    ../Routino/src/queue.c
    ../Routino/src/relations.c   
    ../Routino/src/results.c
    ../Routino/src/segments.c
    ../Routino/src/translations.c
    ../Routino/src/types.c
    ../Routino/src/ways.c
    ../Routino/src/xmlparse.c
)

if(MSVC)
set(SRCS_${APPNAME}
    ${SRCS_${APPNAME}}
    ../Routino/src/mman-win32.c
)
endif(MSVC)

set(HDRS_${APPNAME}
    routino.h
    binout.h
    ../Routino/src/cache.h
    ../Routino/src/fakes.h
    ../Routino/src/files.h
    ../Routino/src/functions.h
    ../Routino/src/logging.h
    ../Routino/src/nodes.h
    ../Routino/src/profiles.h    
    ../Routino/src/relations.h    
    ../Routino/src/results.h
    ../Routino/src/segments.h
    ../Routino/src/translations.h
    ../Routino/src/types.h
    ../Routino/src/ways.h
    ../Routino/src/xmlparse.h
)

if(MSVC)
set(HDRS_${APPNAME}
    ${HDRS_${APPNAME}}
    ../Routino/src/mman-win32.h
)
endif(MSVC)


set(ALLINP_${APPNAME}
    ${SRCS_${APPNAME}}
    ${HDRS_${APPNAME}}
)

add_library(${APPNAME} STATIC ${ALLINP_${APPNAME}})
if(UNIX)
target_link_libraries(${APPNAME} m)
endif(UNIX)


########### planet splitter ###########

set(APPNAME planetsplitter)

set(SRCS_${APPNAME}
    ../Routino/src/planetsplitter.c
    ../Routino/src/errorlog.c
    ../Routino/src/errorlogx.c
    ../Routino/src/logerror.c
    ../Routino/src/nodesx.c
    ../Routino/src/osmo5mparse.c
    ../Routino/src/osmparser.c
    ../Routino/src/osmpbfparse.c
    ../Routino/src/osmxmlparse.c
    ../Routino/src/prunex.c
    ../Routino/src/relationsx.c
    ../Routino/src/segmentsx.c
    ../Routino/src/sorting.c
    ../Routino/src/superx.c
    ../Routino/src/tagging.c
    ../Routino/src/uncompress.c
    ../Routino/src/visualiser.c
    ../Routino/src/waysx.c
)

set(HDRS_${APPNAME}
    ../Routino/src/errorlog.h
    ../Routino/src/errorlogx.h
    ../Routino/src/nodesx.h
    ../Routino/src/osmparser.h
    ../Routino/src/prunex.h
    ../Routino/src/relationsx.h
    ../Routino/src/segmentsx.h
    ../Routino/src/sorting.h
    ../Routino/src/superx.h
    ../Routino/src/tagging.h
    ../Routino/src/typesx.h
    ../Routino/src/uncompress.h
    ../Routino/src/visualiser.h
    ../Routino/src/waysx.h
)

set(ALLINP_${APPNAME}
    ${SRCS_${APPNAME}}
    ${HDRS_${APPNAME}}
)

add_executable(${APPNAME} ${ALLINP_${APPNAME}})
target_link_libraries(${APPNAME} routino)

install(
    TARGETS
      ${APPNAME}
    DESTINATION
      ${BIN_INSTALL_DIR}
)


########### router ###########

set(APPNAME router)

set(SRCS_${APPNAME}
    ../Routino/src/router.c
    ../Routino/src/output.c
)

set(HDRS_${APPNAME}
)

set(ALLINP_${APPNAME}
    ${SRCS_${APPNAME}}
    ${HDRS_${APPNAME}}
)

add_executable(${APPNAME} ${ALLINP_${APPNAME}})
target_link_libraries(${APPNAME} routino)

