TODO list:
- [ ] Use dynamic vector to store the tracks
- [ ] Add `--video-loop` and `--video-buff` support to the video module (based 
      on `ffmpeg-io`)
- [ ] Document `--out-mag` output text file (objects magnitudes)
- [ ] Add CI tests with `fmdt-check` 
- [ ] Tests column by column in the python script (new columns should not result 
      in a failure test)
- [x] Add a parameter to select the number of threads to use in `ffmpeg-io`
- [x] Improve magnitudes management in the tracking (keep array sizes per 
      objects + memory reallocations => like in C++ `std::vector`. What about
      creating a `vector` module in C for this? It will also be useful for 
      bounding boxes)
- [x] Add classification reason (from meteor to noise) in the tracks output
- [x] Implement `features_compute_magnitude` with FMDT multi-threaded runtime