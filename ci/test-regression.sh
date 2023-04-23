#!/bin/bash
set -x

WD=$(pwd)
build_root=build
out_root_dir="./output"

# -----------------------------------------------------------------------------

cd scripts/regression
curl https://lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4 --output 2022_05_31_tauh_34_meteors.mp4
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

# -----------------------------------------------------------------------------
# ----------------------------------------------------------------- fmdt-detect

if test -f "${WD}/${build_root}/bin/fmdt-detect-rt-seq"; then
list_exe_detect="fmdt-detect, fmdt-detect-rt-pip, fmdt-detect-rt-seq, fmdt-detect-rt2-pip, fmdt-detect-rt2-seq"
else
list_exe_detect="fmdt-detect"
fi

curl https://lip6.fr/adrien.cassagne/data/fmdt/refs_detect_2022_05_31_tauh_34_meteors_b34a8a1f.zip --output refs.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
unzip refs.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

./compare.py --build-path ${WD}/${build_root}/bin/ \
             --list-exe "${list_exe_detect}" \
             --exe-args "--vid-in-path ${WD}/scripts/regression/2022_05_31_tauh_34_meteors.mp4 --trk-all --cca-mag --trk-bb-path ${out_root_dir}/bb.txt --ccl-fra-path ${out_root_dir}/%05d.pgm --log-path ${out_root_dir}" \
             --out-path "${out_root_dir}" \
             --refs-path ${WD}/scripts/regression/refs

rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

# -----------------------------------------------------------------------------
# ---------------------------------------------------------------- fmdt-ellipse

list_exe_ellipse="fmdt-ellipse"

curl https://lip6.fr/adrien.cassagne/data/fmdt/refs_ellipse_2022_05_31_tauh_34_meteors_9f8d6ec4.zip --output refs_ellipse.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
unzip refs_ellipse.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

./compare.py --build-path ${WD}/${build_root}/bin/ \
             --list-exe "${list_exe_ellipse}" \
             --exe-args "--vid-in-path ${WD}/scripts/regression/2022_05_31_tauh_34_meteors.mp4 --ccl-fra-path ${out_root_dir}/%05d.pgm --log-path ${out_root_dir}" \
             --out-path "${out_root_dir}" \
             --refs-path ${WD}/scripts/regression/refs_ellipse

rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

# -----------------------------------------------------------------------------

cd ${WD}
