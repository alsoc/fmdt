#!/bin/bash
set -x

WD=$(pwd)
build_root=build

cd scripts/regression
curl https://lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4 --output 2022_05_31_tauh_34_meteors.mp4
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
curl https://lip6.fr/adrien.cassagne/data/fmdt/refs_2022_05_31_tauh_34_meteors_c5838f09.zip --output refs.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
unzip refs.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
rm refs/tracks.txt
rm refs/mag.txt

./compare.py --exe-args "--in-video ${WD}/scripts/regression/2022_05_31_tauh_34_meteors.mp4 --track-all" --list-exe "fmdt-detect, fmdt-detect-rt-pip, fmdt-detect-rt-seq" --refs-path ${WD}/scripts/regression/refs
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

cd ${WD}

mkdir code_coverage_files || true
for Exe in 'fmdt-common-obj' 'fmdt-detect-common-obj' 'fmdt-detect-rt-common-obj' 'fmdt-detect-exe' 'fmdt-detect-rt-pip-exe' 'fmdt-detect-rt-seq-exe'; do
    lcov --capture --directory $build_root/CMakeFiles/${Exe}.dir/src --output-file code_coverage_files/${Exe}.info
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    cp code_coverage_files/${Exe}.info code_coverage_files/${Exe}2.info
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    sed -i -e "s#${WD}/#\./#g" code_coverage_files/${Exe}2.info
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    lcov --remove code_coverage_files/${Exe}2.info "*/usr*" "*lib/*" --output-file ${WD}/code_coverage_files/${Exe}_clean.info
done
