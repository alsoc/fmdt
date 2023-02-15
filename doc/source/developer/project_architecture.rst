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
underscore (``_kNN_match``) requires only buffers of native types (``float``,
``uint32_t``, ``size_t`` and ``int``) while the other function (``kNN_match``)
requires structure types (``kNN_data_t``, ``RoIs_basic_t`` and ``RoIs_asso_t``).
In the implementation, the ``kNN_match`` function simply call the ``_kNN_match``
function.

Compute functions often use inner data. This data is NOT input or output data.
This is data required to store intermediate results during the computation.
They are different ways to manage this type of data in C codes. In |FMDT| the
chosen pattern is to allocate this inner data before calling the compute
function. And to deallocate this data after. For instance, in the previous
``kNN_match`` function, the first parameter is a pointer of ``kNN_data_t`` type.
This data can be allocated with the ``kNN_alloc_and_init_data`` function defined
in the same ``kNN_compute.h`` header.

The following lines illustrate how to properly use the |k-NN| module:

.. code-block:: c

	// inner data allocation on the heap (+ initialization)
	kNN_data_t* kNN_data = kNN_alloc_and_init_data(MAX_SIZE);
	// kNN matching computation (multiple calls of kNN match function with the same `kNN_data`)
	kNN_match(kNN_data, /* ... */);
	kNN_match(kNN_data, /* ... */);
	kNN_match(kNN_data, /* ... */);
	kNN_match(kNN_data, /* ... */);
	// inner data deallocation
	kNN_free_data(kNN_data);

