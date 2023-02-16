.. _developer_coding_conventions:

******************
Coding Conventions
******************

Start reading our code and you'll get the hang of it. For the readability, we
apply some coding conventions detailed in the following sections.

This is open source software. Consider the people who will read your code, and
make it look nice for them. It's sort of like driving a car: Perhaps you love
doing donuts when you're alone, but with passengers the goal is to make the ride
as smooth as possible.

General
"""""""

- This is an English code (functions/variables/defines/comments/... should be
  written in English).
- Indentation is made by using spaces (4 spaces).
- ALWAYS put spaces after list items and method parameters (``[1, 2, 3]``,
  not ``[1,2,3]``), around operators (``x += 1``, not ``x+=1``), and around
  hash arrows.
- The `snake case <https://en.wikipedia.org/wiki/Snake_case>`_ is used,
  (``my_variable``, not ``myVariable``), classes start with an upper case
  (``My_class``, not ``my_class``) and variables/methods/functions start with a
  lower case.
- The number of characters is limited to 120 per line of code.

Variables
"""""""""

- Global variables are prefixed with ``g_``.
- Parameter variables from the command line are prefixed with ``p_``.
- Static variables from defines are all uppercase
  (ex.: ``#define MY_STATIC_VAR 12``).
- If a variable contains more that one element, its name should ends with a "s"
  (ex.: ``int values[100]``).

Functions
"""""""""

- Function name starts with the corresponding file name (for instance, if you
  are in the ``motion.c`` file and you want to write a function that compute
  the motion, the function name will be ``motion_compute``).
- Function name should contain a verb.
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

- Structure name is always post-fixed with ``_t`` (ex.: ``my_struct_t``).
- Enumeration name is always post-fixed with ``_e`` (ex.: ``my_enum_e``).

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
