#!/usr/bin/env python3

import fmdt.log
import json
import argparse
import sys

parser = argparse.ArgumentParser(prog='compare.py',
                                 formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-l', '--log-path',
                    action='store',
                    dest='logPath',
                    type=str,
                    required=True,
                    help='path to the fmdt-detect log files (can be a directory or a file)')
parser.add_argument('-r', '--trk-roi-path',
                    action='store',
                    dest='tracks2RoIsPath',
                    type=str,
                    default="",
                    help='path to tracks to RoIs file')
parser.add_argument('-f', '--filter',
                    action='store',
                    dest='filter',
                    type=str,
                    default="^[0-9]{5}\.txt",
                    help='regular expression filter for log filenames')
parser.add_argument('-o', '--out-fra',
                    action='store',
                    dest='outFramesPath',
                    type=str,
                    default="frames.json",
                    help='path to store the frames in a JSON format')
parser.add_argument('-t', '--out-trk',
                    action='store',
                    dest='outTracksPath',
                    type=str,
                    default="tracks.json",
                    help='path to store the last tracks in a JSON format')
parser.add_argument('-x', '--feature',
                    action='store',
                    dest='featureName',
                    type=str,
                    default="",
                    help='name of a RoI feature to extract')
parser.add_argument('-s', '--feature-path',
                    action='store',
                    dest='featurePath',
                    type=str,
                    default="",
                    help='Path to store the RoI feature extraction')
parser.add_argument('-b', '--bb-path',
                    action='store',
                    dest='BoundingBoxesPath',
                    type=str,
                    default="",
                    help='Path to store the bounding boxes')

args = parser.parse_args()

print("---------------------------")
print("----- FMDT Log Parser -----")
print("---------------------------")
print("");
print("Arguments:")
print("  -l, --log-path     = \"" + args.logPath + "\"")
print("  -r, --trk-roi-path = \"" + args.tracks2RoIsPath + "\"")
print("  -f, --filter       = \"" + args.filter + "\"")
print("  -o, --out-fra      = \"" + args.outFramesPath + "\"")
print("  -t, --out-trk      = \"" + args.outTracksPath + "\"")
print("  -x, --feature      = \"" + args.featureName + "\"")
print("  -s, --feature-path = \"" + args.featurePath + "\"")
print("  -b, --bb-path      = \"" + args.BoundingBoxesPath + "\"")
print("");

if args.featurePath:
    if not args.featureName:
        print("(EE) \"-x, --feature\" has to be specified")
        sys.exit(-1)
if args.featureName:
    if not args.featurePath:
        print("(EE) \"-s, --feature-path\" has to be specified")
        sys.exit(-1)
if args.featureName or args.featurePath or args.BoundingBoxesPath:
    if not args.tracks2RoIsPath:
        print("(EE) \"-r, --trk-roi-path\" has to be specified")
        sys.exit(-1)

frames = fmdt.LogParser.parseFiles(args.logPath, args.filter)

print("LogParser.getLastTracks()...", end=" ")
tracks = fmdt.LogParser.getLastTracks(frames)
print("Done.")

if args.tracks2RoIsPath != "":
    print("LogParser.parseTracks2RoIsFile()...", end=" ")
    tracks2RoIs = fmdt.LogParser.parseTracks2RoIsFile(args.tracks2RoIsPath)
    print("Done.")

    print("LogParser.insertRoIsInTracks()...", end=" ")
    tracks = fmdt.LogParser.insertRoIsInTracks(tracks, frames, tracks2RoIs)
    print("Done.")

    if args.BoundingBoxesPath != "":
        print("LogParser.getBoundingBoxes()...", end=" ")
        bbs = fmdt.LogParser.getBoundingBoxes(tracks, frames, tracks2RoIs)
        print("Done.")
        print("Writing '" + args.BoundingBoxesPath + "' file...", end=' ')
        fmdt.LogParser.writeBoundingBoxes(bbs, args.BoundingBoxesPath)
        print("Done.")

print("Writing '" + args.outFramesPath + "' file...", end=' ')
import json
with open(args.outFramesPath, 'w') as fp:
    json.dump(frames, fp)
print("Done.")

if args.outTracksPath:
    print("Writing '" + args.outTracksPath + "' file...", end=' ')
    import json
    with open(args.outTracksPath, 'w') as fp:
        json.dump(tracks, fp)
    print("Done.")

print("Writing '" + args.featurePath + "' file...", end=' ')
fmdt.LogParser.extractFeature(tracks, args.featureName, args.featurePath)
print("Done.")
