.. _uw-underworld:

**********
Underworld
**********

Introduction
============

This chapter details the essentials of Underworld modelling, namely, running the program and
modifying XML input files used to setup models. This introduces the general declarative XML file
structure. The chapter concludes with an introduction on how to run the testing scripts and basic test
models that are distributed with the code. The following chapter :ref:`uw-benchmark` introduces how
to create reproducible work flow scripts that can set up, run and analyse a model using the
CREDO benchmarking and analysis toolkit. Succeeding chapters talks about other functionalities
in Underworld like :ref:`uw-journal`, :ref:`uw-checkpoint`, :ref:`uw-timestep` and :ref:`uw-scaling`.

Running Underworld
==================

Underworld is run from the command line, executing the Underworld executable with appropriate
XML files and/or command line arguments.

For example, running the following line::

 ./Underworld-1.5/build/bin/Underworld ./InputFile/Arrhenius.xml
 --outputPath=./uw_results/exp1

will read the model declarations from the file located at *./InputFile/Arrhenius.xml*, execute the
program and put the output to directory *./uw_results/exp1*.

.. _saving-models-data:

Saving Models and Data
======================

Underworld is designed to operate in a multi-user environment, in which users can have their own
model files (input and output) located in their separate home directories, but all users execute the
same Underworld executable installed in a single shared location. This recommended setup is how
Underworld is packaged on cluster machines and in Package binaries.
Users should always keep their XML input files, and output files, in separate locations to the
source code itself. This allows users to try different version of Underworld against the same input
files, and ensures conflicts do not arise if the code is modified or removed.

For example, the Underworld directory structure could be:

 * Input files in *~/UnderworldFiles/MyInputFiles*.
 * Visualisation files in *~/UnderworldFiles/MyInputFiles/MyViewports*.
 * Output subdirectories for each model within *~/UnderworldFiles/MyOutput*.

It is also wise to create a soflink to the *Underworld* executable, to save you from always having
to point to its absolute path.

For example::

 ln -s Underworld-1.5/build/bin/Underworld ./Underworld

Running Models
==============

Assuming the directory structure is as specified in :ref:`saving-models-data`, the job can be executed
as follows::

 ./Underworld ~/UnderworldFiles/MyInputFiles/Sample.xml \
 --outputPath=~/UnderworldFiles/MyOutput/exp1 \
 -Uzawa_velSolver_ksp_type preonly -Uzawa_velSolver_pc_type lu

This executes the *Sample.xml* input file, saves the output in the directory
*~/UnderworldFiles/MyOutput/exp1*, and uses the direct solver (single processor only,
but very fast) by passing a PETSc option through Underworld to the numerical package.

For multi-processor runs, you can execute the following::

 mpiexec -np 2 ./Underworld ~/UnderworldFiles/MyInputFiles/Sample.xml \
 --outputPath=~/UnderworldFiles/MyOutput/exp1 

On a Cluster
------------

If you wish to run Underworld on an Australian supercomputer, it’s recommended to use the latest
pre-installed Underworld release module installed by the development team and cluster administrators.

There are broadly three different ways to use Underworld on computer clusters:

 * Use pre-installed Underworld release modules using a grid submission client.
 * Use pre-installed Underworld release modules via a traditional shell login. This will require
   a cluster account.
 * Install your own copy of Underworld via a traditional shell login. This will also require a
   cluster account.

For information on setting up and running Underworld on a cluster,
see the following pages: `Underworld Cluster`_, `Underworld Releases`_  and `Underworld Modules`_.

On Grisu
--------

For details on how to setup an account for Grisu on the ARCS Grid, as well as details on how to run a
job, please refer to the `Underworld Grid`_ page. This section will briefly run through some of
the work-flow differences when running Underworld on Grisu (Web-start Application Version 0.2.2).

In Grisu, the Underworld job submission template is used to submit and monitor an Underworld
job. There is no need to create a link to an Underworld executable, or to write a PBS script to
submit the job. This is all handled by the job submission template. A user may select the version
of Underworld they wish to use based on the available options on their selected cluster.

