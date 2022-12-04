#!/usr/bin/env python3

import os
import sys
import hashlib
import argparse

PATH_HEAD = ".."
PATH_BUILD = PATH_HEAD+"/build"
PATH_EXE = PATH_BUILD+"/exe"

# List of executable to compare
# L_EXE = ["fmdt-detect", "fmdt-detect-rt-pip", "fmdt-detect-rt-seq", "fmdt-detect-rt2"]
# L_EXE = ["fmdt-detect", "fmdt-detect-rt-pip", "fmdt-detect-rt-seq"] # waiting to update fmdt-detect-rt2 

parser = argparse.ArgumentParser(prog='compare.py', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('--exe-args',    action='store', dest='exeArgs',     type=str,   default="", required=True,        help='String of exe-argurments')
parser.add_argument('--list-exe',    action='store', dest='strListExe',  type=str,   default="", required=True,        help='List of executables to compare (format: "exe0, exe1, ... , exeN"')
parser.add_argument('--refs-path',   action='store', dest='refsPath',    type=str,   default=PATH_BUILD + "/refs",     help='Path to the references to compare.')
parser.add_argument('--new-ref-exe', action='store', dest='newRefExe',   type=str,   default="",                       help='Executable considered for ref.')

def strListExe_to_listExe():
    return args.strListExe.replace(' ', '').split(',')

def main_exec(L_EXE):
    print("#")
    print("# EXE:")
    
    for i in L_EXE: 
        out_bb     = " --out-bb "     + PATH_BUILD + "/" + i + "/" + "bb.txt"
        out_frames = " --out-frames " + PATH_BUILD + "/" + i
        out_stats  = " --out-stats "  + PATH_BUILD + "/" + i
        bin        = PATH_EXE + "/" + i + " "

        exec = bin + args.exeArgs + out_bb + out_frames + out_stats
        print("# "+ exec)
        os.system(exec)
    
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
        res.append((filename, "X", key1, key2))
    return res

def diff_txt(filename, p_v1, p_v2):
    res = []
    
    f_v1 = open(p_v1, "rb")
    f_v2 = open(p_v2, "rb")

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

def display_res(res, exe_name):
    if res == [] :
        print("# -------------------------------------------")
        print("# ---- {:>25s} checked ----".format(exe_name))
        print("# -------------------------------------------")
        print("#")
        return 0    

    print("# ----------------|---------------------------||-----------|--------------------------------------------------------|--------------------------------------------------------")
    print("#            FILE |                       EXE ||      LINE |                                                  refs  |                          diff for txt/checksum for pgm ")
    print("# ----------------|---------------------------||-----------|--------------------------------------------------------|--------------------------------------------------------")

    size = len(res)
    for i in range (size):
        (file,line,txt0,txt1) = res[i] 
        print("  {:>15s} |{:>26s} ||{:>10s} |{:>55s} |{:>56s}".format(file, exe_name, str(line), str(txt0, errors='replace')[:51], str(txt1, errors='replace')[:51]))
    print("#")
    return 1

def main_diff(path_ref, exe_name):
    res = []

    # list of files in refs/
    listdir = os.listdir(path_ref)
    listdir.sort()

    for filename in listdir :
        f_ref = os.path.join(path_ref, filename)

        if not os.path.isfile(f_ref):
            continue

        dir_tocmp = PATH_BUILD + "/" + exe_name
        f_tocmp = os.path.join(dir_tocmp, filename)
        

        if ".pgm" in filename:
            r = diff_pgm(filename, f_ref, f_tocmp)
            if r != []:
                res = res + r 

        elif ".txt" in filename:
            r = diff_txt(filename, f_ref, f_tocmp)
            if r != []:
                res = res + r

    return display_res(res, exe_name)

def main():
    print("#")
    print("#         SCRIPT COMPARE.PY IS STARTING")
    print("#")

    L_EXE  = strListExe_to_listExe()

    # execute and save all the data in ../build/name_executable
    main_exec(L_EXE)  

    print("#")
    print("#         END OF THE DATA GENERATION")
    print("#")

    # choose the references
    if args.newRefExe == "" :
        ref = args.refsPath
    else :
        ref = PATH_BUILD + "/" + args.newRefExe

    print("#")
    print("# The references directory : " + ref )
    print("#")

    # compare all the data with refs
    errors = 0
    for exe_cmp in L_EXE :
        errors = errors + main_diff(ref, exe_cmp)

    print("#")
    print("#         END OF THE SCRIPT")
    print("#")
    return errors

args = parser.parse_args()
parser.print_help()

errors = main()
sys.exit(errors)
