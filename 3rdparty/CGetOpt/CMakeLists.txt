project(CGetOpt)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

find_package(Qt5Widgets)

set(SRCS
    CGetOpt.cpp
)

set(HDRS
    CGetOpt.h
)

set(UIS

)

set(RCS


)



  qt5_wrap_cpp(MOC_SRCS ${HDRS})
  qt5_wrap_ui(UI_HDRS ${UIS})
  qt5_add_resources(RC_SRCS ${RCS})

include_directories(

)

link_directories(
    ${CMAKE_BINARY_DIR}/lib
)

add_library(CGetOpt
  STATIC
  ${SRCS}
  ${HDRS}
  ${UI_HDRS}
  ${RC_SRCS}
  ${MOC_SRCS}
  ${QM_FILES}
)


qt5_use_modules(CGetOpt Widgets)

target_link_libraries( CGetOpt
      ${QT_LIBRARIES}
)
