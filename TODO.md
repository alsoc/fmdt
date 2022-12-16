TODO list:
- [ ] Add `--video-loop` and `--video-buff` support to the video module (based 
      on `ffmpeg-io`)
- [ ] Extrapolate more than one frame in tracking
- [ ] Add CI tests with `fmdt-check` 
- [x] Compute velocity of moving ROIs, add this to the statistics (no need to do 
      this, this is the same thing as ROI error when is_moving = 1)
- [x] Tests column by column in the python script (new columns should not result 
      in a failure test)
- [x] Use dynamic vector to store the tracks
- [x] Enable read from images in `fmdt-visu`
- [x] Document `--out-mag` output text file (objects magnitudes)
- [x] Add a parameter to select the number of threads to use in `ffmpeg-io`
- [x] Improve magnitudes management in the tracking (keep array sizes per 
      objects + memory reallocations => like in C++ `std::vector`. What about
      creating a `vector` module in C for this? It will also be useful for 
      bounding boxes)
- [x] Add classification reason (from meteor to noise) in the tracks output
- [x] Implement `features_compute_magnitude` with FMDT multi-threaded runtime