See :ref:`grisu-underworld-fig1`, :ref:`grisu-underworld-fig2` and :ref:`grisu-underworld-fig3` for screen shots of
Grisu’s Underworld job submission panels.

.. Note:: Grisu’s Underworld Job submission panel also allows you to add in command-line override
    options to the XML.

Another point of difference in Grisu is that all paths to other XML’s within your submitted
XML files MUST be relative. If the other XML files are submitted with the job.

The path reference can be:

.. code-block:: xml
 
 <include>./MyReferenceXML.xml</include>

.. _grisu-underworld-fig1:

.. figure:: _static/Grisu_BasicJobProperties.*
   :align: center

   Grisu’s Main Panel

If instead you wish to reuse files from previous jobs, you will need to specify the relative path
to those jobs. Reasons to do this include appending results to a previous job and loading a 
checkpointed job.

Now you can load all these into the current job, or you can specify the path to your previous
jobs. Each job will have its own directory which will contain all output files and input files for
that particular job. The *grisu-jobs* subdirectory, located in a users directory of their virtual
organisation’s home directory, contains all the job directories.

On Grisu, you may also want to set up a directory of *Common Files* if you have a large number
of Base XML’s. Using relative paths you will be able to reference these in submitted jobs without
having to upload them each time. You may like to set your checkpoint files to be saved in a separate
output subdirectory, to make them easier to manage.

Most output from Underworld jobs can be previewed on Grisu in realtime. These files are
downloaded to a local Grisu cache. Grisu can preview text files and image files but is unable
to preview movie files. Any output files can also be downloaded during or after a job run has
completed. All results from Grisu can be downloaded to your local machine, or an account on another
cluster.

.. _grisu-underworld-fig2:

.. figure:: _static/Grisu_JobParameters.*
   :align: center

   Grisu's Job Parameter Panel 

.. _grisu-underworld-fig3:

.. figure:: _static/Grisu_UnderworldPostProcessingPanel.*
   :align: center

   Grisu's Post Processing Panel

Using an Underworld binary package
----------------------------------

This section demonstrates how to run Underworld models using the *Underworld* executable installed
from one of the Underworld binary packages. For this example, we're using the *Underworld-1.5.2 Binary
Package for Ubuntu*. It's assumed that you've downloaded the *InputFiles* tarball from the 
`Underworld Downloads`_ page, (see :ref:`getting-underworld-inputfiles`). 

Once the *InputFiles* tarball has been untarred, you can now perform the followin operations:

 * Go to the *InputFiles* directory::

    cd ~/InputFiles/
 * The ``Underworld`` executable should be already part of the environment, so you can issue the command
   right away::

    Underworld Arrhenius.xml
   This will run the standard simulation.
 * To quit the simulation at any time white it's running, you can perform::

    <Ctrl> + c

Underworld’s input files are written in XML format and contain all the information required
to set up a simulation. In the *InputFiles* directory you will find a listing of the
available standard input files. When you run a simulation using Underworld, all the output
created from the simulation will be dumped into a directory in your file system.
The output directory is specified in the XML InputFile you run.

The default output directory is::

 ./output

The output directory will contain main files depending on what output options are enabled on the 
input file. 

The default output directory will contain the following:

 * A time-stamped copy of the input files you used, combined into one XML file (flattended).
 * A *FrequentOutput.dat* file that contains statistical information on the simulation run.
 * Output images (\*.png, \*.jpg, or \*.tiff) of each timestep.
 * A movie of the whole simulation called *window.mpeg* if you enabled movie output on the input file.

See :ref:`binary-underworld-output-image` for a sample image output produced by Underworld. 

.. _binary-underworld-output-image:

.. figure:: _static/BinaryScreenshot3.*
   :align: center

   Sample image output from running *Arrhenius.xml* in Underworld.

.. _underworld_modelling:

Modelling
=========

