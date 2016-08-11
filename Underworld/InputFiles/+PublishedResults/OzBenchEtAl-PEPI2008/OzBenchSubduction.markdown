Author: Louis Moresi  
Date: 2012-09-19
Title: Ozbench Example

# Ozbench Example

Use this setup to run the Underworld benchmark from the
Ozbench et al. [#Ozbench:2008][] paper (Figure 3b).

*Louis Moresi*  
*September 2012*

# Description 

The Ozbench benchmark is used to compare 3D subduction models. The
benchmark tests the ability to reproduce the  deformation and retreat
velocity of a single "slab" foundering in a large box of viscous
fluid.

Viscous/plastic and viscoelastic/plastic models are catered for, with
the appropriate equivalent parameter choices.  The upper surface can
be free-slip (no vertical movement) or a moving, stress-free
interface. In the former case, some plasticity in the near-surface is
needed to allow the slab to bend and detach from the surface.
Underworld example in the paper is for a slab with a viscous core and
viscoplastic upper and lower layers and  a free-slip upper boundary.

The model parameters are given in [#Ozbench:2008][] - Table 1 and Figures 1 and 3.

# Usage 

The Launch-OzBenchSubduction.sh script is a template for running the model.
It is intended to be copied somewhere and then modified.
It is necessary to edit the script to update the following:

  - `UWPATH`  which points to the installation of Underworld
  - `PROJECTPATH` which points to the location of the OzBenchSubduction.xml file 
    (so you can leave it where it is and not have to copy everything)
  - `OUTPUTPATH` is where you want the output to land 
  - `EXPTNAME` is a name for this particular experiment in case you decide to
    run different models using the same template script. 
  - `openmpirun -np 3` this is specific to running with openmpi on a single 
    image machine. You will have to check if you are running on a supercomputer
    how to launch under their queueing system. This varies so much that it is
    difficult to give more comprehensive guidance. 

Underworld (and PETSc) allows you to over-ride most parameters on the command
line at run time. Some examples relevant to this problem are given here. The 
PETSc parameters have a single hyphen, the Underworld ones have a double hyphen.
PETSc also allows you to gather command line parameters into an options file,
but Underworld allows you to concatenate multiple xml files which serves the
same purpose. See the documentation for details. 

# Notes 

There is another version of the `Launch... .sh` script which utilizes
the pre-release `Solvers` package by Louis Moresi, Mirko Velic and Dave May.
This is included to for regression testing purposes once this
package is released.



[#Ozbench:2008]:  OzBench, M.; Regenauer-Lieb, K.; Stegman, D. R.;
                  Morra, G.; Farrington, R.; Hale, A.; May, D. A.; Freeman, J.;
			      Bourgouin, L.; Mühlhaus, H. & Moresi, L. A model comparison 
			      study of large-scale mantle-lithosphere dynamics driven by
			      subduction. Physics of the Earth and Planetary Interiors,
			      2008, 171, 224-234. 
			      [(Science Direct)](http://www.sciencedirect.com/science/article/pii/S0031920108002318)

