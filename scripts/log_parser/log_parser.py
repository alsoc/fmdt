#!/usr/bin/env python3

import re
import os
import sys
import argparse
import re
import shutil

# liste des tables a ne pas traiter dans la comparaison
ExceptionTables = ["no conflict found", "association conflicts"]
ValidTables = {
    "RoIs": {
        "name": "RoIs",
        "regex": ["^# Frame n°[0-9]{5}.*\(t\).*--.*Regions of interest.*[0-9]", "^# Frame n°.*(t)"],
        "search": { "what": "frame_id", "how": "[0-9]{5}" },
        "id": "rid",
        "cols_rename": {
            "roi_id": "rid",
            "track_id": "tid",
            "track_type": "otype",
            "bounding box_xmin": "xmin",
            "bounding box_xmax": "xmax",
            "bounding box_ymin": "ymin",
            "bounding box_ymax": "ymax",
            "surface (s in pixels)_s": "S",
            "surface (s in pixels)_sx": "Sx",
            "surface (s in pixels)_sy": "Sy",
            "surface (s in pixels)_sx2": "Sx2",
            "surface (s in pixels)_sy2": "Sy2",
            "surface (s in pixels)_sxy": "Sxy",
            "center_x": "x",
            "center_y": "y",
            "magnitude_--": "mag",
            "saturation_counter": "sat",
            "ellipse_a" : "a",
            "ellipse_b" : "b",
            "ellipse_ratio" : "r",
        },
    },
    "Assos": {
        "name": "Assos",
        "regex": ["^# Associations.*[0-9]*:$"],
        # "id": "rid_t",
        "cols_rename": {
            "roi id_t-1": "rid_t-1",
            "roi id_t": "rid_t",
            "distance_pixels": "dist",
            "distance_rank": "k",
            "error (or velocity)_dx": "dx",
            "error (or velocity)_dy": "dy",
            "error (or velocity)_e": "e",
            "motion_is moving": "mov",
        },
    },
    "Motion": {
        "name": "Motion",
        "regex": ["^# Motion:"],
        "single_entry": True,
        "cols_rename": {
            "first motion estimation (with all associated rois)_theta": "theta1",
            "first motion estimation (with all associated rois)_tx": "tx1",
            "first motion estimation (with all associated rois)_ty": "ty1",
            "first motion estimation (with all associated rois)_mean err": "mean_er1",
            "first motion estimation (with all associated rois)_std dev": "std_dev1",
            "second motion estimation (exclude moving rois)_theta": "theta2",
            "second motion estimation (exclude moving rois)_tx": "tx2",
            "second motion estimation (exclude moving rois)_ty": "ty2",
            "second motion estimation (exclude moving rois)_mean err": "mean_er2",
            "second motion estimation (exclude moving rois)_std dev": "std_dev2",
        },
    },
    "Tracks": {
        "name": "Tracks",
        "regex": ["^# Tracks.*[0-9]"],
        "id": "tid",
        "cols_rename": {
            "track_id": "tid",
            "begin_frame #": "fbeg",
            "begin_x": "xbeg",
            "begin_y": "ybeg",
            "end_frame #": "fend",
            "end_x": "xend",
            "end_y": "yend",
            "object_type": "otype",
            "reason of changed_object only)": "reason",
        },
    },
}

