TODO list:
- [ ] Improve magnitudes management in the tracking (keep array sizes per 
      objects + memory reallocations => like in C++ `std::vector`. What about
      creating a `vector` module in C for this? It will also be useful for 
      bounding boxes)
- [ ] Document `--out-mag` output text file (objects magnitudes)
- [ ] Add CI tests with `fmdt-check` 
- [ ] Tests column by column in the python script (new columns should not result 
      in a failure test)
- [x] Add classification reason (from meteor to noise) in the tracks output
- [x] Implement `features_compute_magnitude` with FMDT multi-threaded runtime