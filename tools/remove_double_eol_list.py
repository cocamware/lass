import os
import string
import sys
import glob


if len(sys.argv)!=2:
    print "Usage : %s filefilter\n\tReplaces double EOL by a single EOL\n" & (sys.argv[0])

infiles = glob.glob(sys.argv[1])

for infile in infiles:
    print infile
    if True:
        intext = string.join(open(infile,"r",).readlines(),"")
        outtext = intext.replace("\n\n","\n")
        outfile = infile
        open(outfile,"w",).write(outtext)
