Underworld 1
============

Underworld1 is a parallel, particle-in-cell finite element code for large-scale geodynamics simulations.

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1445812.svg)](https://doi.org/10.5281/zenodo.1445812)

If you use our code, please cite the following paper:

Moresi, L., Moresi, L. N., S. Quenette, V. Lemiale, C. Mériaux, B. Appelbe, W. Appelbe, H. B. Muhlhaus, Mühlhaus (2007), Computational approaches to studying non-linear dynamics of the crust and mantle, Physics of the Earth and Planetary Interiors, 163(1-4), 69–82, doi:10.1016/j.pepi.2007.06.009.

Underworld1 is no longer in development. We are still happy to provide support, but encourage users to migrate to [_Underworld 2_](http://www.underworldcode.org).

The Underworld development team is based in Melbourne, Australia at the University of Melbourne and at Monash University led by Louis Moresi. We would like to acknowledge AuScope Simulation, Analysis and Modelling for providing long term funding which has made the project possible. Additional funding for specific improvements and additional functionality has come from the Australian Research Council (http://www.arc.gov.au). Underworld was originally developed in collaboration with the Victorian Partnership for Advanced Computing.


Dependencies
-------------
  * MPI
  * PETSc 
  * libpng
  * libhdf5
  * libXML2

Getting the code
----------------
```bash
git clone https://github.com/underworldcode/underworld1.git
```

Compilation
-----------


```bash
    $ ./configure.py
    $ ./scons.py
```

Check available configuration options using `./configure.py --help`.  
Library testing options may be found by running `./scons.py --help`.

Run tests
---------

```bash
    $ ./scons.py check
```

Documentation
-------------
The user manual may be found at Docs/Manual/Underworld.pdf
