# -*- coding: utf-8 -*-
#
# Configuration file for the Sphinx documentation builder.
#
# This file does only contain a selection of the most common options. For a
# full list see the documentation:
# http://www.sphinx-doc.org/en/master/config

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import subprocess
import os
import textwrap
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# This is an homemade environment variable set by the user to reduce the
# compilation time if 'SPHINX_BUILDERNAME' == 'latex'. It that case, the
# 'breathe' and 'exhale' extensions are disabled since the final LaTeX PDF do
# not include the API documentation.
buildername = str(os.getenv('SPHINX_BUILDERNAME'))

# 'read_the_docs_build' is whether we are on readthedocs.org, this line of code
# grabbed from docs.readthedocs.org
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

# this is a hack to disable the library API doc on Read the docs until time
# limit can be extended
# if (read_the_docs_build):
#    buildername = "latex"

# -- Project information -----------------------------------------------------

project = 'FMDT'
copyright = '2024, FMDT team'
author = 'FMDT team'

if os.environ.get('SKIP_VERSION', 'NO') == 'YES':
    version = "unknown_version"
    release = version
else:
    # get the FMDT version from Git
    if (read_the_docs_build):
        subprocess.call('git fetch --unshallow', shell=True)
    label = subprocess.check_output(["git", "describe"]).strip().decode(encoding='UTF-8')
    split_label = label.split("-")

    # The short X.Y version
    version = split_label[0]
    # The full version, including alpha/beta/rc tags
    release = label

# -- General configuration ---------------------------------------------------

# If your documentation needs a minimal Sphinx version, state it here.
#
# needs_sphinx = '1.0'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx_rtd_theme',
    'sphinx.ext.autodoc',
    'sphinx.ext.mathjax',
    'sphinxcontrib.bibtex',
    'sphinxcontrib.rsvgconverter',
    'sphinxcontrib.video',
    'm2r',
]

bibtex_bibfiles = ['refs.bib']
bibtex_encoding = 'utf-8-sig'

if buildername != "latex":
    extensions.append('breathe')
    extensions.append('exhale')

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# The suffix(es) of source filenames.
# You can specify multiple suffix as a list of string:
#
# source_suffix = ['.rst', '.md']
source_suffix = '.rst'

# The master toctree document.
master_doc = 'index'

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#
# This is also used if you do content translation via gettext catalogs.
# Usually you set "language" from the command line for these cases.
language = None

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

# Figures, tables and code-blocks are automatically numbered if they have a caption
numfig = True

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = None

# -- Options for HTML output -------------------------------------------------

import sphinx_rtd_theme
# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
html_theme = 'sphinx_rtd_theme'
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]

# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
# see here for description : https://sphinx-rtd-theme.readthedocs.io/en/latest/configuring.html#html-theme-options
# TODO : Why the compilation fails the first time ???? -> the second time is good.
html_theme_options = {
    'canonical_url': '', # to help search engines with duplicated versions of the doc -> TODO
    'style_external_links': False, # Add an icon next to external links.
    'display_version': True, # the version number shown at the top of the sidebar
    # Toc options
    'navigation_depth' : -1,
    'collapse_navigation': True,
    'sticky_navigation': True,
    'includehidden': False,
    'titles_only': False
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
# html_static_path = ['_static']

# Custom sidebar templates, must be a dictionary that maps document names
# to template names.
#
# The default sidebars (for documents that don't match any pattern) are
# defined by theme itself.  Builtin themes are using these templates by
# default: ``['localtoc.html', 'relations.html', 'sourcelink.html',
# 'searchbox.html']``.
#
# html_sidebars = {}
html_favicon = None
html_logo    = None

# -- Options for HTMLHelp output ---------------------------------------------

# Output file base name for HTML help builder.
htmlhelp_basename = 'FMDTdoc'

# -- Options for LaTeX output ------------------------------------------------

latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    #
    # 'papersize': 'letterpaper',

    # The font size ('10pt', '11pt' or '12pt').
    #
    # 'pointsize': '10pt',

    # Additional stuff for the LaTeX preamble.
    #
    # 'preamble': '',

    # Latex figure (float) alignment
    #
    # 'figure_align': 'htbp',

    'preamble': '\setcounter{tocdepth}{10}'
}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (master_doc, 'FMDT.tex', 'FMDT Documentation',
     'FMDT team', 'manual'),
]

# -- Options for manual page output ------------------------------------------

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    (master_doc, 'aff3ct', 'FMDT Documentation',
     [author], 1)
]

# -- Options for Texinfo output ----------------------------------------------

# Grouping the document tree into Texinfo files. List of tuples
# (source start file, target name, title, author,
#  dir menu entry, description, category)
texinfo_documents = [
    (master_doc, 'FMDT', 'FMDT Documentation',
     author, 'FMDT', 'One line description of project.',
     'Miscellaneous'),
]

# -- Options for Epub output -------------------------------------------------

# Bibliographic Dublin Core info.
epub_title = project

