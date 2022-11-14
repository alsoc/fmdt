#!/bin/bash
set -x

mkdir code_coverage_report || true
genhtml code_coverage_files/*_clean.info --output-directory ./code_coverage_report/
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
