#!/usr/bin/env python3

import fmdt.log
import json
import argparse
import sys

parser = argparse.ArgumentParser(prog='fmdt-log-parser',
                                 formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-l', '--log-path',
                    action='store',
                    dest='logPath',
                    type=str,
                    required=True,
                    help='path to the fmdt-detect log files (can be a directory or a file) (input)')
parser.add_argument('-r', '--trk-roi-path',
                    action='store',
                    dest='tracks2RoIsPath',
                    type=str,
                    help='path to tracks to RoIs file (input)')
parser.add_argument('-f', '--log-flt',
                    action='store',
                    dest='logFilter',
                    type=str,
                    default="^[0-9]{5}\.txt",
                    help='regular expression filter for log filenames')
parser.add_argument('-o', '--fra-path',
                    action='store',
                    dest='outFramesPath',
                    type=str,
                    default="frames.json",
                    help='path to store the frames in a JSON format (output)')
parser.add_argument('-x', '--ftr-name',
                    action='store',
                    dest='featureName',
                    type=str,
                    help='name of a RoI feature to extract')
parser.add_argument('-s', '--ftr-path',
                    action='store',
                    dest='featurePath',
                    type=str,
                    help='Path to store the RoI feature extraction (output)')
parser.add_argument('-b', '--trk-bb-path',
                    action='store',
                    dest='BoundingBoxesPath',
                    type=str,
                    help='Path to store the bounding boxes (output)')
parser.add_argument('-t', '--trk-path',
                    action='store',
                    dest='outTracksPath',
                    type=str,
                    default="tracks.json",
                    help='path to store the last tracks in a JSON format (output)')

args = parser.parse_args()

print("#  -------------------------");
print("# |              ----*      |");
print("# | --* FMDT-LOG-PARSER --* |");
print("# |       -------*          |");
print("#  -------------------------");

print("#");
print("# Parameters:")
print("# -----------")
print("#  * log-path     = \"" + args.logPath + "\"")
print("#  * trk-roi-path = \"" + args.tracks2RoIsPath + "\"")
print("#  * log-flt      = \"" + args.logFilter + "\"")
print("#  * fra-path     = \"" + args.outFramesPath + "\"")
print("#  * ftr-name     = \"" + args.featureName + "\"")
print("#  * ftr-path     = \"" + args.featurePath + "\"")
print("#  * trk-bb-path  = \"" + args.BoundingBoxesPath + "\"")
print("#  * trk-path     = \"" + args.outTracksPath + "\"")
print("#");

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

frames = fmdt.LogParser.parseFiles(args.logPath, args.logFilter, verbose=True)

print("# LogParser.getLastTracks()...", end=" ")
tracks = fmdt.LogParser.getLastTracks(frames)
print("Done.")

if args.tracks2RoIsPath:
    print("# LogParser.parseTracks2RoIsFile()...", end=" ")
    tracks2RoIs = fmdt.LogParser.parseTracks2RoIsFile(args.tracks2RoIsPath)
    print("Done.")

    print("# LogParser.insertRoIsInTracks()...", end=" ")
    tracks = fmdt.LogParser.insertRoIsInTracks(tracks, frames, tracks2RoIs)
    print("Done.")

    if args.BoundingBoxesPath:
        print("# LogParser.getBoundingBoxes()...", end=" ")
        bbs = fmdt.LogParser.getBoundingBoxes(tracks, frames, tracks2RoIs)
        print("Done.")
        print("# Writing '" + args.BoundingBoxesPath + "' file...", end=' ')
        fmdt.LogParser.writeBoundingBoxes(bbs, args.BoundingBoxesPath)
        print("Done.")

print("# Writing '" + args.outFramesPath + "' file...", end=' ')
import json
with open(args.outFramesPath, 'w') as fp:
    json.dump(frames, fp)
print("Done.")

if args.outTracksPath:
    print("# Writing '" + args.outTracksPath + "' file...", end=' ')
    import json
    with open(args.outTracksPath, 'w') as fp:
        json.dump(tracks, fp)
    print("Done.")

print("# Writing '" + args.featurePath + "' file...", end=' ')
fmdt.LogParser.extractFeature(tracks, args.featureName, args.featurePath)
print("Done.")
