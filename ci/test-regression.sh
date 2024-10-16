#!/bin/bash
set -x

WD=$(pwd)
build_root=build
out_root_dir="./output"

# -----------------------------------------------------------------------------

cd scripts/regression
# curl https://perso.lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4 --output 2022_05_31_tauh_34_meteors.mp4
wget https://perso.lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4 -O 2022_05_31_tauh_34_meteors.mp4
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

# -----------------------------------------------------------------------------
# ----------------------------------------------------------------- fmdt-detect

if test -f "${WD}/${build_root}/bin/fmdt-detect-rt-seq"; then
    list_exe_detect="fmdt-detect, \
                     fmdt-detect-no-fail, \
                     fmdt-detect-opt, \
                     fmdt-detect-opt-no-fail, \
                     fmdt-detect-rt-pip, \
                     fmdt-detect-rt-seq, \
                     fmdt-detect-rt2-pip, \
                     fmdt-detect-rt2-seq, \
                     fmdt-detect-rt-opt-seq, \
                     fmdt-detect-rt-opt-pip"
else
    list_exe_detect="fmdt-detect, \
                     fmdt-detect-no-fail, \
                     fmdt-detect-opt, \
                     fmdt-detect-opt-no-fail"
fi

# curl https://perso.lip6.fr/adrien.cassagne/data/fmdt/refs_detect_2022_05_31_tauh_34_meteors_3da41bc5.zip --output refs_detect.zip
wget https://perso.lip6.fr/adrien.cassagne/data/fmdt/refs_detect_2022_05_31_tauh_34_meteors_3da41bc5.zip -O refs_detect.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
unzip refs_detect.zip
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

./compare.py --build-path ${WD}/${build_root}/bin/ \
             --list-exe "${list_exe_detect}" \
             --exe-args "--vid-in-path ${WD}/scripts/regression/2022_05_31_tauh_34_meteors.mp4 --ccl-impl \"LSLH\" --trk-all --cca-mag --cca-ell --ccl-fra-path ${out_root_dir}/%05d.pgm --log-path ${out_root_dir}" \
             --out-path "${out_root_dir}" \
             --refs-path ${WD}/scripts/regression/refs_detect
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

if [ "$LSLM" == "ON" ]
then
    ./compare.py --build-path ${WD}/${build_root}/bin/ \
                 --list-exe "${list_exe_detect}" \
                 --exe-args "--vid-in-path ${WD}/scripts/regression/2022_05_31_tauh_34_meteors.mp4 --ccl-impl \"LSLM\" --trk-all --cca-mag --cca-ell --ccl-fra-path ${out_root_dir}/%05d.pgm --log-path ${out_root_dir}" \
                 --out-path "${out_root_dir}" \
                 --refs-path ${WD}/scripts/regression/refs_detect
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
fi

# -----------------------------------------------------------------------------
# ---------------------------------------------------------------- fmdt-ellipse

list_exe_ellipse="fmdt-ellipse"

# curl https://perso.lip6.fr/adrien.cassagne/data/fmdt/refs_ellipse_2022_05_31_tauh_34_meteors_3da41bc5.zip --output refs_ellipse.zip
wget https://perso.lip6.fr/adrien.cassagne/data/fmdt/refs_ellipse_2022_05_31_tauh_34_meteors_3da41bc5.zip -O refs_ellipse.zip
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
