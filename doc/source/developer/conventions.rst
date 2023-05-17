.. _developer_conventions:

***********
Conventions
***********

Start reading our code and you'll get the hang of it. For the readability, we
apply some conventions detailed in the following sections.

This is open source software. Consider the people who will read your code, and
make it look nice for them. It's sort of like driving a car: Perhaps you love
doing donuts when you're alone, but with passengers the goal is to make the ride
as smooth as possible.

Coding Conventions
^^^^^^^^^^^^^^^^^^

General
"""""""

- Indentation is made by using spaces (4 spaces).
- ALWAYS put spaces after list items and method parameters (``[1, 2, 3]``,
  not ``[1,2,3]``), around operators (``x += 1``, not ``x+=1``), and around
  hash arrows.
- The number of characters is limited to 120 per line of code.
- For data buffers, explicitly sized types from ``stdint.h`` should be preferred
  (for instance, ``int`` is NOT good and ``int32_t`` should be used instead).
- Please use unsigned integers to store data that cannot take negative values.
- Use double precision floating-points numbers ONLY when it is necessary. Most
  of the time, simple precision floating-points numbers should be enough.

Functions
"""""""""

- First parameters parenthesis is put directly after the function name
  (``motion_compute(int param)`` is valid, while ``motion_compute (int param)``
  is NOT valid).
- Parameters that are only read in the function have to be post-fixed by the
  ``const`` qualifier (ex.:
  ``void my_func(const float* read_only_data, float* write_data)``),
- Braces are directly put after the last parameters parenthesis (see the example
  below).

.. code-block:: c

    void filename_verb(int param, int long_param_name) {
        for (int i = 0; i < 12; i++) {
            printf("Hello World %d\n", i);
        }
    }

Structures and Enumerations
"""""""""""""""""""""""""""

Here are some code examples to illustrate the conventions.

.. code-block:: c

    typedef struct {
        uint32_t attr1_var;
        uint32_t attr2_var;
        uint32_t* attr3_ptr;
    } my_struct_t;

.. code-block:: c

    enum color_e { COLOR_MISC = 0,
                   COLOR_GRAY,
                   COLOR_GREEN,
                   COLOR_RED,
                   COLOR_PURPLE,
                   COLOR_ORANGE,
                   COLOR_BLUE,
                   COLOR_YELLOW,
                   N_COLORS
    };

Conditional Structures and Loops
""""""""""""""""""""""""""""""""

Here are some code examples to illustrate the conventions.

.. code-block:: c

    if (counter < 12 && is_valid) {
        // do something
    } else {
        // do something else
    }

.. code-block:: c

    switch (value) {
    case 1:
        // do something
        break;
    case 2:
        // do something
        break;
    case 3:
        // do something
        break;
    default:
        break;
    }

.. code-block:: c

    for (int i = 0; i < 12; i++) {
        // do something
    }

.. code-block:: c

    while (i < 100) {
        // do something
        i++;
    }


Source Code Auto-format
"""""""""""""""""""""""

This project mainly follow LLVM coding conventions. For coding conventions
(except for the naming) the code formatting can be automatized thanks to
the ``clang-format`` parser. At the root of the project a ``clang-format``
configuration file is provided (see the ``.clang-format`` file).

For instance, if you want to auto-format the ``src/motion.c`` file you can run
``clang-format`` from the project root as follow:

.. code-block:: bash

    clang-format -i src/motion.c

Naming Conventions
^^^^^^^^^^^^^^^^^^

General
"""""""

- This is an English code (functions/variables/defines/comments/... should be
  written in English).
- The `snake case <https://en.wikipedia.org/wiki/Snake_case>`_ is used,
  (``my_variable``, not ``myVariable``), classes start with an upper case
  (``My_class``, not ``my_class``) and variables/methods/functions start with a
  lower case.

Variables
"""""""""

- Global variables are prefixed with ``g_``.
- Parameter variables from the command line are prefixed with ``p_``.
- If a variable contains more that one element, its name should ends with a "s"
  (ex.: ``int values[100]``).
- Static variables from defines are all uppercase
  (ex.: ``#define MY_STATIC_VAR 12``).
- Defines that come from the compiler should be prefixed with ``FMDT_``.

Functions
"""""""""

- Function name starts with the corresponding module name (for instance, if you
  are in the ``motion_compute.c`` file and you want to write a function that
  compute the motion, the function name could be ``motion_compute``).
- Function name should always contains a verb.

.. code-block:: c

    void filename_verb(int param, int long_param_name) {
        for (int i = 0; i < 12; i++) {
            printf("Hello World %d\n", i);
        }
    }

Structures and Enumerations
"""""""""""""""""""""""""""

- Structure name is always post-fixed with ``_t`` (ex.: ``my_struct_t``).
- Enumeration name is always post-fixed with ``_e`` (ex.: ``my_enum_e``).
- Enumeration values are in uppercase and always start with the name of the
  enumeration (in the following example ``COLOR_``). Except for the last value
  that can be in the form ``N_*s``.

.. code-block:: c

    enum color_e { COLOR_MISC = 0,
                   COLOR_GRAY,
                   COLOR_GREEN,
                   COLOR_RED,
                   COLOR_PURPLE,
                   COLOR_ORANGE,
                   COLOR_BLUE,
                   COLOR_YELLOW,
                   N_COLORS
    };

Other Conventions
^^^^^^^^^^^^^^^^^

Images Sizes and Borders
""""""""""""""""""""""""

In |FMDT| the image sizes are given with 4 parameters:

- ``i0``: first height index in the image (included),
- ``i1``: last height index in the image (included),
- ``j0``: first width index in the image (included),
- ``j1``: last width index in the image (included).

Images data can be accessed in 2D: ``img[id_height][id_width]``.
For instance if the resolution of the image is :math:`1920 \times 1080`, then
the first pixel can be accessed like this: ``img[0][0]``
and the last one like this: ``img[1079][1919]``. In the previous example:

- ``i0 = 0``,
- ``i1 = 1079``,
- ``j0 = 0``,
- ``j1 = 1919``.

Here is an example how to loop over an image in |FMDT|:

.. code-block:: c

    for (int i = i0; i <= i1; i++)
        for (int j = j0; j <= j1; i++)
            printf("Pixel img[%d][%d] has the following val: %d\n",
                   i, j, img[i][j]);

In |FMDT|, images are allocated with the |NRC| library (see
:numref:`developer_deps_nrc`). Then images can have borders (= extra columns or
lines). The extra columns or lines on the left or on the top can be accessed
with negatives indexes. The extra columns or lines on the right or on the bottom
can be accessed with higher indexes than ``i1`` and ``j1`` values.

Objects Identifiers
"""""""""""""""""""

In |FMDT| there are mainly two different types of object: the **RoIs** (= |CCs|)
and the **tracks**. A |RoI| represents a set of connected pixels at a given time
:math:`t` while a track represents an object over the time (stars, meteors,
noise, ...).
To distinguish different objects of the same type (|RoI| or track), |FMDT| uses
unique identifiers. These identifiers are encoded by 32-bit unsigned integers
and they start from **1** (and NOT 0).
The 0 value is used to recognize uninitialized objects or to mark an object for
later deletion.
