.. _user_installation_guide:

******************
Installation Guide
******************

Dependencies
""""""""""""

This project uses ``ffmpeg-io``, ``nrc2``, ``c-vector`` and ``aff3ct-core``
projects as Git submodules, **you need to download them with the following
command**:

.. code-block:: bash

	git submodule update --init --recursive


.. note:: ``ffmpeg-io`` requires the ``ffmpeg`` executable: **you need to
          install ffmpeg on your system** if you want to be able to read video
          files. In addition, if you want to enable text indications in
          generated videos/images, the ``OpenCV`` library is required.

On Debian like systems you can easily install these packages with the ``apt``
package manager:

.. code-block:: bash

	sudo apt install ffmpeg libopencv-dev


On macOS, we recommend you to use the ``homebrew`` package manager:

.. code-block:: bash

	brew install ffmpeg opencv

Compilation with CMake
""""""""""""""""""""""

.. _CMake: https://cmake.org/

This project uses `CMake`_ in order to generate any type of projects (Makefile,
Visual Studio, Eclipse, CLion, XCode, etc.). The code can easily be compiled
with the following command lines:

.. code-block:: bash

	mkdir build
	cd build
	cmake ..
	make -j4

.. note:: the previous CMake command (``cmake ..``) will generate a Makefile
          without any compiler flag.

If you are using a GNU or Clang compiler like, **it is advised to use the
following CMake command line** instead:

.. code-block:: bash

	cmake .. -DFMDT_OPENCV_LINK=ON -DFMDT_AFF3CT_RUNTIME=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O3 -g" -DCMAKE_CXX_FLAGS="-Wall -funroll-loops -fstrict-aliasing -march=native"

.. note::  On Apple Silicon M1 CPUs and with Apple Clang, use ``-mcpu=apple-m1``
           instead of ``-march=native``.

The previous command line generates a Makefile in **release mode** (with
debug information ``-g``). It will produce optimized and ready for debug
binaries. Moreover, OpenCV and AFF3CT libraries will be used during the
compilation. It enables advanced features
(see the following :ref:`user_installation_cmake` section for more details about
it).

.. _user_installation_cmake:

CMake Options
-------------

+-------------------------------+---------+---------+---------------------------------+
| Option                        | Type    | Default | Description                     |
+===============================+=========+=========+=================================+
| ``FMDT_DETECT_EXE``           | BOOLEAN | ON      | |cmake-opt-detect-exe|          |
+-------------------------------+---------+---------+---------------------------------+
| ``FMDT_VISU_EXE``             | BOOLEAN | ON      | |cmake-opt-visu-exe|            |
+-------------------------------+---------+---------+---------------------------------+
| ``FMDT_CHECK_EXE``            | BOOLEAN | ON      | |cmake-opt-check-exe|           |
+-------------------------------+---------+---------+---------------------------------+
| ``FMDT_MAXRED_EXE``           | BOOLEAN | ON      | |cmake-opt-maxred-exe|          |
+-------------------------------+---------+---------+---------------------------------+
| ``FMDT_DEBUG``                | BOOLEAN | OFF     | |cmake-opt-debug|               |
+-------------------------------+---------+---------+---------------------------------+
| ``FMDT_OPENCV_LINK``          | BOOLEAN | OFF     | |cmake-opt-opencv-link|         |
+-------------------------------+---------+---------+---------------------------------+
| ``FMDT_AFF3CT_RUNTIME``       | BOOLEAN | OFF     | |cmake-opt-aff3ct-runtime|      |
+-------------------------------+---------+---------+---------------------------------+

.. |cmake-opt-detect-exe| replace:: Compile the detection chain executable.
.. |cmake-opt-visu-exe| replace:: Compile the tracking visualization executable.
.. |cmake-opt-check-exe| replace:: Compile the check executable..
.. |cmake-opt-maxred-exe| replace:: Compile the max reduction executable..
.. |cmake-opt-debug| replace:: Build the project using debugging prints: these
    additional prints will be output on ``stderr`` and prefixed by ``(DBG)``.
.. |cmake-opt-opencv-link| replace:: Link with OpenCV library (required to
    enable some options for better visualization in ``fmdt-xxx`` executables).
.. |cmake-opt-aff3ct-runtime| replace:: Link with |AFF3CT| runtime and produce
    multi-threaded detection executable (``fmdt-detect-rt``).