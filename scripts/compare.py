import os
import hashlib

PATH_HEAD = ".."
PATH_BUILD = PATH_HEAD+"/build"
PATH_EXE = PATH_BUILD+"/exe"

EXE_REF = "fmdt-detect"
L_EXE_CMP = ["fmdt-detect-rt", "fmdt-detect-rt2"]
# L_EXE_CMP = ["fmdt-detect-rt"]
LIST_EXE = L_EXE_CMP + [EXE_REF]

def exec(): 

    in_video       = " --in-video       " + PATH_BUILD + "/2022_05_31_tauh_34_meteors.mp4" 
    fra_start      = " --fra-start      " + str(0) 
    fra_end        = " --fra-end        " + str(10000) 
    skip_fra       = " --skip-fra       " + str(0) 
    light_min      = " --light-min      " + str(55)  
    light_max      = " --light-max      " + str(80)  
    surface_min    = " --surface-min    " + str(3)  
    surface_max    = " --surface-max    " + str(1000)  
    k              = " --k              " + str(3)  
    r_extrapol     = " --r-extrapol     " + str(5)  
    angle_max      = " --angle-max      " + str(20)  
    fra_star_min   = " --fra-star-min   " + str(15)  
    fra_meteor_min = " --fra-meteor-min " + str(3)  
    fra_meteor_max = " --fra-meteor-max " + str(100)  
    diff_dev       = " --diff-dev       " + str(4.0) 
    track_all      = " --track-all      " 

    print("OPTIONS  :")
    print("\t"+in_video       )
    print("\t"+fra_start      )
    print("\t"+fra_end        )
    print("\t"+skip_fra       )
    print("\t"+light_min      )
    print("\t"+light_max      )
    print("\t"+surface_min    )
    print("\t"+surface_max    )
    print("\t"+k              )
    print("\t"+r_extrapol     )
    print("\t"+angle_max      )
    print("\t"+fra_star_min   )
    print("\t"+fra_meteor_min )
    print("\t"+fra_meteor_max )
    print("\t"+diff_dev       )
    print("\t"+track_all      )


    for i in LIST_EXE: 
        out_bb         = " --out-bb        " + PATH_BUILD + "/" + i +  "/" + "bb.txt" 
        out_frames     = " --out-frames    " + PATH_BUILD + "/" + i  
        out_stats      = " --out-stats     " + PATH_BUILD + "/" + i  
        bin            = PATH_EXE + "/" + i 
        print(out_bb)
        print(out_frames)
        print(out_stats)
        print(bin)

        exec = bin + in_video + fra_start + fra_end + skip_fra + light_min + light_max + surface_min + surface_max + k + r_extrapol + angle_max + fra_star_min + fra_meteor_min + fra_meteor_max + diff_dev + track_all + out_bb + out_frames + out_stats 

        print(exec)
        os.system(exec)
    
    return 0

def diff_pgm(filename, p_v1, p_v2):
    res = []
    md5_hash1 = hashlib.md5()
    md5_hash2 = hashlib.md5()
    
    dir_v1 = PATH_BUILD + "/" + p_v1
    filename_v1 = os.path.join(dir_v1, filename)

    dir_v2 = PATH_BUILD + "/" + p_v2
    filename_v2 = os.path.join(dir_v2, filename)

    f_v1 = open(filename_v1, "rb")
    f_v2 = open(filename_v2, "rb")

    md5_hash1.update(f_v1.read())
    key1 = md5_hash1.hexdigest()
    md5_hash2.update(f_v2.read())
    key2 = md5_hash2.hexdigest()

    if key1 != key2 :
        res.append((filename, p_v1, p_v2, "X", key1, key2))
    return res

def diff_txt(filename, p_v1, p_v2):
    res = []
    
    dir_v1 = PATH_BUILD + "/" + p_v1
    filename_v1 = os.path.join(dir_v1, filename)

    dir_v2 = PATH_BUILD + "/" + p_v2
    filename_v2 = os.path.join(dir_v2, filename)

    f_v1 = open(filename_v1, "r")
    f_v2 = open(filename_v2, "r")

    L_v1 = f_v1.readlines()
    L_v2 = f_v2.readlines()

    size_v1 = len(L_v1)
    for i in range (size_v1):
        if L_v1[i] != L_v2[i]: 
            res.append((filename, p_v1, p_v2, i+1, L_v1[i][:-2], L_v2[i][:-2]))
            break

    f_v1.close()
    f_v2.close()
    return res

def display_res(res):


    print("---------------|-----------------|-----------------||----------|--------------------------------------------------|--------------------------------------------------")
    print("           FILE|             EXE1|             EXE2||      LINE|                     diff for txt/checksum for pgm|                     diff for txt/checksum for pgm")
    print("---------------|-----------------|-----------------||----------|--------------------------------------------------|--------------------------------------------------")


    size = len(res)
    for i in range (size):
        (a,b,c,d,e,f) = res[i] 
        print("{:>15s}|{:>17s}|{:>17s}||{:>10s}|{:>50s}|{:>50s}".format(a,b,c,str(d),e,f))
    return 0 


def main_diff(p_v1, p_v2):
    res = []

    dir = PATH_BUILD + "/" + p_v1
    listdir = os.listdir(dir)
    listdir.sort()

    for filename in listdir :
        f = os.path.join(dir, filename)
        if not os.path.isfile(f):
            continue

        # if checksum(): OPTION

        elif ".pgm" in filename:
            r = diff_pgm(filename, p_v1, p_v2)
            if r != []:
                res = res + r 
        elif ".txt" in filename:
            r = diff_txt(filename, p_v1, p_v2)
            if r != []:
                res = res + r

    display_res(res)

    return 0


def main():
    # faire 2 exec séparer?
    exec()  
    for exe_cmp in L_EXE_CMP :
        main_diff(EXE_REF, exe_cmp)

    return 0


main()
