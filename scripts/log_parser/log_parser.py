#!/usr/bin/env python3

import fmdt.log
import json
import argparse

parser = argparse.ArgumentParser(prog='compare.py',
                                 formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-l', '--log-path',
                    action='store',
                    dest='logPath',
                    type=str,
                    required=True,
                    help='path to the fmdt-detect log files (can be a directory or a file)')
parser.add_argument('-f', '--filter',
                    action='store',
                    dest='filter',
                    type=str,
                    default="^[0-9]{5}\.txt",
                    help='regular expression filter for log filenames')
parser.add_argument('-o', '--out',
                    action='store',
                    dest='outJsonFile',
                    type=str,
                    default="frames.json",
                    help='path to store the frames in a JSON format')

args = parser.parse_args()

print("---------------------------")
print("----- FMDT Log Parser -----")
print("---------------------------")
print("");
print("Arguments:")
print("  -l, --log-path = '" + args.logPath + "'")
print("  -f, --filter   = '" + args.filter + "'")
print("  -o, --out      = '" + args.outJsonFile + "'")
print("");

frames = fmdt.LogParser.parseFiles(args.logPath, args.filter)

print("Writing '" + args.outJsonFile + "' file...", end=' ')
import json
with open(args.outJsonFile, 'w') as fp:
    json.dump(frames, fp)
print("Done.")