def parseTab(lines, start, tableInfo):
    colList = []
    subtitleIndexLen = 0
    lenght = 2 # offset

    # set up titles and subtitles
    titleList = (lines[start+2][2:-1] ).split("||")
    titleList = [x.lower() for x in titleList]
    titleLenList = [len(txt) for txt in titleList]
    subtitleList = (lines[start+5][2:-1]).replace("||","|").split("|")
    subtitleList = [x.lower() for x in subtitleList]
    subtitleLenList = [len(txt) for txt in subtitleList]
    titleListLen = len(titleList)

    for i in range(titleListLen):
        titleListLenIndex = titleLenList[i]

        cpt = 0
        while cpt != titleListLenIndex:
            # UGLY
            if cpt == 0:
                cpt = subtitleLenList[subtitleIndexLen]
            else:
                cpt += subtitleLenList[subtitleIndexLen] + 1

            colName = titleList[i].strip() + "_" + subtitleList[subtitleIndexLen].strip()
            if "cols_rename" in tableInfo and colName in tableInfo["cols_rename"]:
                colName = tableInfo["cols_rename"][colName]
            colList += [(colName,lenght,lenght + subtitleLenList[subtitleIndexLen], [])]
            lenght += subtitleLenList[subtitleIndexLen] + 1
            subtitleIndexLen += 1
        lenght += 1

    # this print is useful to fill the 'cols_rename' field in the ValidTables global variable
    # print(colList)

    if "id" in tableInfo:
        entries = {}
    else:
        entries = []

    # add data
    cur = start + 7
    while cur != len(lines) and lines[cur][0] != '#':
        roi_id = 0;
        entry = {}
        for (column, begin, end, data) in colList:
            if "id" in tableInfo and column == tableInfo["id"]:
                tableId = int(lines[cur][begin:end])

            valueStr = lines[cur][begin:end].strip()

            regex = re.compile("^[-+]?[0-9]+\.[0-9]+$", re.IGNORECASE)
            value = "empty"
            if re.match(regex, valueStr):
                value = float(valueStr)
            else:
                regex2 = re.compile(r'^[-+]?([1-9]\d*|0)$', re.IGNORECASE)
                if re.match(regex2, valueStr):
                    value = int(valueStr)
                else:
                    regex3 = re.compile("^-*$", re.IGNORECASE)
                    if re.match(regex3, valueStr):
                        value = None
                    else:
                        regex4 = re.compile("^yes$|^no$", re.IGNORECASE)
                        if re.match(regex4, valueStr):
                            if valueStr == "yes":
                                value = True
                            else:
                                value = False
                        else:
                            value = valueStr

            entry[column] = value

        if "id" in tableInfo:
            entry[tableInfo["id"]] = tableId
            entries[tableId] = entry
        else:
            entries.append(entry)

        cur += 1

    return (entries, cur)

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
print("----- FMDT log parser -----")
print("---------------------------")
print("");
print("Arguments:")
print("  -l, --log-path = '" + args.logPath + "'")
print("  -f, --filter   = '" + args.filter + "'")
print("  -o, --out      = '" + args.outJsonFile + "'")
print("");

if not os.path.exists(args.logPath):
    print("(EE) The given path does not exist")
    sys.exit(1)

onlyFiles = []
if os.path.isdir(args.logPath):
    onlyFiles = [f for f in os.listdir(args.logPath) if os.path.isfile(os.path.join(args.logPath, f))]
else:
    onlyFiles.append(os.path.basename(args.logPath))
    args.logPath = args.logPath.replace(onlyFiles[0], "")
onlyFiles = sorted(onlyFiles)

userRegex = re.compile(args.filter)
filteredFiles = []
for file in onlyFiles:
    if re.search(userRegex, file):
        filteredFiles.append(file)

frames = dict()
for file in filteredFiles:
    cur_frame = dict()
    frame_id = 0
    frame_id = int(file.split(".")[0])
    frames[frame_id] = {"frame_id": frame_id}

    f = open(args.logPath + "/" + file, "r")
    lines = f.readlines()
    f.close()

    print("Processing '" + args.logPath + "/" + file + "' file...", end=' ')

    l = 0
    while l < len(lines):
        name = lines[l].lower()
        skip_table = 0
        # on vérifie que le nom de la table à traiter n'est pas une exception
        for excep in ExceptionTables:
            if excep in name:
                skip_table = 1
                continue

        tableName = "Not defined"
        if not skip_table:
            skip_table = 1
            for vTable in ValidTables:
                nMatch = 0
                for sregex in ValidTables[vTable]["regex"]:
                    regexc = re.compile(sregex, re.IGNORECASE)
                    if re.match(regexc, name):
                        nMatch += 1
                if nMatch == len(ValidTables[vTable]["regex"]):
                    skip_table = 0
                    tableName = ValidTables[vTable]["name"]
                    continue

        # si c'est une exception alors on avance jusqu'a la nouvelle table
        if skip_table:
            l += 1
            while l < len(lines) and lines[l][0] != '#': l += 1
            # sauter la ligne ou il y a un seul caractere #
            if l + 1 < len(lines): l += 1

        # sinon on traite la table normalement
        else:
            # retreive the real frame_id
            if tableName == "RoIs":
                regexc = re.compile(ValidTables[tableName]["search"]["how"], re.IGNORECASE)
                frame_id = int(re.search(regexc, name).group(0))
                print("(frame_id = " + str(frame_id) + ")", end=' ')

            (entries, cur) = parseTab(lines, l, ValidTables[tableName])
            l = cur + 1
            if "single_entry" in ValidTables[tableName] and ValidTables[tableName]["single_entry"]:
                frames[frame_id][tableName] = entries[0]
            else:
                frames[frame_id][tableName] = entries
    print("Done.", end="\r")
print("", end="\n")

print("Writing '" + args.outJsonFile + "' file...", end=' ')
import json
with open(args.outJsonFile, 'w') as fp:
    json.dump(frames, fp)
print("Done.")

# print(frames)
