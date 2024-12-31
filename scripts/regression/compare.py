#!/usr/bin/env python3

import os
import shutil
import sys
import hashlib
import argparse
import re
import shutil
import struct

# A dictionary where the keys are the columns for whom there is a tolerated
# percentage of error and where the values are an array of two elements:
#  1. The maximum tolerated percentage of error (ratio between 0 and 1)
#  2. The minimum significant difference
#
#                 col name                               max ratio   min diff
colsFloatErr = {                   "center ~ x":        [     0.05,   1.0e-05 ],
                                   "center ~ y":        [     0.05,   1.0e-05 ],
                                  "ellipse ~ a":        [     0.05,   1.0e-05 ],
                                  "ellipse ~ b":        [     0.05,   1.0e-05 ],
                                  "ellipse ~ ratio":    [     0.05,   1.0e-05 ],
                                 "distance ~ pixels":   [     0.05,   1.0e-05 ],
                                    "error ~ dx":       [     0.25,   1.0e-05 ],
                                    "error ~ dy":       [     0.25,   1.0e-05 ],
                                    "error ~ e":        [     0.05,   1.0e-05 ],
                  "first motion estimation ~ theta":    [     0.05,   1.0e-05 ],
                  "first motion estimation ~ tx":       [     0.05,   1.0e-05 ],
                  "first motion estimation ~ ty":       [     0.05,   1.0e-05 ],
                  "first motion estimation ~ mean err": [     0.05,   1.0e-05 ],
                  "first motion estimation ~ std dev":  [     0.05,   1.0e-05 ],
                 "second motion estimation ~ theta":    [     0.05,   1.0e-05 ],
                 "second motion estimation ~ tx":       [     0.05,   1.0e-05 ],
                 "second motion estimation ~ ty":       [     0.05,   1.0e-05 ],
                 "second motion estimation ~ mean err": [     0.05,   1.0e-05 ],
                 "second motion estimation ~ std dev":  [     0.05,   1.0e-05 ],
                                    "begin ~ x":        [     0.05,   1.0e-05 ],
                                    "begin ~ y":        [     0.05,   1.0e-05 ],
                                      "end ~ x":        [     0.05,   1.0e-05 ],
                                      "end ~ y":        [     0.05,   1.0e-05 ],
               }

