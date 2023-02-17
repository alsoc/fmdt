# FMDT Documentation Generation

**FMDT** uses [Sphinx](http://www.sphinx-doc.org) and [Doxygen](http://www.doxygen.nl/) to generate the documentation.

Install Sphinx, Doxygen and Sphinx extensions:

```bash
sudo apt install python3 python3-pip librsvg2-bin doxygen
pip3 install --user -r requirements.txt
```

Generate the HTML documentation:

```bash
make html
```

Generate the PDF documentation:

```bash
make latexpdf
```

The last command will create a new folder: `build/html`.
In the `html` folder you just have to open the `index.html` file to read the generated documentation.

## Generating **Only** the Doxygen Documentation

Install Doxygen:

```bash
sudo apt install doxygen
```

Generate the documentation:

```bash
mkdir build
cd source
doxygen Doxyfile
```

This will produce `html` and `xml` documentation in `./build/doxygen/html` and 
`./build/doxygen/xml` folders, respectively.
