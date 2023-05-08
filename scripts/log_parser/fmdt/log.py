import os
import sys
import re
import shutil
import math

class LogParser:
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
            # "id": "rid_t", # uncomment this line to index associations with `rid_t`
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
                "track_state": "state",
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
        `lines` (list[str]): A list of strings. Each string corresponds to a
            line in the FMDT log file.
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
            specified in the `tableInfo` input parameter. If the `id` field is
            not specified in `tableInfo` then the returned `entries` are not a
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

        # this print is useful to fill the 'cols_rename' field in the LogParser.Tables global variable
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
        `filePath` (str): A path to a file (without the file name). For
            instance, '/home/toto/dir/' is valid and '/home/toto/dir/00012.txt'
            is NOT.
        `fileBasename` (str): File name without the full path. For instance,
            '00012.txt' is valid and '/home/toto/dir/00012.txt' is NOT.

        Returns
        -------
        (`curFrame`, `frameId`) (dict, int): `curFrame` is a dictionary of the
            parsed table in the file. `frameId` is the identifier of the frame
            corresponding of the file parsed. `curFrame` has the following form:
            {
                "RoIs": {...},
                "Assocs": {...},
                "Motion": {...},
                "Tracks": {...},},
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
            for vTable in LogParser.Tables:
                nMatch = 0
                for sregex in LogParser.Tables[vTable]["regex"]:
                    regexc = re.compile(sregex, re.IGNORECASE)
                    if re.match(regexc, name):
                        nMatch += 1
                if nMatch == len(LogParser.Tables[vTable]["regex"]):
                    skip_table = 0
                    tableName = LogParser.Tables[vTable]["name"]
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
                    regexc = re.compile(LogParser.Tables[tableName]["search"]["how"], re.IGNORECASE)
                    frameId = int(re.search(regexc, name).group(0))

                (entries, cur) = LogParser.parseTab(lines, l, LogParser.Tables[tableName])
                l = cur + 1
                if "single_entry" in LogParser.Tables[tableName] and LogParser.Tables[tableName]["single_entry"]:
                    curFrame[tableName] = entries[0]
                else:
                    curFrame[tableName] = entries
        return (curFrame, frameId)

    @staticmethod
    def parseFiles(path:str, fileNameFilter:str=".*", verbose:bool=False):
        """Parse information from one or multiple FMDT log files.

        Parameters
        ----------
        `path` (str): A path to a file or the a directory of FMDT log files.
        `fileNameFilter` (str): Regex for the file name. If the regex is not
            matched then the file is not parsed.

        Returns
        -------
        `frames` (dict): dictionary of the parsed files. `frames` is indexed by
            the frame id:
            {
                firstId: {
                    "frame_id": firstId,
                    "RoIs": {...},
                    "Assocs": {...},
                    "Motion": {...},
                    "Tracks": {...},},
                secondId: {
                    "frame_id": secondId,
                    "RoIs": {...},
                    "Assocs": {...},
                    "Motion": {...},
                    "Tracks": {...},},
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
            if verbose:
                print("# Processing '" + path + "/" + file + "' file...", end=' ')
            (curFrame, frameId) = LogParser.parseFile(path, file)
            if verbose:
                print("(frameId = " + str(frameId) + ")", end=' ')
            frames[frameId] = {**{"frame_id": frameId}, **curFrame}
            if verbose:
                print("Done.", end="\r")
        if verbose:
            print("", end="\n")
        return frames

    @staticmethod
    def parseTracks2RoIsFile(path:str, fileNameFilter:str=".*"):
        """Parse tracks to RoIs files.

        Parameters
        ----------
        `path` (str): A path to a file containing the RoI ids per track id.
        `fileNameFilter` (str): Regex for the file name. If the regex is not
            matched then the file is not parsed.

        Returns
        -------
        `tracks2RoIs` (dict): dictionary of the entries. `tracks2RoIs` is
            indexed by track id (`tid`):
            {
                tid1: [rid1_1, rid1_2, ...],
                tid2: [rid2_1, rid2_1, rid2_3, ...],
                tid3: [rid3_1, ...],
                ...
            }
            Note that if `ridx_y` == 0, it means that the track has been
            extrapolated for this frame and thus, there is no corresponding RoI.
        """

        if not os.path.isfile(path):
            print("(EE) The given path is not a file")
            sys.exit(1)

        f = open(path, "r")
        lines = f.readlines()
        f.close()

        tracks2RoIs = {}
        for line in lines:
            regex = re.compile(r"\s+")
            line2 = regex.sub(" ", line).strip()
            cols = line2.split(" ")

            tid = int(cols[0].strip())
            RoIIds = []
            for col in cols[2:]:
                RoIIds.append(int(col))
            tracks2RoIs[tid] = RoIIds

        return tracks2RoIs

    @staticmethod
    def getLastTracks(frames:dict):
        """Return the tracks from the last frame.

        Parameters
        ----------
        `frames` (dict): a dictionnary of frames.

        Returns
        -------
        Return the tracks from the last frame.
            {
                tid1: {
                    "tid": tid1,
                    "fbeg": fbeg1,
                    "xbeg": xbeg1,
                    "ybeg": ybeg1,
                    "fend": fend1,
                    "xend": xend1,
                    "yend": yend1,
                    "otype": otype1,
                    "reason": reason1},
                tid2: {
                    "tid": tid2,
                    "fbeg": fbeg2,
                    "xbeg": xbeg2,
                    "ybeg": ybeg2,
                    "fend": fend2,
                    "xend": xend2,
                    "yend": yend2,
                    "otype": otype2,
                    "reason": reason2},
                ...
            }
        """
        lastFrame = sorted(frames.keys())[-1]
        return frames[lastFrame]["Tracks"]

    @staticmethod
    def writeTracks(tracks:dict, path:str):
        """Write the tracks.

        Parameters
        ----------
        `tracks` (dict): a dictionnary of tracks
        `path` (str): a path to a file to write the tracks
        """

        f = open(path, "w")

        f.write("# Tracks [" + str(len(tracks)) + "]:\n")
        f.write("# -------||---------------------------||---------------------------||---------\n")
        f.write("#  Track ||           Begin           ||            End            ||  Object \n")
        f.write("# -------||---------------------------||---------------------------||---------\n")
        f.write("# -------||---------|--------|--------||---------|--------|--------||---------\n")
        f.write("#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type \n")
        f.write("# -------||---------|--------|--------||---------|--------|--------||---------\n")

        for tid in tracks:
            fbeg = tracks[tid]["fbeg"]
            xbeg = tracks[tid]["xbeg"]
            ybeg = tracks[tid]["ybeg"]
            fend = tracks[tid]["fend"]
            xend = tracks[tid]["xend"]
            yend = tracks[tid]["yend"]
            otype = tracks[tid]["otype"]

            line = '   {:5d} || {:7d} | {:6.1f} | {:6.1f} || {:7d} | {:6.1f} | {:6.1f} || {:>7} \n'.format(tid, fbeg, xbeg, ybeg, fend, xend, yend, otype)
            f.write(line)

        f.close()

    @staticmethod
    def insertRoIsInTracks(tracks:dict, frames:dict, tracks2RoIs:dict):
        """Insert the RoIs into a `tracks` dictionnary.

        Parameters
        ----------
        `tracks` (dict): a dictionnary of tracks.
        `frames` (dict): a dictionnary of frames.
        `tracks2RoIs` (dict): a dictionnary that containd RoIs corresponding to
            each track.

        Returns
        -------
        `tracks` (dict): the tracks with a new field containing the list of
            associated RoIs:
            {
                tid1: {
                    "tid": tid1,
                    "fbeg": fbeg1,
                    "xbeg": xbeg1,
                    "ybeg": ybeg1,
                    "fend": fend1,
                    "xend": xend1,
                    "yend": yend1,
                    "otype": otype1,
                    "reason": reason1,
                    "RoIs": [{RoI1_1}, {RoI1_2}, ...]}, <==
                tid2: {
                    "tid": tid2,
                    "fbeg": fbeg2,
                    "xbeg": xbeg2,
                    "ybeg": ybeg2,
                    "fend": fend2,
                    "xend": xend2,
                    "yend": yend2,
                    "otype": otype2,
                    "reason": reason2,
                    "RoIs": [{RoI2_1}, ...]}, <==
                ...
            }
        """

        for tid in tracks2RoIs:
            fbeg = tracks[tid]["fbeg"]
            fend = tracks[tid]["fend"]
            flen = fend - fbeg + 1
            if flen != len(tracks2RoIs[tid]):
                print("(EE) len(tracks2RoIs[tid]) != flen, tid = " + str(tid) + ", flen = " + str(flen) + ", len(tracks2RoIs[tid]) = " + str(len(tracks2RoIs[tid])))
                sys.exit(-1)
            RoIs = []
            f = 0
            for rid in tracks2RoIs[tid]:
                if "RoIs" not in frames[fbeg + f]:
                    print("(EE) \"RoIs\" not in frames[fbeg + f], fbeg = " + str(fbeg) + ", f = " + str(f))
                    sys.exit(-1)
                if rid != 0:
                    RoIs.append(frames[fbeg + f]["RoIs"][rid])
                else:
                    RoIs.append(None)
                f += 1
            tracks[tid]["RoIs"] = RoIs
        return tracks

    @staticmethod
    def extractFeature(tracks:dict, feature:str, path:str):
        """Extract a given RoI feature per track.

        Write this feature into a file in the following form:

        ```
        {tid} {otype} {f1} {f2} {...} {fn}
        ```

        Each line corresponds to a track/object. `{f1}` is the first feature
        value of the track/object of `{tid}` id. `{f2}` is the second feature
        value (in the second frame where the object has been tracked). And so
        on, until the last feature value `{fn}`. Note that sometime the feature
        value can be `0`, it means that the object has been extrapolated on this
        frame, thus the feature cannot be computed.

        Parameters
        ----------
        `tracks` (dict): a dictionnary of tracks.
        `feature` (str): name of the feature field in the RoIs.
        `path` (srt): path to the file to write the features.
        """

        f = open(path, "w")
        for tid in tracks:
            f.write(str(tid) + " " + tracks[tid]["otype"] + " ")
            if "RoIs" not in tracks[tid]:
                print("(EE) \"RoIs\" not in tracks[tid], tid = " + str(tid))
                sys.exit(-1)

            for roi in tracks[tid]["RoIs"]:
                if roi == None:
                    f.write(str(0) + " ")
                else:
                    if feature not in roi:
                        print("(EE) feature not in roi, feature = " + feature)
                        sys.exit(-1)
                    f.write(str(roi[feature]) + " ")
            f.write("\n")
        f.close()

    @staticmethod
    def getBoundingBoxes(tracks:dict, frames:dict, tracks2RoIs:dict, extrapolate:bool=True):
        """Extract the list of bounding boxes sorted by frame order.

        Parameters
        ----------
        `tracks` (dict): a dictionnary of tracks.
        `frames` (dict): a dictionnary of frames.
        `tracks2RoIs` (dict): a dictionnary that containd RoIs corresponding to
            each track.
        `extrapolate` (bool): a boolean to enable the extrapolated bounding
            boxes (linear extrapolation).

        Returns
        -------
        `bbs` (list): a list of bounding boxes in the following form:
            [{
                "fid": fid1,
                "x_radius": xrad1,
                "y_radius": yrad1,
                "x_center": xcen1,
                "y_center": ycen1,
                "tid": tid1,
                "extrapolated": True of False,
            }, {
                "fid": fid2,
                "x_radius": xrad2,
                "y_radius": yrad2,
                "x_center": xcen2,
                "y_center": ycen2,
                "tid": tid2,
                "extrapolated": True of False,
            }, ...
            ]
        """

        for tid in tracks2RoIs:
            fbeg = tracks[tid]["fbeg"]
            fend = tracks[tid]["fend"]
            fcur = fbeg
            if len(tracks2RoIs[tid]) != (fend - fbeg + 1):
                print("(EE) len(tracks2RoIs[tid]) != (fend - fbeg + 1)")
                sys.exit(1)
            e = 0
            posHist0 = {'x': None, 'y': None}
            posHist1 = {'x': None, 'y': None}
            for rid in tracks2RoIs[tid]:
                entry = {}
                if rid == 0:
                    # compute bounding box for extrapolated tracks
                    if extrapolate and xcenter and ycenter and xradius and yradius:
                        if e == 0:
                            theta1 = frames[fcur - 1]["Motion"]["theta2"]
                            tx1 = frames[fcur - 1]["Motion"]["tx2"]
                            ty1 = frames[fcur - 1]["Motion"]["ty2"]

                            x2_2 = posHist1["x"]
                            y2_2 = posHist1["y"]

                            x2_1 = tx1 + x2_2 * math.cos(theta1) - y2_2 * math.sin(theta1);
                            y2_1 = ty1 + x2_2 * math.sin(theta1) + y2_2 * math.cos(theta1);

                            x1_1 = posHist0["x"]
                            y1_1 = posHist0["y"]

                            dx = x1_1 - x2_1
                            dy = y1_1 - y2_1

                        theta = frames[fcur]["Motion"]["theta2"]
                        tx = frames[fcur]["Motion"]["tx2"]
                        ty = frames[fcur]["Motion"]["ty2"]

                        xcenter = int(dx + tx + posHist0["x"] * math.cos(theta) - posHist0["y"] * math.sin(theta));
                        ycenter = int(dy + ty + posHist0["x"] * math.sin(theta) + posHist0["y"] * math.cos(theta));
                    else:
                        xcenter = None
                        ycenter = None
                        xradius = None
                        yradius = None
                    e += 1
                    posHist1 = posHist0
                    posHist0 = {'x': xcenter, 'y': ycenter}
                else:
                    e = 0
                    xmin = frames[fcur]["RoIs"][rid]["xmin"]
                    xmax = frames[fcur]["RoIs"][rid]["xmax"]
                    ymin = frames[fcur]["RoIs"][rid]["ymin"]
                    ymax = frames[fcur]["RoIs"][rid]["ymax"]
                    xcenter = math.ceil((xmin + xmax) / 2)
                    ycenter = math.ceil((ymin + ymax) / 2)
                    xradius = xcenter - xmin
                    yradius = ycenter - ymin
                    if extrapolate:
                        x = frames[fcur]["RoIs"][rid]["x"]
                        y = frames[fcur]["RoIs"][rid]["y"]
                        posHist1 = posHist0
                        posHist0 = {'x': x, 'y': y}

                entry["x_radius"] = xradius
                entry["y_radius"] = yradius
                entry["x_center"] = xcenter
                entry["y_center"] = ycenter
                entry["tid"] = tid
                entry["extrapolated"] = rid == 0

                if "BBs" not in frames[fcur]:
                    frames[fcur]["BBs"] = []
                frames[fcur]["BBs"].append(entry)

                fcur += 1

        bbs = []
        for fid in frames:
            if "BBs" in frames[fid]:
                for b in frames[fid]["BBs"]:
                    bb = {}
                    bb["fid"] = fid
                    bb["x_radius"] = b["x_radius"]
                    bb["y_radius"] = b["y_radius"]
                    bb["x_center"] = b["x_center"]
                    bb["y_center"] = b["y_center"]
                    bb["tid"] = b["tid"]
                    bb["extrapolated"] = b["extrapolated"]
                    bbs.append(bb)

        return bbs

    @staticmethod
    def writeBoundingBoxes(bbs:list, path:str):
        """Write the bounding boxes in a file.

        Parameters
        ----------
        `bbs` (list): a list of bounding boxes ordered by frame id
        `path` (str): a path to a file to write the bounding boxes
        """

        f = open(path, "w")

        for bb in bbs:

            fid = bb["fid"]
            xradius = bb["x_radius"]
            yradius = bb["y_radius"]
            xcenter = bb["x_center"]
            ycenter = bb["y_center"]
            tid = bb["tid"]
            if bb["extrapolated"]:
                extrapolated = 1
            else:
                extrapolated = 0

            if xradius == None: xradius = 0
            if yradius == None: yradius = 0
            if xcenter == None: xcenter = 0
            if ycenter == None: ycenter = 0

            f.write(str(fid) + " " +
                    str(xradius) + " " +
                    str(yradius) + " " +
                    str(xcenter) + " " +
                    str(ycenter) + " " +
                    str(tid) + " " +
                    str(extrapolated) + "\n")

        f.close()
