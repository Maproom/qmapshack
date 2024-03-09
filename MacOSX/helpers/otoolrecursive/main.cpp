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
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <set>
#include <vector>

#include "loaderpathresolver.h"

using namespace std;

const string OTOOL{"otool -L "};

const string SYSTEM_LIBS{"/System/Library/"};
const string LIBS{"/usr/lib"};
set<string> systemLibs;
set<string> userLibs;

const string RPATH{"@rpath"};
set<string> rpathLibs;
const string LOADER_PATH{"@loader_path"};
vector<pair<string, string>> loaderLibs; // calling dylib, referenced dylib
const string EXECUTABLE_PATH{"@executable_path"};
set<string> executableLibs;

string libFileName{}; // dynlib to parse
string pathToLibFileName{};
vector<string> options{}; // command line options
    // (-v verbose, -u user libs -r rpath libs, -e executable libs, -l loader_path libs, -s system libs)

string absPath(const string &filename)
{
    int index = filename.find_last_of("/\\");
    return filename.substr(0, index);
}

string filenameNoPath(const string &filename)
{
    int index = filename.find_last_of("/\\");
    return filename.substr(index + 1);
}

string trim(const string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

///
/// \brief firstWord - Assumption str starts with non whitespace character
/// \param str String where to extract first word
///
void firstWord(string &str)
{
    for (int i = 0; i < str.size(); i++) {
        if (isspace(str[i])) {
            str.erase(i, str.size());
            return;
        }
    };
}

///
/// \brief startsWith
/// \param str - Complete string
/// \param pre - partial string to compare with str from the start
/// \return true, if str starts with pre, otherwise false
///
bool startsWith(const string &str, const string &pre)
{
    return str.compare(0, pre.size(), pre) == 0 ? true : false;
}

void printLibs()
{
    cout << "----------------------------------------------------------------" << endl;
    cout << "*** @rpath ***" << endl;
    for (const string &dylib : rpathLibs) {
        cout << dylib << endl;
    }

    cout << "----------------------------------------------------------------" << endl;
    cout << "*** @loader_path ***" << endl;
    for (const pair<string, string> &dylib : loaderLibs) {
        cout << dylib.first << " -- " << dylib.second << endl;
    }

    cout << "----------------------------------------------------------------" << endl;
    cout << "*** @executabe_path ***" << endl;
    for (const string &dylib : executableLibs) {
        cout << dylib << endl;
    }

    cout << "----------------------------------------------------------------" << endl;
    cout << "*** system libs ***" << endl;
    for (const string &dylib : systemLibs) {
        cout << dylib << endl;
    }
    cout << "----------------------------------------------------------------" << endl;
}

vector<string> toBeProcessedLibs;
set<string> processedLibs;

void libsReferred(const string &dylib)
{
    string cmd{OTOOL};
    cmd.append(dylib);
    cmd.append(" | awk '{ print $1 }'");
    // cout << "Calling " << cmd << endl;

    FILE *fp;
    char path[PATH_MAX];

    fp = popen(cmd.c_str(), "r");
    if (fp == nullptr) {
        std::cerr << "Cannot open pipe to otool\n";
        return;
    }

    // ignore 1st line, since it is the calling dylib
    if (fgets(path, PATH_MAX, fp) == NULL) {
        return;
    }
    while (fgets(path, PATH_MAX, fp) != NULL) {
        char *pos = strchr(path, '\n');
        if (pos == nullptr)
            continue;
        *pos = '\0';

        // duplicate?
        if (dylib.compare(path) == 0)
            continue;

        // system libs
        if (startsWith(path, SYSTEM_LIBS) || startsWith(path, LIBS)) {
            systemLibs.insert(path);
            continue;
        }

        // @rpath
        if (startsWith(path, RPATH)) {
            // Collect for processing later on
            rpathLibs.insert(path);
            continue;
        }

        // @loader_path
        if (startsWith(path, LOADER_PATH)) {
            // Collect for processing later on
            loaderLibs.push_back(make_pair(dylib, path));
            //            toBeProcessedLibs.push_back(LoaderPathResolver::loader_path(dylib, path));
            continue;
        }

        // @executable_path
        // Collect for processing later on
        if (startsWith(path, EXECUTABLE_PATH)) {
            executableLibs.insert(path);
            continue;
        }

        toBeProcessedLibs.push_back(path);
    }

    pclose(fp);

    while (!toBeProcessedLibs.empty()) {
        string lib = toBeProcessedLibs.back();
        toBeProcessedLibs.pop_back();
        auto result = processedLibs.insert(lib);
        if (result.second == true) {
            cout << lib << endl;
            libsReferred(lib);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        std::cerr << "No dylib filename given!" << endl;
        std::cerr << "usage: otoolrecursive [-vurels]  <dylib file>\n";
        return EXIT_FAILURE;
    }

    // Parsing command line arguments
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-' && strlen(argv[i]) == 2) {
            options.push_back(argv[i] + 1);

        } else if (filesystem::exists(argv[i])) {
            libFileName = filesystem::absolute(argv[i]);
            pathToLibFileName = absPath(libFileName);
        } else {
            std::cerr << "usage: otoolrecursive [-vprelo]  <dynlib file>\n";
            return EXIT_FAILURE;
        }
    }

    libsReferred(libFileName);
    //    containsLibs(libFileName);

    if (std::count(options.begin(), options.end(), "v")) {
        printLibs();
    }
    return EXIT_SUCCESS;
}
