#=============================================================================
# The translate_desktop() function was copied from the
# LXQt LXQtTranslate.cmake
#
# Original Author: Alexander Sokolov <sokoloff.a@gmail.com>
#
# funtion translate_desktop(_RESULT
#                           SOURCES <sources>
#                           [TRANSLATION_DIR] translation_directory
#                    )
#     Output:
#       _RESULT The generated .desktop (.desktop) files
#
#     Input:
#
#       SOURCES List of input desktop files (.destktop.in) to be translated
#               (merged), relative to the CMakeList.txt.
#
#       TRANSLATION_DIR Optional path to the directory with the .ts files,
#                        relative to the CMakeList.txt. Defaults to
#                        "translations".
#
#=============================================================================

function(translate_desktop _RESULT)
    # Parse arguments ***************************************
    set(oneValueArgs TRANSLATION_DIR)
    set(multiValueArgs SOURCES)

    cmake_parse_arguments(_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # check for unknown arguments
    set(_UNPARSED_ARGS ${_ARGS_UNPARSED_ARGUMENTS})
    if (NOT ${_UNPARSED_ARGS} STREQUAL "")
        MESSAGE(FATAL_ERROR
          "Unknown arguments '${_UNPARSED_ARGS}'.\n"
          "See translate_desktop() documentation for more information.\n"
        )
    endif()

    if (NOT DEFINED _ARGS_SOURCES)
        set(${_RESULT} "" PARENT_SCOPE)
        return()
    else()
        set(_sources ${_ARGS_SOURCES})
    endif()

    if (NOT DEFINED _ARGS_TRANSLATION_DIR)
        set(_translationDir "translations")
    else()
        set(_translationDir ${_ARGS_TRANSLATION_DIR})
    endif()


    get_filename_component (_translationDir ${_translationDir} ABSOLUTE)

    foreach (_inFile ${_sources})
        get_filename_component(_inFile   ${_inFile} ABSOLUTE)
        get_filename_component(_fileName ${_inFile} NAME_WE)
        #Extract the real extension ............
        get_filename_component(_fileExt  ${_inFile} EXT)
        string(REPLACE ".in" "" _fileExt ${_fileExt})
        #.......................................
        set(_outFile "${CMAKE_CURRENT_BINARY_DIR}/${_fileName}${_fileExt}")

        file(GLOB _translations
            ${_translationDir}/${_fileName}_*${_fileExt}
        )

        set(_pattern "'\\[.*]\\s*='")
        if (_translations)
            list(SORT _translations)
            add_custom_command(OUTPUT ${_outFile}
                COMMAND grep -v -a "'#TRANSLATIONS_DIR='" ${_inFile} > ${_outFile}
                COMMAND grep -h -a ${_pattern} ${_translations} >> ${_outFile}
                COMMENT "Generating ${_fileName}${_fileExt}"
            )
        else()
            add_custom_command(OUTPUT ${_outFile}
                COMMAND grep -v -a "'#TRANSLATIONS_DIR='" ${_inFile} > ${_outFile}
                COMMENT "Generating ${_fileName}${_fileExt}"
            )
        endif()

        set(__result ${__result} ${_outFile})
    endforeach()

    set(${_RESULT} ${__result} PARENT_SCOPE)
endfunction(translate_desktop)
