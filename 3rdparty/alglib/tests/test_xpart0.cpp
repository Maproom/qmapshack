#include "linalg.h"

int main()
{
    alglib::real_2d_array a("[[1]]");
    alglib::spdmatrixcholesky(a, 1, true);
    return 0;
}