parser = argparse.ArgumentParser(prog='compare.py', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('--exe-args',    action='store',      dest='exeArgs',    type=str,   default="", required=True, help='String of exe-argurments.')
parser.add_argument('--list-exe',    action='store',      dest='strListExe', type=str,   default="", required=True, help='List of executables to compare (format: "exe0, exe1, ... , exeN").')
parser.add_argument('--refs-path',   action='store',      dest='refsPath',   type=str,   default="refs",            help='Path of the references to compare.')
parser.add_argument('--new-ref-exe', action='store',      dest='newRefExe',  type=str,   default="",                help='Executable considered for ref.')
parser.add_argument('--out-path',    action='store',      dest='outPath',    type=str,   default="output",          help='Output folder of the tested binaries.')
parser.add_argument('--build-path',  action='store',      dest='buildPath',  type=str,   default="",                help='Path to the directory containing the binaries.')
parser.add_argument('--hexa-float',  action='store_true', dest='hexaFloat',              default=False,             help='Enable hexadecimal float checking from "*_hexa.txt" log files.')

def hex_to_float_struct(hex_str):
    hex_str = hex_str.strip().replace("0x", "")
    byte_array = bytes.fromhex(hex_str)
    float_num = struct.unpack('!f', byte_array)[0]
    return float_num

def strListExe_to_listExe():
    return args.strListExe.replace(' ', '').split(',')

def main_exec(L_EXE):
    print("#")
    print("# EXE:")
    if os.path.exists(args.outPath):
        print("# (WW) Deleting '" + args.outPath + "' folder!")
    shutil.rmtree(args.outPath, ignore_errors=True)
    os.mkdir(args.outPath);
    for exe in L_EXE:
        full_exe = args.buildPath + exe
        exec = full_exe + " " + args.exeArgs
        print("# "+ exec)
        os.system(exec)
        if os.path.exists(args.outPath + "_" + exe):
            print("# (WW) Deleting '" + args.outPath + "_" + exe + "' folder!")
        shutil.rmtree(args.outPath + "_" + exe, ignore_errors=True)
        shutil.move(args.outPath, args.outPath + "_" + exe);
    return 0

def diff_pgm(filename, p_v1, p_v2):
    res = []
    md5_hash1 = hashlib.md5()
    md5_hash2 = hashlib.md5()
    
    f_v1 = open(p_v1, "rb")
    f_v2 = open(p_v2, "rb")

    md5_hash1.update(f_v1.read())
    key1 = md5_hash1.hexdigest()
    md5_hash2.update(f_v2.read())
    key2 = md5_hash2.hexdigest()

    if key1 != key2 :
        res.append((filename, "-", "MD5 file hash (8 first hex digits)", key1[:8], key2[:8]))
    return res

def diff_bb(filename, p_v1, p_v2):
    res = []
    
    f_v1 = open(p_v1, "r")
    f_v2 = open(p_v2, "r")

    L_v1 = f_v1.readlines()
    L_v2 = f_v2.readlines()

    size_v1 = len(L_v1)
    for i in range (size_v1):
        if L_v1[i] != L_v2[i]: 
            res.append((filename, i+1, L_v1[i][:-1], L_v2[i][:-1]))
            break

    f_v1.close()
    f_v2.close()
    return res


def diff_stats(filename, p_v1, p_v2):
    stats1 = parser_stats(p_v1)
    stats2 = parser_stats(p_v2)
    # type(stats1) = list[name_tab(str), line_title_tab(int), list[column_name(str), data(str)]]

    # if all data are same
    if stats1 == stats2 : 
        return []

    nb_tabs1 = len(stats1)
    nb_tabs2 = len(stats2)
    if nb_tabs1 != nb_tabs2:
        return [(filename, "-", "number of tabs", str(nb_tabs1), str(nb_tabs2))]

    for i in range (nb_tabs1):
        # if not the same tab name
        if stats1[i][0] != stats2[i][0] :
            return [(filename, stats1[i][1]+1, stats1[i][0], stats2[i][0])]
        
        for col1, data1 in stats1[i][2] :
            for col2, data2 in stats2[i][2]:
                if col1 == col2 :
                    if data1 != data2: 
                        size = len(data1)
                        for k in range(size):
                            val1 = data1[k].strip()
                            val2 = data2[k].strip()

                            if val1 != val2:
                                # error management
                                if col1 in colsFloatErr:
                                    floatPattern = re.compile(r'^[-+]?(?:\d*\.*\d+)$')
                                    hexaPattern = re.compile(r'^0x([0-9a-fA-F]{8})$')
                                    m1 = re.search(hexaPattern, val1)
                                    if m1:
                                        val1f = hex_to_float_struct(m1.group(1))
                                    else:
                                        m1bis = re.search(floatPattern, val1)
                                        if m1bis:
                                            val1f = float(val1)
                                    m2 = re.search(hexaPattern, val2)
                                    if m2:
                                        val2f = hex_to_float_struct(m2.group(1))
                                    else:
                                        m2bis = re.search(floatPattern, val2)
                                        if m2bis:
                                            val2f = float(val2)

                                    if 'val1f' in locals() and 'val2f' in locals():
                                        if val1f != 0 and val2f != 0 and val1f * val2f >= 0:
                                            aval1f = abs(val1f)
                                            aval2f = abs(val2f)
                                            if aval1f > aval2f:
                                                ratio = aval2f / aval1f
                                            else:
                                                ratio = aval1f / aval2f
                                            delta = 1 - ratio
                                            if (delta < colsFloatErr[col1][0]):
                                                continue

                                        if val1f > val2f:
                                            dist = val1f - val2f
                                        else:
                                            dist = val2f - val1f
                                        if (dist < colsFloatErr[col1][1]):
                                            continue

                                        return [(filename, stats1[i][1]+8+k, col1, "{:.4e}".format(val1f), "{:.4e}".format(val2f))]

                                return [(filename, stats1[i][1]+8+k, col1, val1, val2)]
                    break
    return []

def parser_Tab(Lines, name, start, size_max):
    List_columns = []
    len_subtitle_index = 0
    lenght = 2 # offset

    # set up titles and subtitles
    List_title = (Lines[start+2][2:-1] ).split("||")
    List_title = [x.lower() for x in List_title]
    regexRmParentheses = re.compile(r'(\(.*\))')
    Shortened_list_title = [re.sub(regexRmParentheses, "", x) for x in List_title]
    List_title_size = [len(txt) for txt in List_title] 
    List_subtitle  = (Lines[start+5][2:-1]).replace("||","|").split("|")
    List_subtitle = [x.lower() for x in List_subtitle]
    List_subtitle_size = [len(txt) for txt in List_subtitle]
    len_title = len(List_title)

    for i in range(len_title):
        len_title_index = List_title_size[i]
        
        cpt = 0
        while cpt != len_title_index : 
            # UGLY
            if cpt == 0 :
                cpt = List_subtitle_size[len_subtitle_index]
            else :
                if (len(List_subtitle_size) <= len_subtitle_index):
                    sys.exit("(EE) Something went wrong when parsing the table named: '" + name.replace("\n", "") + "'.")
                cpt += List_subtitle_size[len_subtitle_index] + 1 

            List_columns += [(Shortened_list_title[i].strip()+" ~ "+List_subtitle[len_subtitle_index].strip(),lenght,lenght + List_subtitle_size[len_subtitle_index], [])]
            lenght += List_subtitle_size[len_subtitle_index] + 1
            len_subtitle_index += 1
        lenght += 1
        
    # add data
    cur = start + 7
    while cur != size_max and Lines[cur][0] != '#'  :
        for (column, begin, end, data) in List_columns :
            data.append(Lines[cur][begin:end])
        cur += 1
    
    List_columns = [(column, data) for (column, begin, end, data) in List_columns]
    
    return ((name[:-1], start, List_columns), cur)

# only help to dev
def display_tab_colums(input): 
    size = len(input)

    for name, line, tab in input:
        print(name,end = '')
        for column, trash0, trash1, data in tab:
            print('{0:15s}'.format(column), end = ' ')
        print("\n")
    return None

# liste des tables a ne pas traiter dans la comparaison
table_exceptions = ["no conflict found", "association conflicts"]

def parser_stats(path_filename):
    f = open(path_filename, "r")

    Lines = f.readlines()

    f.close()

    size = len(Lines)
    res = []
    i = 0 
    
    # on suppose qu'il n'y a que des tab
    while i < size :
        name = Lines[i].lower();
        skip_table = 0
        # on vérifie que le nom de la table à traiter n'est pas une exception
        for excep in table_exceptions:
            if excep in name:
                skip_table = 1;
        # si c'est une exception alors on avance jusqu'a la nouvelle table
        if skip_table:
            if i + 1 < size:
                i = i + 1
            while i < size and Lines[i][0] != '#':
                i = i + 1
            # sauter la ligne ou il y a un seul caractere #
            if i + 1 < size:
                i = i + 1
        # sinon on traite la table normalement
        else:
            (tuple, cur)= parser_Tab(Lines, name, i, size)
            i = cur + 1
            res.append(tuple)
    return res

def display_res(res, exe_name):
    if res == [] :
        print("# -------------------------------------------")
        print("# ---- {:>25s} checked ----".format(exe_name))
        print("# -------------------------------------------")
        print("#")
        return 0

    print("# -----------------|---------------------------||-----------|-------------------------------------|--------------|--------------")
    print("#  FILE            | EXE                       ||      LINE | COLUMN NAME                         |          REF |          NEW ")
    print("# -----------------|---------------------------||-----------|-------------------------------------|--------------|--------------")

    size = len(res)
    for i in range (size):
        (file,line,col,txt0,txt1) = res[i]
        print("   {:<15s} | {:<25s} || {:>9s} | {:<35s} | {:>12s} | {:>12s} ".format(file, exe_name, str(line), col, txt0, txt1))
    print("#")
    return 1

file_exceptions = ["bb.txt", "mag.txt", "tracks.txt", "trk2roi.txt"]

def main_diff(path_ref, exe_name):
    res = []

    # list of files in refs/
    listdir = os.listdir(path_ref)
    listdir.sort()

    for filename in listdir :
        f_ref = os.path.join(path_ref, filename)

        if not os.path.isfile(f_ref):
            continue

        dir_tocmp = args.outPath + "_" + exe_name
        f_tocmp = os.path.join(dir_tocmp, filename)
        
        if ".pgm" in filename: # image
            r = diff_pgm(filename, f_ref, f_tocmp)
            if r != []:
                res += r
        else:
            skip = False
            for excep in file_exceptions:
                if excep in filename:
                    skip = True
                    continue
            if not skip and ".txt" in filename: # stats
                if (    args.hexaFloat and "_hexa"     in filename) or \
                   (not args.hexaFloat and "_hexa" not in filename):
                    r = diff_stats(filename, f_ref, f_tocmp)
                    if r != []:
                        res += r

    return display_res(res, exe_name)

def main():
    print("#")
    print("#         SCRIPT COMPARE.PY IS STARTING")
    print("#")

    L_EXE  = strListExe_to_listExe()

    # execute and save all the data in ../build/name_executable
    main_exec(L_EXE)

    print("#")
    print("#         END OF THE DATA GENERATION")
    print("#")

    # choose the references
    if args.newRefExe == "" :
        ref = args.refsPath
    else :
        ref = args.buildPath + args.newRefExe

    print("#")
    print("# The references directory : " + ref )
    print("#")

    # compare all the data with refs
    errors = 0
    for exe_cmp in L_EXE :
        errors += main_diff(ref, exe_cmp)

    print("#")
    print("#         END OF THE SCRIPT")
    print("#")
    return errors

args = parser.parse_args()
parser.print_help()

errors = main()
sys.exit(errors)