# The unique identifier of the text. This can be a ISBN number
# or the project homepage.
#
# epub_identifier = ''

# A unique identification for the text.
#
# epub_uid = ''

# A list of files that should not be packed into the epub file.
epub_exclude_files = ['search.html']

rst_epilog = """
.. |AFF3CT|    replace:: :abbr:`AFF3CT (A Fast Forward Error Correction Toolbox)`
.. |BB|        replace:: :abbr:`BB     (Bounding Box)`
.. |BBs|       replace:: :abbr:`BBs    (Bounding Boxes)`
.. |CI|        replace:: :abbr:`CI     (Continuous Integration)`
.. |CC|        replace:: :abbr:`CC     (Connected-Component)`
.. |CCs|       replace:: :abbr:`CCs    (Connected-Components)`
.. |CCL|       replace:: :abbr:`CCL    (Connected-Components Labeling)`
.. |CCA|       replace:: :abbr:`CCA    (Connected-Components Analysis)`
.. |CubeSat|   replace:: :abbr:`CubeSat(A class of miniaturized satellite based around a form factor consisting of 10 cm
                                        (3.9 in) cubes.)`
.. |FMDT|      replace:: :abbr:`FMDT   (Fast Meteor Detection Toolbox)`
.. |FPS|       replace:: :abbr:`FPS    (Frames Per Second)`
.. |GT|        replace:: :abbr:`GT     (Ground Truth)`
.. |HD|        replace:: :abbr:`HD     (High Definition, 1920x1080 resolution)`
.. |IMCCE|     replace:: :abbr:`IMCCE  (Institut de Mécanique Céleste et de Calcul des Éphémérides,
                                        or Institute for Celestial Mechanics and Computation of Ephemerides in
                                        English)`
.. |ISS|       replace:: :abbr:`ISS    (International Space Station)`
.. |KNN|       replace:: :abbr:`K-NN   (k-Nearest Neighbor)`
.. |kNN|       replace:: :abbr:`k-NN   (k-Nearest Neighbor)`
.. |k-NN|      replace:: :abbr:`k-NN   (k-Nearest Neighbor)`
.. |LIP6|      replace:: :abbr:`LIP6   (Laboratoire Information de Sorbonne Université, or Computer Science Laboratory
                                        of Sorbonne University in English)`
.. |MR|        replace:: :abbr:`MR     (Merge Request)`
.. |NRC|       replace:: :abbr:`NRC    (Numerical Recipes in C)`
.. |OpenCV|    replace:: :abbr:`OpenCV (Open Computer Vision library)`
.. |PR|        replace:: :abbr:`PR     (Pull Request)`
.. |RoI|       replace:: :abbr:`RoI    (Region of Interest)`
.. |RoIs|      replace:: :abbr:`RoIs   (Regions of Interest)`
.. |release|   replace:: """ + release + """
.. |version|   replace:: """ + version + """

"""

# -- Extension configuration -------------------------------------------------

# -- Configure Breathe (Developer doc from Doxygen XML files)

if buildername != "latex":

    # Uncomment the following lines to enable the Doxygen compilation
    # If we are on a Readthedocs server
    if read_the_docs_build:
         # display Doxygen version
        print("Doxygen version:")
        subprocess.call('doxygen --version', shell=True)

        # Generate the Doxygen XML files
        subprocess.call('mkdir ../build', shell=True)
        subprocess.call('mkdir ../build/doxygen', shell=True)
        subprocess.call('doxygen Doxyfile', shell=True)

    breathe_projects = { "FMDT": "../build/doxygen/xml/" }
    breathe_default_project = "FMDT"

# -- Configure Exhale (Require the previous Breathe config)

if buildername != "latex":

    if read_the_docs_build:
        # Setup the exhale extension
        exhale_args = {
            # These arguments are required
            "containmentFolder":     "./api",
            "rootFileName":          "library_root.rst",
            "rootFileTitle":         "Library API",
            "doxygenStripFromPath":  "../../include",
            # Suggested optional arguments
            "createTreeView":        True,
        }
    else:
        # Setup the exhale extension
        exhale_args = {
            # These arguments are required
            "containmentFolder":     "./api",
            "rootFileName":          "library_root.rst",
            "rootFileTitle":         "Library API",
            "doxygenStripFromPath":  "../../include",
            # Suggested optional arguments
            "createTreeView":        True,
            # TIP: if using the sphinx-bootstrap-theme, you need
            # "treeViewIsBootstrap": True,
            "exhaleExecutesDoxygen": True,
            # "verboseBuild":          True,
            "exhaleUseDoxyfile":     True,
            # "exhaleDoxygenStdin": textwrap.dedent('''
            #     INPUT      = ../../include
            #     # Using `=` instead of `+=` overrides
            #     PREDEFINED = DOXYGEN_SHOULD_SKIP_THIS="1"
            # ''')
        }

    # Tell sphinx what the primary language being documented is.
    primary_domain = 'cpp'

    # Tell sphinx what the pygments highlight language should be.
    highlight_language = 'cpp'
