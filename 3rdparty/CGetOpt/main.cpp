#include "getopt.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    // construct class from command line arguments
    GetOpt opts(argc, argv);

    // add some switches
    bool verbose;
    opts.addSwitch("verbose", &verbose);
    QString arg;
    opts.addOptionalArgument("file", &arg);
    QStringList defines;
    opts.addRepeatableOption('D', &defines);

    // do the parsing and check for errors
    if (!opts.parse()) {
        cerr << "Usage: " << (const char *)opts.appName().toAscii()
             << " [--verbose]"
             << " [-Ddefine]*"
             << " [file]" << endl;
        return 1;
    }

    cout << "\nVerbose mode: " << (verbose ? "yes" : "no") << endl;
    cout << "Optional file argument: " <<
        (arg.isNull() ? "(none)" : (const char *)arg.toAscii()) << endl;
    cout << "Defines: " << (defines.isEmpty() ? "(none)" :
                            (const char *)defines.join(", ").toAscii()) << endl;
    cout << endl;

    return 0;
}
