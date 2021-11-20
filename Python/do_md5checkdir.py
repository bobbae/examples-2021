import sys
import os
import glob
import argparse
import hashlib

parser = argparse.ArgumentParser()
parser.add_argument("-s", "--source", help="source dir")
args = parser.parse_args()

if args.source:
    print("using source directory {}".format(args.source))
else:
    print("source required, use -s")
    sys.exit(1)

if not os.path.isdir(args.source):
    print("{} is not a directory".format(args.source))
    sys.exit(1)

# python 3.5 or above
files = [ f for f in glob.glob(args.source + "**/*", recursive=True)]

hashes = {}

for f in files:
    if os.path.isfile(f):
        #print("looking at ", f)
        hashval = hashlib.md5(open(f,'rb').read()).hexdigest()
        if hashval in hashes:
            print("{} exists {} collides with {}".format(hashval, f, hashes[hashval]))
        else:
            hashes[hashval] = f

