#!/usr/bin/env python3

import re
import os
import sys
import argparse
import re
import shutil

class FMDTLogParser:
    # list of tables to parse in the FMDT log files
    Tables = {
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
        "Assocs": {
            "name": "Assocs",
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

    @staticmethod
    def parseTab(lines:list[str], startLineId:int, tableInfo:dict):
        """Parse information from an FMDT table.

        Parameters
        ----------
        `lines` (list[str]): A list of strings. Each string corresponds to a line in
            the FMDT log file.
        `startLineId` (int): The start id in the list of `lines`.
        `tableInfo` (dict): A dictionary that describes how to parse the current
            table. The dictionary has the following form
            {
                "id": StrOfTheColumnCorrespondingToTheId,
                "cols_rename": {
                    "OldColumnName1": "NewColumnName1",
                    "OldColumnName2": "NewColumnName2",
                    "OldColumnName3": "NewColumnName3",
                    "OldAndSoOn": "NewAndSoOn",
            }
            Note that in the previous dict, the "id" and "cols_rename" are not
            mandatory.

        Returns
        -------
        (`entries`, `curLineId`) (dict, int): `entries` is a dictionary of the
            parsed item in the table. Each entry key corresponds to the `id`
            specified in the `tableInfo` input parameter. If the `id` field is not
            specified in `tableInfo` then the returned `entries` are not a
            dictionary but a list. `curLineId` is the current id in the list of
            `lines` (after parsing the table).
        """

        colList = []
        subtitleIndexLen = 0
        lenght = 2 # offset

        # set up titles and subtitles
        titleList = (lines[startLineId+2][2:-1] ).split("||")
        titleList = [x.lower() for x in titleList]
        titleLenList = [len(txt) for txt in titleList]
        subtitleList = (lines[startLineId+5][2:-1]).replace("||","|").split("|")
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

        # this print is useful to fill the 'cols_rename' field in the FMDTLogParser.Tables global variable
        # print(colList)

        if "id" in tableInfo:
            entries = {}
        else:
            entries = []

        # add data
        curLineId = startLineId + 7
        while curLineId != len(lines) and lines[curLineId][0] != '#':
            roi_id = 0;
            entry = {}
            for (column, begin, end, data) in colList:
                if "id" in tableInfo and column == tableInfo["id"]:
                    tableId = int(lines[curLineId][begin:end])

                valueStr = lines[curLineId][begin:end].strip()

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

            curLineId += 1

        return (entries, curLineId)

    @staticmethod
    def parseFile(filePath:str, fileBasename:str):
        """Parse information from an entire FMDT log file.

        Parameters
        ----------
        `filePath` (str): A path to a file (without the file name). For instance,
            '/home/toto/dir/' is valid and '/home/toto/dir/00012.txt' is NOT.
        `fileBasename` (str): File name without the full path. For instance,
            '00012.txt' is valid and '/home/toto/dir/00012.txt' is NOT.

        Returns
        -------
        (`curFrame`, `frameId`) (dict, int): `curFrame` is a dictionary of the
            parsed table in the file. `frameId` is the identifier of the frame
            corresponding of the file parsed. `curFrame` has the following form:
            {
                "RoIs": {},
                "Assocs": {},
                "Motion": {}
                "Tracks": {},},
            }
        """

        curFrame = dict()
        frameId = 0
        frameId = int(fileBasename.split(".")[0])

        f = open(filePath + "/" + fileBasename, "r")
        lines = f.readlines()
        f.close()

        l = 0
        while l < len(lines):
            name = lines[l].lower()
            tableName = "Not defined"
            skip_table = 1
            for vTable in FMDTLogParser.Tables:
                nMatch = 0
                for sregex in FMDTLogParser.Tables[vTable]["regex"]:
                    regexc = re.compile(sregex, re.IGNORECASE)
                    if re.match(regexc, name):
                        nMatch += 1
                if nMatch == len(FMDTLogParser.Tables[vTable]["regex"]):
                    skip_table = 0
                    tableName = FMDTLogParser.Tables[vTable]["name"]
                    continue

            # si c'est une exception alors on avance jusqu'a la nouvelle table
            if skip_table:
                l += 1
                while l < len(lines) and lines[l][0] != '#': l += 1
                # sauter la ligne ou il y a un seul caractere #
                if l + 1 < len(lines): l += 1
            # sinon on traite la table normalement
            else:
                # retrieve the real frameId
                if tableName == "RoIs":
                    regexc = re.compile(FMDTLogParser.Tables[tableName]["search"]["how"], re.IGNORECASE)
                    frameId = int(re.search(regexc, name).group(0))

                (entries, cur) = FMDTLogParser.parseTab(lines, l, FMDTLogParser.Tables[tableName])
                l = cur + 1
                if "single_entry" in FMDTLogParser.Tables[tableName] and FMDTLogParser.Tables[tableName]["single_entry"]:
                    curFrame[tableName] = entries[0]
                else:
                    curFrame[tableName] = entries
        return (curFrame, frameId)

    @staticmethod
    def parseFiles(path:str, fileNameFilter:str):
        """Parse information from one or multiple FMDT log files.

        Parameters
        ----------
        `path` (str): A path to a file or the a directory of FMDT log files.
        `fileNameFilter` (str): Regex for the file name. If the regex is not matched
            then the file is not parsed.

        Returns
        -------
        `frames` (dict): dictionary of the parsed files. `frames` is indexed by the
            frame id:
            {
                firstId: {
                    "frame_id": firstId,
                    "RoIs": {},
                    "Assocs": {},
                    "Motion": {}
                    "Tracks": {},},
                secondId: {
                    "frame_id": secondId,
                    "RoIs": {},
                    "Assocs": {},
                    "Motion": {}
                    "Tracks": {},},
            }
        """

        if not os.path.exists(path):
            print("(EE) The given path does not exist")
            sys.exit(1)

        onlyFiles = []
        if os.path.isdir(path):
            onlyFiles = [f for f in os.listdir(path) if os.path.isfile(os.path.join(path, f))]
        else:
            onlyFiles.append(os.path.basename(path))
            path = path.replace(onlyFiles[0], "")
        onlyFiles = sorted(onlyFiles)

        userRegex = re.compile(fileNameFilter)
        filteredFiles = []
        for file in onlyFiles:
            if re.search(userRegex, file):
                filteredFiles.append(file)

        frames = dict()
        for file in filteredFiles:
            print("Processing '" + path + "/" + file + "' file...", end=' ')
            (curFrame, frameId) = FMDTLogParser.parseFile(path, file)
            print("(frameId = " + str(frameId) + ")", end=' ')
            frames[frameId] = {**{"frame_id": frameId}, **curFrame}
            print("Done.", end="\r")
        print("", end="\n")
        return frames

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

frames = FMDTLogParser.parseFiles(args.logPath, args.filter)

print("Writing '" + args.outJsonFile + "' file...", end=' ')
import json
with open(args.outJsonFile, 'w') as fp:
    json.dump(frames, fp)
print("Done.")
