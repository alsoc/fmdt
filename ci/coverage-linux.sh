#!/bin/bash
set -x

WD=$(pwd)
build_root=build

mkdir code_coverage_files || true
for Exe in 'fmdt-common-obj' \
           'fmdt-detect-rt-common-obj' \
           'fmdt-detect-exe' \
           'fmdt-detect-rt-pip-exe' \
           'fmdt-detect-rt-seq-exe' \
           'fmdt-detect-rt2-pip-exe' \
           'fmdt-detect-rt2-seq-exe' \
           'fmdt-detect-rt-opt-seq-exe' \
           'fmdt-detect-rt-opt-pip-exe' \
           'fmdt-ellipse-exe'; do
    lcov --capture --directory $build_root/CMakeFiles/${Exe}.dir/src --output-file code_coverage_files/${Exe}.info
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    cp code_coverage_files/${Exe}.info code_coverage_files/${Exe}2.info
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    sed -i -e "s#${WD}/#\./#g" code_coverage_files/${Exe}2.info
    rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
    lcov --remove code_coverage_files/${Exe}2.info "*/usr*" "*lib/*" "*/version.c" --output-file ${WD}/code_coverage_files/${Exe}_clean.info
done

mkdir code_coverage_report || true
genhtml code_coverage_files/*_clean.info --output-directory ./code_coverage_report/
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
