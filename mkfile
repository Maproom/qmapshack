#!/usr/bin/python

import sys
import os

if len(sys.argv) != 2:
    print "bad number of arguments"
    sys.exit(-1)
    

path    = sys.argv[0][:-6]
fn      = sys.argv[1]

if os.path.exists(fn):
    print "file already exists."
    sys.exit(-1)

pathTemplates   = os.path.join(path, "templates")
name, ext       = os.path.splitext(fn)

if ext == ".h":
    template = os.path.join(pathTemplates, "header.h")
elif ext == ".cpp":
    template = os.path.join(pathTemplates, "source.cpp")
elif ext == ".c":
    template = os.path.join(pathTemplates, "source.c")    
else:
    print "unknown file type"
    sys.exit(-1)
    
fid = file(template,"r")
text = fid.read()
fid.close()

text = text.replace("CLASSNAME_H", name.upper() + "_H")
text = text.replace("CLASSNAME", name)

fid = file(fn,"w")
fid.write(text)
fid.close()

