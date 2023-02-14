.. _user_introduction:

************
Introduction
************

Purpose
"""""""

|FMDT| is derived from a software which was **designed to detect meteors** on
board |ISS| or a |CubeSat|. |FMDT| is foreseen to be applied to airborne camera
systems, e.g. in atmospheric balloons or aircraft.
**It is robust to camera movements by a motion compensation algorithm.**

**FMDT is ready for real-time processing on small boards like Raspberry Pi 4**
or Nvidia Jetson Nano for embedded systems. For instance, on the Raspberry Pi 4
(@ 1.5 GHz), |FMDT| is able to compute **30 frames per second** on a |HD| video
sequence while the instant power is only **around 4 Watts**.

.. _fig_detection_image:

.. figure:: ../../pics/2022_tauh_00183.jpg
   :figwidth: 100 %
   :align: center

   Exemple of meteors detection and visualization.

:numref:`fig_detection_chain` shows an example of detection on one frame. Green
|BBs| represent detected *meteors*, purple |BBs| represent detected *stars* and
orange |BBs| represent detected *noise* (= something which is not a *meteors*
and not a *star*).

Scientific Background
"""""""""""""""""""""

.. _fig_detection_chain:

.. figure:: ../../figs/detection_chain.svg
   :figwidth: 100 %
   :align: center

   The detection chain.

:numref:`fig_detection_chain` presents the whole |FMDT|'s detection chain. For
each pair of images, **an intensity hysteresis threshold**, **a connected
component labeling and an analysis algorithm** are applied to get a list of
|CCs| with their bounding boxes and surface. Moreover, it also provides the
first raw moments to compute the centroid :math:`(x_G,y_G)=(S_x/S,S_y/S)` of
each blob of pixels. **A morphological threshold** is then done on the surface
:math:`S` to reject small and big |CCs|. **A** |k-NN| **matching** is applied to
extract pairs of |CCs| from image :math:`I_{t+0}` and :math:`I_{t+1}` with
:math:`t` the image number in the video sequence. These matches are used to
perform **a first global motion estimation** (rigid registration). Note that
|CCs| are sometimes refered as |RoIs| in this documentation.

This motion estimation is used to classify the |CCs| into two classes - still
stars or moving meteors according to the following criterion:
:math:`|e_k-\bar{e_t}| > \sigma_t` with :math:`e_k` the compensation error of
the |CC| number :math:`k`, :math:`\bar{e_t}`
the average error of compensation of all |CCs| of image :math:`I_t` and
:math:`\sigma_t` the standard deviation of the error. **A second motion
estimation** is done with only star |CCs|, to get a more accurate motion
estimation and a more robust classification. Finally **a piece-wise tracking**
is done by extending the :math:`(t+0,t+1)` matching with :math:`(t+1,t+2)`
matching (and so on) to reduce the amount of false positive detection.

Scientific Results
""""""""""""""""""

|IMCCE| astronomers (from Paris's Observatory) led an airborne observation
campaign of the 2022 :math:`\tau`-Herculids. The 2022 :math:`\tau`-Herculids
mission is `detailed here
<https://www.imcce.fr/recherche/campagnes-observations/meteors/2022the>`_.
The data collected by the mission have been processed with |FMDT|. The detection
results helped the astronomers to see more meteors than their first "manual"
detection (by human eyes). From 28 to 34 meteors thanks to |FMDT| automated
detection. Detailed results are available in an article published in the
*Astronomy & Astrophysics* journal :cite:`Vaubaillon2023`.

Some preliminary results about the parallel implementation of the detection
chain (see :numref:`fig_detection_chain`) have been presented in a poster
:cite:`Kandeepan2022` of the workshop |AFF3CT|. The poster shows results in
terms of throughput (|FPS|), latency and energy consumption. The selected
hardware targets match embedded systems constraints (e.g.
:math:`\mathcal{T} \ge 30` |FPS| and :math:`\mathcal{P} \leq 10` Watts).

References
""""""""""

.. bibliography:: ../refs.bib
