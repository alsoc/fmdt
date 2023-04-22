#!/bin/bash
set -x

wget https://lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4 .

for arg in "$@"
do
    cd ${arg}
    mkdir out_ellipse_frames
    valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./bin/fmdt-ellipse        --vid-in-start 0 --vid-in-stop 20 --vid-in-path ../2022_05_31_tauh_34_meteors.mp4 --log-path out_ellipse_stats --ccl-fra-path "out_ellipse_frames/%05d.pgm"
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    mkdir out_detect_frames
    valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./bin/fmdt-detect --vid-in-start 0 --vid-in-stop 20 --vid-in-path ../2022_05_31_tauh_34_meteors.mp4 --log-path out_detect_stats --trk-bb-path out_detect_bb.txt --ccl-fra-path "out_detect_frames/%05d.pgm" --trk-all --trk-star-min 15 --trk-mag-path out_detect_mag.txt
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    if test -f "./bin/fmdt-detect-rt-seq"; then
        valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./bin/fmdt-detect-rt-seq  --vid-in-start 0 --vid-in-stop 20 --vid-in-path ../2022_05_31_tauh_34_meteors.mp4 --log-path out_detect_stats --trk-bb-path out_detect_bb.txt --ccl-fra-path "out_detect_frames/%05d.pgm" --trk-all --trk-star-min 15 --trk-mag-path out_detect_mag.txt
        rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
        valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./bin/fmdt-detect-rt-pip  --vid-in-start 0 --vid-in-stop 20 --vid-in-path ../2022_05_31_tauh_34_meteors.mp4 --log-path out_detect_stats --trk-bb-path out_detect_bb.txt --ccl-fra-path "out_detect_frames/%05d.pgm" --trk-all --trk-star-min 15 --trk-mag-path out_detect_mag.txt
        rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
        valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./bin/fmdt-detect-rt2-seq --vid-in-start 0 --vid-in-stop 20 --vid-in-path ../2022_05_31_tauh_34_meteors.mp4 --log-path out_detect_stats --trk-bb-path out_detect_bb.txt --ccl-fra-path "out_detect_frames/%05d.pgm" --trk-all --trk-star-min 15 --trk-mag-path out_detect_mag.txt
        rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
        valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./bin/fmdt-detect-rt2-pip --vid-in-start 0 --vid-in-stop 20 --vid-in-path ../2022_05_31_tauh_34_meteors.mp4 --log-path out_detect_stats --trk-bb-path out_detect_bb.txt --ccl-fra-path "out_detect_frames/%05d.pgm" --trk-all --trk-star-min 15 --trk-mag-path out_detect_mag.txt
        rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    fi
    cd ..
done
