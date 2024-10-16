#!/bin/bash
set -x

frame_dir="out_frames"
vid_path="../2022_05_31_tauh_34_meteors.mp4"
log_path="out_logs"

list_of_cmds=(
    "fmdt-ellipse            --vid-in-start 0 --vid-in-stop 20 --vid-in-path $vid_path --log-path $log_path --ccl-fra-path '$frame_dir/%05d.pgm'"
    "fmdt-detect             --vid-in-start 0 --vid-in-stop 20 --vid-in-path $vid_path --log-path $log_path --ccl-fra-path '$frame_dir/%05d.pgm' --trk-all --trk-star-min 15 --cca-mag"
    "fmdt-detect-no-fail     --vid-in-start 0 --vid-in-stop 20 --vid-in-path $vid_path --log-path $log_path --ccl-fra-path '$frame_dir/%05d.pgm' --trk-all --trk-star-min 15 --cca-mag"
    "fmdt-detect-opt         --vid-in-start 0 --vid-in-stop 20 --vid-in-path $vid_path --log-path $log_path --ccl-fra-path '$frame_dir/%05d.pgm' --trk-all --trk-star-min 15 --cca-mag"
    "fmdt-detect-opt-no-fail --vid-in-start 0 --vid-in-stop 20 --vid-in-path $vid_path --log-path $log_path --ccl-fra-path '$frame_dir/%05d.pgm' --trk-all --trk-star-min 15 --cca-mag"
    "fmdt-detect-rt-seq      --vid-in-start 0 --vid-in-stop 20 --vid-in-path $vid_path --log-path $log_path --ccl-fra-path '$frame_dir/%05d.pgm' --trk-all --trk-star-min 15 --cca-mag"
    "fmdt-detect-rt-pip      --vid-in-start 0 --vid-in-stop 20 --vid-in-path $vid_path --log-path $log_path --ccl-fra-path '$frame_dir/%05d.pgm' --trk-all --trk-star-min 15 --cca-mag"
    "fmdt-detect-rt2-seq     --vid-in-start 0 --vid-in-stop 20 --vid-in-path $vid_path --log-path $log_path --ccl-fra-path '$frame_dir/%05d.pgm' --trk-all --trk-star-min 15 --cca-mag"
    "fmdt-detect-rt2-pip     --vid-in-start 0 --vid-in-stop 20 --vid-in-path $vid_path --log-path $log_path --ccl-fra-path '$frame_dir/%05d.pgm' --trk-all --trk-star-min 15 --cca-mag"
    "fmdt-detect-rt-opt-seq  --vid-in-start 0 --vid-in-stop 20 --vid-in-path $vid_path --log-path $log_path --ccl-fra-path '$frame_dir/%05d.pgm' --trk-all --trk-star-min 15 --cca-mag"
    "fmdt-detect-rt-opt-pip  --vid-in-start 0 --vid-in-stop 20 --vid-in-path $vid_path --log-path $log_path --ccl-fra-path '$frame_dir/%05d.pgm' --trk-all --trk-star-min 15 --cca-mag"
)

wget https://perso.lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4 .

for build_name in "$@"
do
    cd $build_name
    mkdir $frame_dir
    for cmd in "${list_of_cmds[@]}"; do
        bin_name=$(echo $cmd | tr -s ' ' | cut -d ' ' -f 1)
        if test -f "./bin/${bin_name}"; then
            valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./bin/$cmd
            rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
        fi
    done
    cd ..
done
