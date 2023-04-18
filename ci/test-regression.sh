#!/bin/bash
set -x

WD=$(pwd)
build_root=build

if test -f "${build_root}/bin/fmdt-detect-rt-seq"; then
list_exe="fmdt-detect, fmdt-detect-opt, fmdt-detect-rt-seq, fmdt-detect-rt-opt-seq, fmdt-detect-rt-pip, fmdt-detect-rt-opt-pip"
else
list_exe="fmdt-detect, fmdt-detect-opt"
fi

cd scripts/regression
curl https://lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4 --output 2022_05_31_tauh_34_meteors.mp4
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
curl https://lip6.fr/adrien.cassagne/data/fmdt/refs_2022_05_31_tauh_34_meteors_b34a8a1f.zip --output refs.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
unzip refs.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
rm refs/tracks.txt
rm refs/mag.txt

./compare.py --exe-args "--vid-in-path ${WD}/scripts/regression/2022_05_31_tauh_34_meteors.mp4 --trk-all --trk-mag-path mag.txt" --list-exe "${list_exe}" --refs-path ${WD}/scripts/regression/refs
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

# test if the other implementation of the LSL gives the same results ("--ccl-impl LSLM")
./compare.py --exe-args "--vid-in-path ${WD}/scripts/regression/2022_05_31_tauh_34_meteors.mp4 --ccl-impl \"LSLM\" --trk-all" --list-exe "${list_exe}" --refs-path ${WD}/scripts/regression/refs
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

cd ${WD}
