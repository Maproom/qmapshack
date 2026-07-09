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

char* get_argv(const int index, char** argv) {
  // argv arrives as UTF-8 on every platform (the executable runs with a UTF-8 active
  // code page on Windows), so a plain copy is enough.
  const int len = strlen(argv[index]) + 1;
  char* result = (char*)calloc(len, 1);
  strcpy(result, argv[index]);
  return result;
}
