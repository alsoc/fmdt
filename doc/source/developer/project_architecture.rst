.. _developer_project_architecture:

********************
Project Architecture
********************

First of all, this is mainly a project written in C language. There are some
exceptions with some part of the code written in C++ but the C++ code is not
mandatory and the project can always compile with a C compiler.

Thus, this projects can be seen as a pool of C structures and C functions.
The headers are located in the ``./include/c/fmdt`` folder (= structures,
enumerations, defines and functions declarations). And the implementations of
the functions are located in the ``./src/common`` folder.

Modules
"""""""

Headers (``.h`` files) and function implementations (``.c`` files) are grouped
into *modules*. A *module* is a set of headers and implementation files that
are working on the same "topic". For instance, a |k-NN| module has been
implemented is the project. It is composed of the following files:

- ``./include/c/fmdt/kNN.h``: this is a proxy header file that includes
  ``kNN_struct.h``, ``kNN_compute.h`` and ``kNN_io.h`` headers,
- ``./include/c/fmdt/kNN/kNN_struct.h``: contains structure definitions related
  to |k-NN|,
- ``./include/c/fmdt/kNN/kNN_compute.h``: declares the functions related to
  |k-NN| computations,
- ``./include/c/fmdt/kNN/kNN_io.h``: declares the functions related to
  |k-NN| inputs and outputs, in the case of the |k-NN| matching there are only
  functions to display the output results after the computations,
- ``./src/common/kNN/kNN_compute.c``: implementations of the functions declared
  in the ``kNN_compute.h`` file, plus additional private functions,
- ``./src/common/kNN/kNN_io.c``: implementations of the functions declared
  in the ``kNN_io.h`` file, plus additional private functions.

This decomposition in several files is made to have a good separation of
concerns. This way developers can easily know what to find in each file.

Executables
"""""""""""

The source code of the final executables is located in ``./src/mains/``
directory. Each file corresponds to a final executable and thus contains a
``main`` function.

Public Interfaces
"""""""""""""""""

Generally there are two levels to call a processing function. For instance, in
the |k-NN| module and in the ``kNN_compute.h`` header, the two following
functions are defined:

.. code-block:: c

	void _kNN_match(float** data_distances, uint32_t** data_nearest, uint32_t* data_conflicts, const uint32_t* RoIs0_id,
	                const uint32_t* RoIs0_S, const float* RoIs0_x, const float* RoIs0_y, uint32_t* RoIs0_next_id,
	                const size_t n_RoIs0, const uint32_t* RoIs1_id, const uint32_t* RoIs1_S, const float* RoIs1_x,
	                const float* RoIs1_y, uint32_t* RoIs1_prev_id, const size_t n_RoIs1, const int k,
	                const uint32_t max_dist, const float min_ratio_S);

.. code-block:: c

	void kNN_match(kNN_data_t* kNN_data, const RoIs_basic_t* RoIs0_basic, const RoIs_basic_t* RoIs1_basic,
	               RoIs_asso_t* RoIs0_asso, RoIs_asso_t* RoIs1_asso, const int k, const uint32_t max_dist,
	               const float min_ratio_S);

Both functions compute the |k-NN| matching. The function prefixed with an
underscore (``_kNN_match``) requires only buffers of native types (``float`` and
``uint32_t`` here) while the other function (``kNN_match``) requires structure
types (``kNN_data_t``, ``RoIs_basic_t`` and ``RoIs_asso_t``). In the
implementation, the ``kNN_match`` function simply call the ``_kNN_match``
function.

Compute functions often use inner data. This data is NOT input or output data.
This is data required to store intermediate results during the computation.
They are different ways to manage this type of data in C codes. In |FMDT| the
chosen pattern is to allocate this inner data before calling the compute
function. And to deallocate this data after. For instance, in the previous
``kNN_match`` function, the first parameter is a pointer of ``kNN_data_t`` type.
This data can be allocated with the ``kNN_alloc_data`` function defined in the
same ``kNN_compute.h`` header.

The following lines illustrate how to properly use the |k-NN| module:

.. code-block:: c

	// inner data allocation on the heap
	kNN_data_t* kNN_data = kNN_alloc_data(MAX_SIZE);
	// initialization of the data with zeros (this is NOT mandatory)
	kNN_init_data(kNN_data);
	// kNN matching computation (multiple calls of kNN match function with the same `kNN_data`)
	kNN_match(kNN_data, /* ... */);
	kNN_match(kNN_data, /* ... */);
	kNN_match(kNN_data, /* ... */);
	kNN_match(kNN_data, /* ... */);
	// inner data deallocation
	kNN_free_data(kNN_data);


Dependencies
""""""""""""

|FMDT| depends on multiple external libraries to work. The following section
details each of these libraries.

ffmpeg-io
---------

ffmpeg-io is a wrapper for the ``ffmpeg`` executable. In |FMDT|, this library
is used in the ``video`` module (to read/write videos/images).

.. note:: ffmpeg-io requires the installation of the ``ffmpeg`` executable to
          work. The library mainly exchanges data with ``ffmpeg`` through system
          pipes.

.. _developer_deps_nrc:

|NRC|
-----

|NRC| is a library dedicated to 1D and multidimensional efficient memory
allocations. This library is used everywhere data allocation are needed.

C Vector
--------

C Vector in a library that implements dynamic arrays like ``std::vector`` in
C++. This is useful when we cannot predict in advance the size of a buffer.
For instance, in |FMDT|, a C Vector is used to store the final tracks.

|AFF3CT|-core
-------------

|AFF3CT|-core is a library that includes a multi-threaded runtime. In |FMDT|,
this multi-threaded runtime is used to speed the restitution time of the
final executables. For instance, the ``./src/detect_rt.cpp`` is feature
compliant with ``./src/detect.cpp``. The main difference is that
``./src/detect_rt.cpp`` is multi-threaded with the |AFF3CT|-core library.

.. note:: |AFF3CT|-core is a C++ library. When |FMDT| is linked with
          |AFF3CT|-core, then the code requires a C++ compiler to be compiled.

|OpenCV|
--------

|OpenCV| is a famous library dedicated to a large set of computer vision
algorithms. In |FMDT|, |OpenCV| is mainly used to write text in images.

.. note:: |OpenCV| is a C++ library. When |FMDT| is linked with |OpenCV|, then
          the code requires a C++ compiler to be compiled.

