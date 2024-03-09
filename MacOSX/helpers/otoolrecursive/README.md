# otoolrecursive

Command line tool which lists refernced dylibs in a given dylib recursively
Usage: otoolrecursive [-uprels]  <dynlib file>

Options are:

- v verbose output
- u only lists user dylibs
- r only lists dylibs which are referenced by @rpath
- l only lists dylibs which are referenced by @loader_path
- e only lists dylibs which are referenced by @executbale_path
- s only lists system dylibs

If neither of the options u,r,l,e,s are given, all dylbs will be listed

Currently dylibs are  listed under packages if they reside in the subtree of /opt/homebrew or /opt/local