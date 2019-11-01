/**********************************************************************************************
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

char* get_argv(const int index, char** argv)
{
    char* result = NULL;
    int len;

#ifdef WIN32
    int numargs;
    wchar_t** argw = CommandLineToArgvW(GetCommandLineW(), &numargs);

    // determine the buffer length first (including the trailing null)
    len = WideCharToMultiByte(CP_UTF8, 0, argw[index], -1, NULL, 0, NULL, NULL);
    result = (char*)calloc(len, 1);
    WideCharToMultiByte(CP_UTF8, 0, argw[index], -1, result, len, NULL, NULL);

    GlobalFree(argw);
#else
    len = strlen(argv[index]) + 1;
    result = (char*)calloc(len, 1);
    strcpy(result, argv[index]);
#endif

    return result;
}
