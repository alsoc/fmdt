# Choose a directory as refereces

./compare.py --exe-args "--in-video ../build/2022_05_31_tauh_34_meteors.mp4 --track-all" --list-exe "fmdt-detect, fmdt-detect-rt-pip, fmdt-detect-rt-seq" --refs-path ../build/refs

#-----------------------------------------------------------------------------------------------------------------------

# Choose an executable as referece

./compare.py --in-video ../build/2022_05_31_tauh_34_meteors.mp4 --new-ref-exe fmdt-detect