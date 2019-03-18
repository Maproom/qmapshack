


# Set source files
# ----------------
set(alg_SRCS
    src/alglibinternal.cpp
    src/alglibmisc.cpp
    src/ap.cpp
    src/dataanalysis.cpp
    src/diffequations.cpp
    src/fasttransforms.cpp
    src/integration.cpp
    src/interpolation.cpp
    src/linalg.cpp
    src/optimization.cpp
    src/solvers.cpp
    src/specialfunctions.cpp
    src/statistics.cpp
)

# Set header files
# ----------------
set(alg_HDRS
    src/alglibinternal.h
    src/alglibmisc.h
    src/ap.h
    src/dataanalysis.h
    src/diffequations.h
    src/fasttransforms.h
    src/integration.h
    src/interpolation.h
    src/linalg.h
    src/optimization.h
    src/solvers.h
    src/specialfunctions.h
    src/statistics.h
    src/stdafx.h
)



# The target to build
# -------------------
add_library(alg STATIC ${alg_SRCS} ${alg_HDRS})

