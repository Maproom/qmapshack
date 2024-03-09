/****************************************************************************
**
** Copyright (C) 2020 Manfred Kern. All rights reserved.
** Contact: manfred.kern@gmail.com
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. All advertising materials mentioning features or use of this software
**    must display the following acknowledgement:
**    This product includes software developed by the the organization.
**
** 4. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/
#include "loaderpathresolver.h"
#include <filesystem>
#include <iostream>

using namespace std;

const string LoaderPathResolver::LOADER_PATH{"@loader_path"};

LoaderPathResolver::LoaderPathResolver()
{
}

/**
 * @brief Replaces the relative path part (@loader_path/.. ...) in loader_path_dylib by loader prefix path from otool_dylib
 * @param otool_dylib - dylib from which loader path is taken
 * @param loader_path_dylib - dylib for which the relative @loader_path/.. is replaced by the absolute loader path
 * @return modified loader_path_dylib with @loader_path/.. replaced by absolute path
 */
const std::string LoaderPathResolver::loader_path(const std::string &otool_dylib,
                                                  const std::string &loader_path_dylib)
{
    // check if referenced dylib starts with @loader_path
    string_view loader_path_dylib_view{loader_path_dylib};
    if (!loader_path_dylib_view.starts_with(LOADER_PATH)) {
        loader_path_dylib_view.remove_prefix(LOADER_PATH.size());

        return loader_path_dylib;
    }

    const string HOMEBREW_PREFIX{"/opt/homebrew/"};
    const string MACPORTS_PREFIX{"/opt/local/"};

    // remove from otool_dylib the filename and one parent) --> last part
    if (!otool_dylib.starts_with(HOMEBREW_PREFIX) && !otool_dylib.starts_with(MACPORTS_PREFIX)) {
        return loader_path_dylib;
    }

    auto firstPart = otool_dylib.starts_with(HOMEBREW_PREFIX) ? HOMEBREW_PREFIX : MACPORTS_PREFIX;

    // remove from loader_path_dylib everything from the start until the first char --> second part
    auto pos_of_lastpart = loader_path_dylib.rfind("../") + 3;
    auto lastPart = loader_path_dylib.substr(pos_of_lastpart,
                                             loader_path_dylib.size() - pos_of_lastpart);
    string parent = firstPart + lastPart;
    return parent;
}
