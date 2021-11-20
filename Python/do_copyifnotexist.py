import sys
import os
import glob
import argparse
import shutil

parser = argparse.ArgumentParser()
parser.add_argument("-s", "--source", help="source dir")
parser.add_argument("-d", "--dest", help="destination dir")
args = parser.parse_args()

if args.source:
    print("using source directory {}".format(args.source))
else:
    print("source required, use -s")
    sys.exit(1)

if not os.path.isdir(args.source):
    print("{} is not a directory".format(args.source))
    sys.exit(1)

if args.dest:
    print("using destination directory {}".format(args.dest))
else:
    print("destination required, use -d")
    sys.exit(1)

if not os.path.isdir(args.dest):
    print("{} is not a directory".format(args.dest))
    sys.exit(1)

# python 3.5 or above
files = [ f for f in glob.glob(args.source + "**/*", recursive=True)]

for f in files:
    if os.path.isfile(f):
        destfn = f.replace(args.source, args.dest)
        if  os.path.exists(destfn):
            print("destination file {} exists. skip".format(destfn))
        else:
            print("{} ==> {}".format(f, destfn))
            shutil.copyfile(f, destfn)
