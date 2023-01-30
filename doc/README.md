# FMDT Documentation Generation

**FMDT** uses [Doxygen](http://www.doxygen.nl/) to generate the documentation.

Install Doxygen:

```bash
sudo apt install doxygen
```

Generate the documentation:

```bash
mkdir build && mkdir build/doxygen
doxygen Doxyfile
```

This will produce `html` and `xml` documentation in `./build/doxygen/html` and 
`./build/doxygen/xml` folders, respectively.
