
option(ROUTINO_BUILD_SLIM_VERSION "Routino: Build slim version" OFF)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/include
)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -Dinline=__inline)
endif(MSVC)

if(UNIX)
add_definitions(-Wall -std=c99 -ffast-math -D_FILE_OFFSET_BITS=64 -D_POSIX_C_SOURCE=200809L -pedantic)
endif(UNIX)

add_definitions(-DROUTINO_DATADIR="./")

if(ROUTINO_BUILD_SLIM_VERSION)
    add_definitions(-DSLIM=1)
else(ROUTINO_BUILD_SLIM_VERSION)
    add_definitions(-DSLIM=0)
endif(ROUTINO_BUILD_SLIM_VERSION)

########### routino library ###########
set(APPNAME routino)

set(SRCS_${APPNAME}
    #errorlog.c
    #errorlogx.c
    ../Routino/src/fakes.c
    ../Routino/src/files.c
    #logerror.c
    ../Routino/src/logging.c
    ../Routino/src/nodes.c
    #nodesx.c
    ../Routino/src/optimiser.c
    #osmo5mparse.c
    #osmparser.c
    #osmpbfparse.c
    #osmxmlparse.c
    ../Routino/src/output.c
    ../Routino/src/profiles.c
    #prunex.c
    ../Routino/src/queue.c
    ../Routino/src/relations.c
    #relationsx.c
    ../Routino/src/results.c
    ../Routino/src/segments.c
    #segmentsx.c
    #sorting.c
    #superx.c
    #tagging.c
    ../Routino/src/translations.c
    ../Routino/src/types.c
    #uncompress.c
    #visualiser.c
    ../Routino/src/ways.c
    #waysx.c
    ../Routino/src/xmlparse.c
)

if(MSVC)
set(SRCS_${APPNAME}
    ${SRCS_${APPNAME}}
    ../Routino/src/mman-win32.c
)
endif(MSVC)

set(HDRS_${APPNAME}
    ../Routino/src/cache.h
    #errorlog.h
    #errorlogx.h
    ../Routino/src/fakes.h
    ../Routino/src/files.h
    #functions.h
    ../Routino/src/logging.h
    ../Routino/src/nodes.h
    #nodesx.h
    #osmparser.h
    ../Routino/src/profiles.h
    #prunex.h
    ../Routino/src/relations.h
    #relationsx.h
    ../Routino/src/results.h
    ../Routino/src/segments.h
    #segmentsx.h
    #sorting.h
    #superx.h
    #tagging.h
    ../Routino/src/translations.h
    ../Routino/src/types.h
    #typesx.h
    #uncompress.h
    #visualiser.h
    ../Routino/src/ways.h
    #waysx.h
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
)

set(HDRS_${APPNAME}
)

set(ALLINP_${APPNAME}
    ${SRCS_${APPNAME}}
    ${HDRS_${APPNAME}}
)

#add_executable(${APPNAME} ${ALLINP_${APPNAME}})
#target_link_libraries(${APPNAME} routino)

########### router ###########

set(APPNAME router)

set(SRCS_${APPNAME}
    ../Routino/src/router.c
)

set(HDRS_${APPNAME}
)

set(ALLINP_${APPNAME}
    ${SRCS_${APPNAME}}
    ${HDRS_${APPNAME}}
)

add_executable(${APPNAME} ${ALLINP_${APPNAME}})
target_link_libraries(${APPNAME} routino)

