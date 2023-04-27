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
                    dest='framesJsonPath',
                    type=str,
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
                    dest='boundingBoxesPath',
                    type=str,
                    help='Path to store the bounding boxes (output)')
parser.add_argument('-t', '--trk-path',
                    action='store',
                    dest='tracksPath',
                    type=str,
                    help='path to store the tracks in a text format for \'fmdt-visu\' and \'fmdt-check\' (output)')
parser.add_argument('-j', '--trk-json-path',
                    action='store',
                    dest='tracksJsonPath',
                    type=str,
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
print("#  * log-path      = \"" + str(args.logPath) + "\"")
print("#  * trk-roi-path  = \"" + str(args.tracks2RoIsPath) + "\"")
print("#  * log-flt       = \"" + str(args.logFilter) + "\"")
print("#  * fra-path      = \"" + str(args.framesJsonPath) + "\"")
print("#  * ftr-name      = \"" + str(args.featureName) + "\"")
print("#  * ftr-path      = \"" + str(args.featurePath) + "\"")
print("#  * trk-bb-path   = \"" + str(args.boundingBoxesPath) + "\"")
print("#  * trk-path      = \"" + str(args.tracksPath) + "\"")
print("#  * trk-json-path = \"" + str(args.tracksJsonPath) + "\"")
print("#");

if args.featurePath:
    if not args.featureName:
        print("(EE) \"-x, --feature\" has to be specified")
        sys.exit(-1)
if args.featureName:
    if not args.featurePath:
        print("(EE) \"-s, --feature-path\" has to be specified")
        sys.exit(-1)
if args.featureName or args.featurePath or args.boundingBoxesPath:
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

    if args.boundingBoxesPath:
        print("# LogParser.getBoundingBoxes()...", end=" ")
        bbs = fmdt.LogParser.getBoundingBoxes(tracks, frames, tracks2RoIs)
        print("Done.")
        print("# Writing '" + args.boundingBoxesPath + "' file...", end=' ')
        fmdt.LogParser.writeBoundingBoxes(bbs, args.boundingBoxesPath)
        print("Done.")

if args.framesJsonPath:
    print("# Writing '" + args.framesJsonPath + "' file...", end=' ')
    import json
    with open(args.framesJsonPath, 'w') as fp:
        json.dump(frames, fp)
    print("Done.")

if args.tracksPath:
    print("# Writing '" + args.tracksPath + "' file...", end=' ')
    fmdt.LogParser.writeTracks(tracks, args.tracksPath)
    print("Done.")

if args.tracksJsonPath:
    print("# Writing '" + args.tracksJsonPath + "' file...", end=' ')
    import json
    with open(args.tracksJsonPath, 'w') as fp:
        json.dump(tracks, fp)
    print("Done.")

if args.featurePath:
    print("# Writing '" + args.featurePath + "' file...", end=' ')
    fmdt.LogParser.extractFeature(tracks, args.featureName, args.featurePath)
    print("Done.")
