.. _user_installation_guide:

******************
Installation Guide
******************

Dependencies
""""""""""""

This project uses ``ffmpeg-io``, ``nrc2``, ``c-vector`` and ``streampu``
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

.. note:: The previous CMake command (``cmake ..``) will generate a Makefile
          without any compiler flag.

If you are using a GNU or Clang compiler like, **it is advised to use the
following CMake command line** instead:

.. code-block:: bash

	cmake .. -DFMDT_OPENCV_LINK=ON -DFMDT_SPU_RUNTIME=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O3 -g" -DCMAKE_C_FLAGS_RELWITHDEBINFO="-O3 -g" -DCMAKE_CXX_FLAGS="-Wall -funroll-loops -fstrict-aliasing -march=native" -DCMAKE_C_FLAGS="-funroll-loops -fstrict-aliasing -march=native"

.. note::  On Apple Silicon M1 CPUs and with Apple Clang, use ``-mcpu=apple-m1``
           instead of ``-march=native``.

The previous command line generates a Makefile in **release mode** (with
debug information ``-g``). It will produce optimized and ready for debug
binaries. Moreover, OpenCV and StreamPU libraries will be used during the
compilation. It enables advanced features
(see the following :ref:`user_installation_cmake` section for more details about
it).

.. _user_installation_cmake:

CMake Options
-------------

Here is the list of the CMake available options:

- ``FMDT_DETECT_EXE``

   :Type: BOOLEAN
   :Default: ON
   :Example: ``cmake .. -DFMDT_DETECT_EXE=OFF``

   Compile the detection chain executable

- ``FMDT_VISU_EXE``

   :Type: BOOLEAN
   :Default: ON
   :Example: ``cmake .. -DFMDT_VISU_EXE=OFF``

   Compile the tracking visualization executable.

- ``FMDT_CHECK_EXE``

   :Type: BOOLEAN
   :Default: ON
   :Example: ``cmake .. -DFMDT_CHECK_EXE=OFF``

   Compile the check executable.

- ``FMDT_MAXRED_EXE``

   :Type: BOOLEAN
   :Default: ON
   :Example: ``cmake .. -DFMDT_MAXRED_EXE=OFF``

   Compile the max reduction executable.

- ``FMDT_DEBUG``

   :Type: BOOLEAN
   :Default: OFF
   :Example: ``cmake .. -DFMDT_DEBUG=ON``

   Build the project using debugging prints: these additional prints will be
   output on ``stderr`` and prefixed by ``(DBG)``.

- ``FMDT_OPENCV_LINK``

   :Type: BOOLEAN
   :Default: OFF
   :Example: ``cmake .. -DFMDT_OPENCV_LINK=ON``

   Link with OpenCV library (required to enable some options for improved
   visualization in ``fmdt-xxx`` executables).

- ``FMDT_SPU_RUNTIME``

   :Type: BOOLEAN
   :Default: OFF
   :Example: ``cmake .. -DFMDT_SPU_RUNTIME=ON``

   Link with StreamPU runtime and produce multi-threaded detection executable
   (``fmdt-detect-rt``).

- ``FMDT_LSL_LINK``

   :Type: BOOLEAN
   :Default: OFF
   :Example: ``cmake .. -DFMDT_LSL_LINK=ON``

   Link with an external |CCL| library. Then the |CCL| implementation can be
   changed with the :ref:`detect_ccl-impl` parameter. **This library is not
   public yet so it may fail when enabling this option.**

- ``FMDT_USE_VCIO``

   :Type: BOOLEAN
   :Default: OFF
   :Example: ``cmake .. -DFMDT_USE_VCIO=ON``

   Link with the external ``vcodecs-io`` library. This library can be used to
   decode video files with the :ref:`detect_vid-in-dec` parameter. **This
   library is not public yet so it may fail when enabling this option.**
