.. _user_examples_use:

***************
Examples of use
***************

Download a video sequence containing meteors here:
https://lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4.
These video sequence comes from IMCCE (*l'Observatoire de Paris*) and is the
result of an airborne observation of the 2022 τ-Herculids. More information
about the 2022 τ-Herculids is available here:
https://www.imcce.fr/recherche/campagnes-observations/meteors/2022the.

Step 1: Meteors detection
"""""""""""""""""""""""""

```shell
./exe/fmdt-detect --vid-in-path ./2022_05_31_tauh_34_meteors.mp4
```

Write tracks and bounding boxes into text files for `fmdt-visu` and
`fmdt-check`:

```shell
./exe/fmdt-detect --vid-in-path ./2022_05_31_tauh_34_meteors.mp4 --trk-bb-path ./out_detect_bb.txt > ./out_detect_tracks.txt
```

Step 2: Visualization
"""""""""""""""""""""

Visualization **WITHOUT** ground truth:

```shell
./exe/fmdt-visu --vid-in-path ./2022_05_31_tauh_34_meteors.mp4 --trk-path ./out_detect_tracks.txt --trk-bb-path ./out_detect_bb.txt --vid-out-path out_visu.mp4
```

Visualization **WITH** ground truth:

```shell
./exe/fmdt-visu --vid-in-path ./2022_05_31_tauh_34_meteors.mp4 --trk-path ./out_detect_tracks.txt --trk-bb-path ./out_detect_bb.txt --gt-path ../validation/2022_05_31_tauh_34_meteors.txt --vid-out-path out_visu.mp4
```

Step 3: Offline checking
""""""""""""""""""""""""

Use `fmdt-check` with the following arguments:

```shell
./exe/fmdt-check --trk-path ./out_detect_tracks.txt --gt-path ../validation/2022_05_31_tauh_34_meteors.txt
```

Step 4: Max reduction
"""""""""""""""""""""

Use `fmdt-maxred` with the following arguments:

```shell
./exe/fmdt-maxred --vid-in-path ./2022_05_31_tauh_34_meteors.mp4 --fra-out-path out_maxred.pgm
```