Underworld is designed to work using declarative programming. This is done through the XML
input files and components (components are Underworld object-oriented objects).
Most users should not need to look at the underlying C code. What this means for the user,
is that in order to create a model, you need to `declare` all the important aspects of your
model in the XML and the program handles the rest.

This design is incredibly flexible, but this can also mean that a new user can get easily lost in
the number of options. For this reason, Underworld has collections of tempalte XML files that can be
used to create models.

:ref:`xml-input-files` and :ref:`underworld-components` will run you through the basics of what
exactly input files and components are, and how to use them to create models in Underworld.
Then :ref:`xml-examples` will run through a basic tutorial on how to use and adapt the
template XML input files.

.. _xml-input-files:

XML input files
---------------

Underworld input files are XML files. It contain all the information and parameters needed to
create your model. XML files can include other XML files, and a common setup is to have a
single custom XML that calls template XML files.

Template XML files and the include command
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Underworld comes with a series of XML files that activate various aspects of a model in
isolation to one another, such as: `boundary conditions`, `numerical solver types`, 
`visualisation options`. The idea is template XML files can be swapped in and out from a
main XML file to quickly and simply modify a simulation. These template XML files are located in
several directories under the build directory of an Underworld installation. 
For example, if the Underworld installation directory is *~/myUnderworld*, the template
XML files will be located in *~/myUnderworld/build/lib/StGermain/*.

The most commonly-used XML files will be located the following paths:

 * *~/myUnderworld/build/lib/StGermain/Underworld* - 
   For general pieces of functionality.
 * *~/myUnderworld/build/lib/StGermain/Underworld/Viewports/* -
   For visualisation definitions.
 * *~/myUnderworld/build/lib/StGermain/Underworld/VariableConditions/* -
   For boundary condition and initial condition definitions.
Template XML files can be included from other XML files using the *<include></include>* tag.

For example, having the following line in your XML:

.. code-block:: xml

 <include>Underworld/Viewports/VelocityMagnitudeVP.xml</include>
will include the template file located in::
 
 ~/myUnderworld/build/lib/StGermain/Underworld/Viewports/VelocityMagnitudeVP.xml

The *<include></include>* tag in this case assumes the search path for the template file starts
at *~/myUnderworld/build/lib/StGermain/*. Relative and absolute paths can also be used in
the *<include></include>* tag.

For example:

.. code-block:: xml
 
 <include>./myViewport.xml</include>

.. _create-own-xml:

Creating your own XML file
^^^^^^^^^^^^^^^^^^^^^^^^^^

The following list outlines the required steps for a user to know  when using/creating XML files for
Underworld:

 * **Creating a blank input file** - Users should have their own XML files located in their home
   directory which links to the template XML files downloaded from the repository. This ensures
   that conflicts do not arise when running updates to Underworld and reduces the amount of work
   the user needs to produce their own working models.

   An empty XML file will require the root *<StGermainData></StGermaindata>* tag.

   For example:

   .. code-block:: xml
 
    <?xml version="1.0"?>
    <!DOCTYPE StGermainData SYSTEM "stgermain.dtd">
    <StGermainData xmlns="http://www.vpac.org/StGermain/XML_IO_Handler/Jun2003">

    ...

    </StGermainData>

 * **Including a template XML file** - Users can add aspects to their model by including
   template XML files.
   
   For example, after including the *Rayleigh-Taylor Benchmark* template XML,
   the XML should look something like this:

   .. code-block:: xml

    <?xml version="1.0"?>
    <!DOCTYPE StGermainData SYSTEM "stgermain.dtd">
    <StGermainData xmlns="http://www.vpac.org/StGermain/XML_IO_Handler/Jun2003">

      <include>$PATHTOUNDERWORLD/Underworld/InputFiles/
      RayleighTaylorBenchmark.xml</include>

    </StGermainData>

   Where *$PATHTOUNDERWORLD* is the path to the Underworld directory.

   For example:

   .. code-block:: xml 
       
    <include>/home/Me/Underworld_1.5/Underworld/InputFiles/
    RayleighTaylorBenchmark.xml</include>

   If you are on a cluster, you will be using a reference to your locally checked out input files
   directory, (see :ref:`getting-underworld-inputfiles`).

 * **Adding or Overwriting a component** - Users can change or add component parameters by simply
   entering them in the XML file. Relevant sections from a template XML file can be copied
   and its parameters changed.

   For example:

   .. code-block:: xml

    <struct name="components" mergeType="merge">
       <struct name="name in InputFile" mergeType="merge">
          <param name="parameter to overwrite" mergeType="replace">value</param>
       </struct>
    </struct>
   In most cases, the name of the *Type* matches the file name of the component.

   The example below changes the background viscosity:
   
   .. code-block:: xml

    <struct name="components" mergeType="merge">
       <struct name="backgroundViscosity" mergeType="merge">
          <param name="eta0" mergeType="replace">2.0</param>
       </struct>
    </struct>

 * **Adding or Overwriting a variable** - Users can add their own variable parameters in their XML
   files.

   The example below changes the location of the output directory:

   .. code-block:: xml

    <param name="outputPath">./outputNew/</param>

   Make sure you are NOT within the *components* struct.
   Variables are usually added towards the end of the file, after the *components* struct.

Standard XML input file format
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A typical XML input file is composed of the following sections:

 * **StGermainData tag** - This section *opens* the XML file, letting the compiler know that it is an
   XML file, and that it is a *StGermainData* XML type.

   For example:

   .. code-block:: xml

    <?xml version="1.0"?>
    <!DOCTYPE StGermainData SYSTEM "stgermain.dtd">
    <StGermainData xmlns="http://www.vpac.org/StGermain/XML_IO_Handler/Jun2003">

    ...

    </StGermainData>
   Where *...* represents entries mentioned below.
 * **A list of toolboxes to import** - The toolbox listed here will be the highest framework in
   the hierarchy that the model will use. For most users, all necessary programs will be loaded
   by default and will not need to be listed here.

   For example:

   .. code-block:: xml

    <list name="import">
       <param>Underworld</param>
    </list>
 * **Context of the model** - The user must specify the *context* of the model. This must be done
   first before other components are read into the dictionary.

   For example:

   .. code-block:: xml

    <struct name="component" mergeType="merge">
       <struct name="context">
           <param name="Type">UnderworldContext</param>
       </struct>
    </struct>
   This should be added directly after *<list name="import">...</list>* block.
 * **A list of other XML files to include** - These are the template XML files that help to define
   exactly what the model is doing. Underworld is set up to use a hierarchy of XML files so that
   your XML doesn’t need to have everything in one file.
 * **A list of plugins to include** - These plugins provides additional functionality that can be
   be included in the input file. Plugins can define what values to output. They can also modify
   how the model operates.

   For example:

   .. code-block:: xml

    <list name="plugins" mergeType="merge">
       <struct>
          <param name="Type">PluginName</param>
          <param name="Context">context</param>
          
          ...

       </struct>
    </list>
   Where *...* represents arguments to pass into the plugin. This structure requires the user
   to know what parameters to pass to a plugin. As with the *components*, parameters passed
   in are error checked and the code will halt and report a problem if invalid parameters are found.

   A complete example:

   .. code-block:: xml

    <list name="plugins" mergeType="merge">
       <struct>
          <param name="Type">StgFEM_FrequentOutput</param>
          <param name="Context">context</param>
       </struct>
       <struct>
          <param name="Type">Underworld_Vrms</param>
          <param name="Context">context</param>
          <param name="GaussSwarm">gaussSwarm</param>
          <param name="VelocityField">VelocityField</param>
       </struct>
       <struct>
          <param name="Type">StgFEM_CPUTime</param>
          <param name="Context">context</param>
       </struct>
       <struct>
          <param name="Type">lucPlugin</param>
          <param name="Context">lucPluginContext</param>
       </struct>
    </list>
   Plugins can be found inside the *StgFEM/plugins*, *PICellerator/plugins*, and *Underworld/plugins*
   directories. To include a plugin, use the plugins’ name within the params struct. If referencing
   a template XML file, your input file will inherit all the template's plugins.
 * **A list of components** - These are the values, parameters and shapes the specified model.

   For example:

   .. code-block:: xml

    <!-- Specialised components for this particular model -->
    <struct name="components" mergeType="merge">
       <struct name="shape">
          <param name="Type">Everywhere</param>
       </struct>
    </struct>
 * **Simulation control parameters** - These parameters specify variables such as: how long the model
   will run; when values are saved; whether to restart from previously checkpointed values; where
   the output should be located; how many iterations should the linear and non-linear solvers
   attempt per timestep; and other *single value* scientific parameters important to the simulation.

   For example:

   .. code-block:: xml
   
    <!-- Simulation control -->
    <param name="maxTimeSteps">10</param>
    <param name="outputPath">./output</param>
    <param name="dumpEvery">100</param>
    <param name="checkpointEvery">100</param>
   Where *maxTimeSteps* defines the maximum number of timesteps, *outputPath* defines the
   directory in which to save all data, *dumpEvery* defines the timestep increment at which data
   is outputted, and *checkpointEvery* defines how often to checkpoint data.
 * **Journal settings** - The journal controls the information that is *printed to screen* during the
   simulation. If running remotely on a HPC cluster, it is written to the standard output file.

   For example:

   .. code-block:: xml

    <!-- Control Journal output -->
    <param name="journal.info.Context-verbose">True</param>
    <param name="journal-level.info.Stg_ComponentFactory">2</param>
    <param name="journal.debug">f</param>
 * **Geometry and mesh setup** - This block outlines the physical parameters of the model: box, size,
   dimensions and resolution.

   For example:

   .. code-block:: xml

    <!-- Geometry & Mesh setup -->
    <param name="dim">2</param>
    <param name="elementResI">32</param>
    <param name="elementResJ">32</param>
    <param name="shadowDepth">1</param>
    <param name="minX">0.0</param>
    <param name="minY">0.0</param>
    <param name="maxX">1.0</param>
    <param name="maxY">1.0</param>
 * **Initial and boundary conditions** - This section will list the template XML files that define
   initial conditions *(ICs)* and boundary conditions *(BCs)* for the model. For a listing of
   available ICs and BCs see the directory *Underworld/InputFiles/VariableConditions*.

   For example:

   .. code-block:: xml

    <include>Underworld/InputFiles/VariableConditions/temperatureBCs.xml
    </include>
 * **Visualisation settings** - This section will either list the XML files that define the
   visualisation settings for the model, or will actually list the parameters and values of
   the visualisation. It is common practice to regroup all the parameters relative to a Viewport
   in another file, that will live in the *Viewports* directory (a sub-directory of the *InputFiles*
   directory where the template XML files are located). This avoids lengthy input files and allows some
   viewports settings to then be re-used in other input files.

   For example:

   .. code-block:: xml

    <include>Underworld/Viewports/TemperatureAndVelocityArrowsVP.xml</include>

   If enabling gLucifer visualisation and not including visualisation template XML files, the following
   line needs to be added into the input file:

   .. code-block:: xml

    <include>glucifer/window.xml</include>

.. _xml-examples:

XML Examples
^^^^^^^^^^^^

This section will run through a few simple Underworld models to give the user a feel for how
Underworld works. It will only focus on using and modifying input XML files. XML files used in this
section are only example input files, and some parameter alterations may be needed in order to do
specific scientific modelling jobs.

The following examples assumes that the user is working with a checkout of Underworld on a local machine.
The initial input file will be called *Example.xml* and is based on the *RayleighTaylorBenchmark.xml*
XML file as per defined in the *<include></include>* tag.

The *Example.xml* file will have the initial XML declarations:

.. code-block:: xml

 <?xml version="1.0"?>
 <!DOCTYPE StGermainData SYSTEM "stgermain.dtd">
 <StGermainData xmlns="http://www.vpac.org/StGermain/XML_IO_Handler/Jun2003">

    <include>$PATHTOUNDERWORLD/Underworld/InputFiles/RayleighTaylorBenchmark.xml
    </include>
    <list name="plugins" mergeType="merge">
       <!-- Output Plugins -->
    </list>
    <struct name="components" mergeType="merge">
   
    ...

    </struct>

    <param name="outputPath">./output/</param>

 </StGermainData>
Where *$PATHTOUNDERWORLD* is the path to the Underworld directory, (see :ref:`create-own-xml`).
The *...* inside the *components* block will be replaced by various component definitions
in the following examples:

 * **Viscous Material Example** - In this example, the entire region is assigned a single 
   viscous material.

   1. Add the shape:

      .. code-block:: xml

       <struct name="backgroundShape" mergeType="replace">
          <param name="Type">Everywhere</param>
       </struct>
   2. Set the material viscosity:

      .. code-block:: xml

       <struct name="backgroundViscosity" mergeType="replace">
          <param name="Type">MaterialViscosity</param>
          <param name="eta0">1.0</param>
       </struct>
     *MaterialViscosity* is a predefined type in Underworld, it requires a value for *eta0*.
   3. Add the material:

      .. code-block:: xml
     
       <struct name="background" mergeType="replace">
          <param name="Type">RheologyMaterial</param>
          <param name="Shape">backgroundShape</param>
          <param name="density">1.0</param>
          <param name="Rheology">backgroundViscosity</param>
       </struct>
      *RheologyMaterial* is a predefined type of material. It requires: a *Shape*, defined
      in backgroundShape); a *density*, defined as 1.0; and a *Rheology*, defined in *backgroundViscosity*.
   4. Save the resulting *Example.xml* file. These declarations will override the default values in
      *RayleighTaylorBenchmark.xml*. It will now override any other values not re-defined by the declarations
 * **Multiple Viscous Materials Example** - This example will show how to create more than one
   viscous material in a simulation. This will use the *Example.xml* From the *Viscous Material Example*.

   1. Add the shape:

      .. code-block:: xml
      
       <struct name="lightLayerShape" mergeType="replace">
          <param name="Type">Sphere</param>
          <param name="CentreX">0.5</param>
          <param name="CentreY">0.5</param>
          <param name="radius">0.10</param>
       </struct>
      This simulation will be run in 2D, so the result is a circle.
   2. Add the material:

      .. code-block:: xml
      
       <struct name="lightLayerViscosity" mergeType="replace">
          <param name="Type">MaterialViscosity</param>
          <param name="eta0">1.0</param>
       </struct>
       <struct name="lightLayer" mergeType="replace">
          <param name="Type">RheologyMaterial</param>
          <param name="Shape">lightLayerShape</param>
          <param name="density">0.1</param>
          <param name="Rheology">lightLayerViscosity</param>
       </struct>
   3. Override the shape of the original material so that it doesn't include the sphere.
      The new shape is created from the old shape minus the sphere:

      .. code-block:: xml
       
       <struct name="nonSphereShape">
          <param name="Type">Intersection</param>
          <list name="shapes">
             <param>backgroundShape</param>
             <param>!lightLayerShape</param>
          </list>
       </struct>
   4. Modify the viscous material *background* to use *nonSphereShape* as its *Shape*.

      From this:

      .. code-block:: xml
      
       <struct name="background" mergeType="replace">
          <param name="Type">RheologyMaterial</param>
          <param name="Shape">backgroundShape</param>
          <param name="density">1.0</param>
          <param name="Rheology">backgroundViscosity</param>
       </struct>
      To this:
      
      .. code-block:: xml

       <struct name="background" mergeType="replace">
          <param name="Type">RheologyMaterial</param>
          <param name="Shape">nonSphereShape</param>
          <param name="density">1.0</param>
          <param name="Rheology">backgroundViscosity</param>
       </struct>
   5. Save the resulting Example.xml file.
  See :ref:`multi-viscous-output` for an example output image from running *Example.xml*
  in Underworld.

 * **Parameters Only Example** - This last example shows how to create a *parameters only* XML file
   that can be used to override existing parameters in other XML files. This is useful for creating
   variations of a standard model and experimenting with its parameters. For this example,
   *Parameters.xml* will be created. As a starting point, it will have the same content as the base
   *Example.xml* file in :ref:`xml-examples`.

   1. Remove the *<include>...</include* line from the file. This include line is not
      required as *Parameters.xml* will be ran together with *Example.xml*.
   2. Remove the *<param name="outputPath">./output/</param>* line from the file.
      This will be re-added in the next step at a different order.
   3. Add parameters towards the end of the file and not inide the *components* block:

      .. code-block:: xml

       <!-- Simulation Parameters -->
       <param name="outputPath">./MyPath/</param>
       <param name="maxTimeSteps">60</param>
       <param name="dumpEvery">1</param>
       <param name="checkpointEvery">0</param>

       <!-- Swarm Parameters -->
       <param name="particleLayoutType">random</param>
       <param name="particlesPerCell">20</param>

       <!-- Mesh Marameters (Model Resolution)-->
       <param name="elementResI">64</param>
       <param name="elementResJ">64</param>
       <param name="elementResK">1</param>

       <!-- Model Parameters -->
       <param name="gravity">1.0</param>
       <param name="dim">2</param>

       <!-- Box Size -->
       <param name="minX">0.0</param>
       <param name="minY">0.0</param>
       <param name="minZ">0.0</param>
       <param name="maxX">1.0</param>
       <param name="maxY">1.0</param>
       <param name="maxZ">1.0</param>
   4. Modifiable parameters on each component used in the model can be included. For example,
      shape, viscosity and density parameters can be included.

      Inside the *components* struct, the parameters can be adjusted by
      changing the shape of the sphere:

      .. code-block:: xml
       
       <struct name="lightLayerShape" mergeType="merge">
          <param name="CentreX" mergeType="replace">0.5</param>
          <param name="CentreY" mergeType="replace">0.5</param>
          <param name="radius" mergeType="replace">0.30</param>
       </struct>
      Or changing the viscosities of each shape:

      .. code-block:: xml
       
       <struct name="backgroundViscosity" mergeType="merge">
          <param name="eta0" mergeType="replace">2.0</param>
       </struct>
       <struct name="lightLayerViscosity" mergeType="merge">
          <param name="eta0" mergeType="replace">2.0</param>
       </struct>
      Or changing the densities:

      .. code-block:: xml
 
       <struct name="background" mergeType="merge">
          <param name="density" mergeType="replace">2.0</param>
       </struct>
       <struct name="lightLayer" mergeType="merge">
          <param name="density" mergeType="replace">0.1</param>
       </struct>
   5. Save the resulting *Parameters.xml* file. This file can then be ran together with *Example.xml*
      in Underworld. 
   
      For example:

       ./Underworld Example.xml Parameters.xml
    
      See :ref:`example-parameter-output` for an example output image from running *Example.xml* and
      *Parameters.xml* in Underworld.

.. _multi-viscous-output:

.. figure:: _static/UnderworldTutorial03.*
   :align: center

   Sample image output from running *Example.xml* in Underworld.

.. _example-parameter-output:

.. figure:: _static/UnderworldTutorial04.*
   :align: center

   Sample image output from running *Example.xml* and *Parameters.xml* in Underworld.

.. _underworld-components:

Components
----------

Components form the major part of an Underworld model. Each template XML file creates objects of
different types, based on components. Each component, composed of one *.h* and one *.c* file,
defines a set of parameters with default values. If the values of the parameters are not redefined by
the type within the template InputFile or your own input file (which calls a template XML file),
the default values from the components will be applied when the model is run.
Components are created and linked together at run-time. Which components are created and
what properties they will have are defined within the XML input files.

Where are they located?
^^^^^^^^^^^^^^^^^^^^^^^

Components are defined throughout the code base in all levels of the hierarchy. If you are interested
in looking up the properties of an individual component, the `Underworld Documentation`_ page contains
links to the component codex for each *stable release* of Underworld. 

Component Examples
^^^^^^^^^^^^^^^^^^

This section provides some examples on the use of components in the XML files.

 * **Defining a Geometric Shape in the Model Domain** - In broad terms, the shape of an object to be
   modelled must be defined prior to its material properties e.g. density, rheology. Users can choose
   from a series of preset shapes or build their own based on those presets.

   For example:

   .. code-block:: xml

    <struct name="backgroundShape" mergeType="replace">
       <param name="Type">Everywhere</param>
    </struct>
   The component codex has detailed examples of the XML entries for the different available shapes.
   Users can look at the component *Stg_Shape* in the StGermain component codex.
 * **Union** - Shapes can be combined using a *Union*. A new shape called *shapeUnion*
   will be created by combining *mainShape* and *extraShape*.

   For example:

   .. code-block:: xml

    <struct name="shapeUnion">
       <param name="Type">Union</param>
       <list name="shapes">
          <param>mainShape</param>
          <param>extraShape</param>
       </list>
    </struct>
   Shapes being combined in a union must have already been definea. It could be defined above the 
   actual union or in an included XML file.
 * **Intersection** - A void can be created where two shapes intersect. A new shape
   *intersectionShape* which includes *mainShape* is created but excludes the region
   defined by *voidShape*.
   
   For example:

   .. code-block:: xml
   
    <struct name="shapeIntersection">
       <param name="Type">Intersection</param>
       <list name="shapes">
          <param>mainShape</param>
          <param>!voidShape</param>
       </list>
    </struct>
   Where *!* in front of *voidShape* means *not*.
 * **Material Definition** - Once a shape has been defined, users can specify its physical properties
   such as rheology and density.

   For example:

   .. code-block:: xml
   
    <struct name="mainShapeViscosity">
       <param name="Type">MaterialViscosity</param>
       <param name="eta0">1.0</param>
    </struct>
    <struct name="materialOne">
       <param name="Type">RheologyMaterial</param>
       <param name="Shape">mainShape</param>
       <param name="density">1.0</param>
       <param name="Rheology">mainShapeViscosity</param>
    </struct>

Override Parameters from the Command Line
-----------------------------------------

Users can override all XML parameters from the command line when running the *Underworld* executable.

For example::

 ./Underworld Example.xml --outputPath=./newPath

Command line options can be used for any XML parameter in Underworld as well as for selecting
some PETSc solver options.

For example:

 * Override a parameter - *--elementResI=5*.
 * Navigate through the struct hierarchy - *--components.lightLayerShape.wavelength=1.7*.
   Dots `.` are used to navigate down through the struct hierarchy.
 * Access lists:
   
   * To override an entry - *--plugins[2].Context=context2*.
   * To add an entry - *--FieldVariablesToCheckpoint[]=PressureField.*
   Square brackets *[]* are used to select or add items into a list.

.. include:: ./uw-testing.rst
.. include:: ./uw-benchmark.rst
.. include:: ./uw-journal.rst
.. include:: ./uw-checkpoint.rst
.. include:: ./uw-timestep.rst
.. include:: ./uw-scaling.rst
.. include:: ./uw-convergence.rst
.. include:: ./uw-recovery.rst
.. include:: ./uw-initial-condition.rst
.. include:: ./uw-viscosity.rst
.. include:: ./uw-multigrid.rst
.. include:: ./uw-passive.rst

.. _Underworld Cluster: http://www.underworldproject.org/documentation/Cluster.html
.. _Underworld Releases: http://www.underworldproject.org/documentation/Releases.html
.. _Underworld Documentation: http://www.underworldproject.org/documentation.html
.. _Underworld Modules: http://www.underworldproject.org/documentation/UnderworldModules.html
.. _Underworld Downloads: http://www.underworldproject.org/downloads.html
.. _Underworld Grid: http://www.underworldproject.org/documentation/UnderworldARCSGrid.html

