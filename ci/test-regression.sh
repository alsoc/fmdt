#!/bin/bash
set -x

WD=$(pwd)
build_root=build

if test -f "${build_root}/bin/fmdt-detect-rt-seq"; then
list_exe="fmdt-detect, fmdt-detect-rt-pip, fmdt-detect-rt-seq, fmdt-detect-rt2-pip, fmdt-detect-rt2-seq"
else
list_exe="fmdt-detect"
fi

cd scripts/regression
curl https://lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4 --output 2022_05_31_tauh_34_meteors.mp4
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
curl https://lip6.fr/adrien.cassagne/data/fmdt/refs_2022_05_31_tauh_34_meteors_854f2731.zip --output refs.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
unzip refs.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
rm refs/tracks.txt
rm refs/mag.txt

./compare.py --exe-args "--vid-in-path ${WD}/scripts/regression/2022_05_31_tauh_34_meteors.mp4 --trk-all" --list-exe "${list_exe}" --refs-path ${WD}/scripts/regression/refs
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

cd ${WD}
