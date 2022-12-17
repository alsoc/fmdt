#!/bin/bash
set -x

wget https://lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4 .

for arg in "$@"
do
    cd ${arg}
    valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./exe/fmdt-detect --fra-start 0 --fra-end 20 --in-video ../2022_05_31_tauh_34_meteors.mp4 --out-stats out_detect_stats --out-bb out_detect_bb.txt --out-frames out_detect_frames --track-all --fra-star-min 15 --out-mag out_detect_mag.txt
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    if test -f "./exe/fmdt-detect-rt-seq"; then
        valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./exe/fmdt-detect-rt-seq --fra-start 0 --fra-end 20 --in-video ../2022_05_31_tauh_34_meteors.mp4 --out-stats out_detect_stats --out-bb out_detect_bb.txt --out-frames out_detect_frames --track-all --fra-star-min 15 --out-mag out_detect_mag.txt
        rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
        valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./exe/fmdt-detect-rt-pip --fra-start 0 --fra-end 20 --in-video ../2022_05_31_tauh_34_meteors.mp4 --out-stats out_detect_stats --out-bb out_detect_bb.txt --out-frames out_detect_frames --track-all --fra-star-min 15 --out-mag out_detect_mag.txt
        rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    fi
    cd ..
done
