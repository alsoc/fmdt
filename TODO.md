TODO list:
- [ ] For each detection: extract meteor video sequence + - a few seconds: this 
      way if we need to re-process it we don't have to run it through the whole 
      video again
- [ ] Put a saturation flag for each detected object: meteor and star -> this 
      will help for photometry 
- [ ] Add CI tests with `fmdt-check` 
- [x] Support more image input formats
- [x] Add `--video-loop` and `--video-buff` support to the video module (based 
      on `ffmpeg-io`)
- [x] Extrapolate more than one frame in tracking
- [x] Add memory check tests in the CI (valgrind --leak-check=full 
      --show-leak-kinds=all)
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