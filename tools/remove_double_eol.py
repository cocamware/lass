import os
import string
import sys


if len(sys.argv)!=3:
    print "Usage : %s infile outfile\n\tReplaces double EOL by a single EOL\n" & (sys.argv[0])

infile = sys.argv[1]
outfile = sys.argv[2]

intext = string.join(open(infile,"r",).readlines(),"")
outtext = intext.replace("\n\n","\n")
open(outfile,"w",).write(outtext)
