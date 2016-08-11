.. _uw-visualisation:

*************
Visualisation
*************

Introduction
============
The gLucifer visualisation framework provides Underworld with a built in set of tools customised to plot, render and explore your model data with results available immediately as the simulation progresses.
As a general visualisation analysis tool kit it can be used with any grid based computational models using the StGermain framework.

Instead of doing all the visualisation and analysis stages of your workflow as a post-processing step using large amounts of data gathered from preceding simulation runs, gLucifer allows you to specify visualisation elements in the XML scripts that define your model.

During the initial construction of a model, it will likely be more useful to set drawing elements to plot large areas of data or visualising areas where it is guessed interesting features may turn up.

The visualisation can then be refined in an iterative process to produce more exacting results and hone in on areas of interest, while reducing the amount of unnecessary output data produced.

Changes in gLucifer 2
=====================
This section is only relevant if you are interested in details of the differences between the previous gLucifer version and reasoning behind the changes, if not please skip ahead.

gLucifer 2.0 provides a new visualisation system for Underworld which attempts to be completely backward compatible with previous gLucifer XML scripts and while providing an expansion of same orignal set of features it has a very different design and approach.
This backwards compatibility has been extensively tested but if you have any issues with visualising a previously working model, please let us help by posting a question in the Visualisation sub-forum of the Underworld Project forum: https://underworldproject.org/forums/viewforum.php?f=26.

The gLucifer toolbox provides a set of StGermain components to define windows, viewports, cameras, various "drawing objects" that control visualisation elements along with input and output components. 
All these components previously interacted directly with a parallel OpenGL rendering engine that could produce image and video output while the simulation ran, on machines ranging from supercomputers to multi-core workstations and single processor notebooks.

This approach had some limitations.
 
* The vast array of combinations of machine architecture, operating system and graphics processing capability that required support in the OpenGL rendering engine.
* Trade-off between supporting so many target systems: using any modern and advanced OpenGL rendering features was impossible.
* Large number of dependencies required for building Underworld.
* Inability to use transparency with parallel compositing.
* 3d model data discarded after image/video output, no viewing/exploration of the visualised data except from the predefined points of view.
* All decisions about rendering defined before the run, even the slightest visual change (eg: colours) could not be adjusted without re-running the simulation steps that needed re-visualising.
* Entanglement of areas of functionality that should be separate: data analysis processing combined with the 3d rendering of results.

In order to solve these problems, a highly modular approach has been taken with gLucifer 2.

* **Data Analysis** provides the familiar gLucifer/StGermain component toolbox interface.

  * Data persistance API - for creating 3D model data, collating the data efficiently in parallel and storing it in a database. Contains the original gLucifer 1 code.
  * No OpenGL calls, all output is replaced with calls to the Data API. All output from Underworld is to database files - images and video can be produced but this is done by calling another program (the gLucifer viewer/renderer), thus Underworld has no OpenGL, imaging or window system dependencies.
  * InputFormat, OutputFormat, Windowing, RenderingEngine and WindowInteraction components/modules are all removed and replaced by the viewer/renderer.
  * Window, Viewport and Camera components remain but simply define a structure which is written to the database and used by the renderer module.

* **Rendering and Output**

  * *Rendering library* load 3D model data from the database and producing OpenGL renderings
  * *Output* Write image & video output of the render to files.
  * *Interactive viewer* Additional features to provide a user interface to interact with and modify the model.

Both interactive and background rendering are still possible, via an intermediate database to store the visualisation data.
Background rendering is done in a separate process to render 3d vis data to image frames from the database file(s). Interactive rendering is available by loading the database into the interactive viewer.

Framework
=========
The gLucifer analysis modules are flexible and extensible parallel modules which produce can visualisation data from any
StGermain-based code as it is generated, they fall into two main categories:

 * **Analysis: Base** - Contains the foundation of the gLucifer Analysis module (Camera, Colourmap, DrawingObject, Viewport, Window) and the Data API (Database).
 * **Analysis: Drawing Objects** - The functional components of the gLucifer Analysis module. Contains implementations of the *lucDrawingObject* class which provide ways to visualise particular sets of data in specific ways. The results produced by these components are passed to the Data API to be written to the render database.

The gLucifer Viewer provides a general purpose 3d renderer and a set of cross-platform data interaction and output capabilities:

 * **Viewer: Renderer** - Contains Rendering library and the Output / Interaction modules. Input data comes from database files produced by the Data API, producing output as images, movies, or interactive exploration of the data.
 * **Viewer: Windows** - Contains platform specific code and modules for using the viewer on different target rendering platforms, off-screen: (AGLViewer - Mac only, OSMesaViewer - all except windows) and interactive: (GlutViewer - Mac/Linux, SDLViewer - all platforms including windows, X11Viewer - all except windows).

.. _gl-fw-fig-01:
.. figure:: _static/gluciferModules.*

gLucifer Build
==============
Underworld pre-packaged binaries are available from the Underworld Project website for Mac OS X and Ubuntu Linux.
gLucifer Viewer pre-packaged binary installer packages are also available for Mac OS X, Ubuntu Linux and Windows.
http://www.underworldproject.org/downloads.html

If you are using the pre-packaged binaries you will not need to build Underworld from source code so please skip this section.

The gLucifer modules are built by default with Underworld, in fact it is much simpler to get up and running than previous versions as the analysis module has no additional dependencies.

This means that gLucifer can be used for analysis on any system that runs Underworld, without requiring an OpenGL implementation or various image/video output libraries. Even the SQLite3 database component is provided as part of the library so requires no external dependency installation.

If no OpenGL implementation is available, the output will be database files only which must be run through the output or interactive viewer modules to view the results. This can easily be done on another machine where OpenGL drivers are available.

If OpenGL capability is present, two additional executables are produced with Underworld:

* **build/bin/gLucifer** the gLucifer viewer, provides various command line options to produce images and videos from a provided visualisation database file. Requires OpenGL and either X11, SDL, Glut, AGL or OSMesa for output and optionally libpng and libavcodec. If an interactive environment is available (X11,SDL,Glut) allows full user interation with the model data.

* **build/bin/gLuciferOS** A non-interactive, off-screen only version of the gLucifer viewer. Will always use OSMesa or AGL if available, otherwise is the same as the standard viewer. Called by Underworld when set to automatically produce output images.

gLucifer Renderer & Viewer Dependencies
---------------------------------------

**OpenGL capable libraries, to render images at least one of the following must be available**

* SDL - Cross-platform display library, works on all platforms when required SDL library package is installed.
* X11 - Available on Linux and other Unix based systems including Mac OS X (if installed), is usually installed on clusters but only usable without access to a display if the Xvfb (X virtual frame-buffer) is installed.
* Glut - Cross-platform display library, available on Mac OS X and Linux if installed.
* AGL - Off-screen rendering for Mac OS X only.
* OSMesa - Off-screen rendering using the Mesa software renderer, used when no display is available such as on HPC clusters.

**Additional libraries, all of which are optional**

* libPNG - allows PNG image output with optional transparent backgrounds. If not available, JPEG images will be produced with the included JPEG encoder (libJPEG is not required).
* libavcodec - video encoder, allows movie output, available for all systems often as part of the ffmpeg package.

Output files produced by the Config system
------------------------------------------
Configuring Underworld before building (running configure.py) should be a automatic process but if problems occur it can be useful to know that there are now up to 4 separate configurations saved depending on packages located:

* config.cfg - the base Underworld configuration, also used to build the gLucifer analysis and data output modules.
* output.cfg - OpenGL and image/video output capabilities available for gLucifer rendering (used to build rendering library)
* offscreen.cfg - Window output capabilities available for off-screen rendering (used to build gLuciferOS)
* viewer.cfg - Window output capabilities available for interactive/on-screen rendering (used to build gLuciferOS)

The content of the last three will define how the gLucifer Viewer and Renderer are built and what capabilities are available for rendering and producing images in the build.

Required XML Components
=======================

The list below outlines the minimum required components to be provided in the XML file to be able to display
visualisation output in gLucifer:

 * gLucifer toolbox
 * A :ref:`gl-reference-window`
 * At least one :ref:`gl-reference-viewport` with
   * A :ref:`gl-reference-camera`
   * And :ref:`gl-reference-drawing-object`\s 

These are the essential requirements for any rendering. They can be defined by hand coding XML or by including template XML files.

Quickstart
==========

First you will require the gLucifer toolbox, this is specified by adding it to the *import* list at the start of one of your XML input files.

For example:

.. code-block:: xml
 
 <list name="import">
    <param> gLucifer </param>
 </list>

The easiest way to get started is to include one of the predefined visualisation Viewport template files containing a viewport and set of drawing objects which can be included in the XML input file.

For example:

.. code-block:: xml

 <include>Underworld/Viewports/StrainRateInvariantVP.xml</include>

 <struct name="components">
    <struct name="window">
       <param name="Type">lucWindow</param>
       <param name="Viewport">StrainRateInvariantVP</param>
    </struct>
 </struct>

The :ref:`gl-reference-window` component is the basic component used to create a visualisation, it must contain at least one :ref:`gl-reference-viewport`.
By default it uses png or jpeg image output and creates a output window large enough for each viewport sized to 400x400 pixels.
*StrainRateInvariantVP.xml* contains a pre-packaged set of drawing objects, a camera, colour-map and viewport designed to visualise the strain rate invariant field.

Templates
---------

The Underworld/Viewports directory contains a series of visualisation files that can be used as templates.
Users can:
 
 * Call a template visualisation file directly from within the XML file, or
 * Call their own visualisation file which could contain modifications to a called template
   visualisation file.

Some of these are already used in the template input files, so if a template XML file is already
being included in the user input file, another visualisation template doesn't need to be included 
again (see :ref:`create-own-xml` and :ref:`xml-examples`). Any template visualisation files which
is included in Underworld can be used and be a basis customising the model's appearance and outputs.
As per other XML file parameters, parameters used in the visualisation template can also be overridden
(see :ref:`xml-examples`).

.. _gl-output:

Images and Movies
-----------------

gLucifer will render a window at each dump timestep in the calculation, specified by the parameter
*dumpEvery*:

.. code-block:: xml
 
 <element type="param" name="dumpEvery">1</element>

Each time it is rendered, the window will be saved to the database. This output can be read by the viewer to produce images or view the model interactively at any time.

There is also the option to produce images on the fly, this will incurr a slight overhead but is enabled by default for historical reasons. By default, Underworld calls the *gLuciferOS* binary to do this, so it must have been built or no images will be produced.
You can also define the $GLUCIFER_PATH environment variable to point Underworld to the renderer executable you want to use.

Images are saved in PNG format if libpng is available, otherwise the built in JPEG encoder is used (which has no support for transparent backgrounds).

To produce an images of the first 5 time steps when passed a visualisation database as a command line parameter.

   **build/bin/gLucifer output/gLucifer.gldb -w -0 -4**

Similarly, to produce a video file from a previously created database of time steps 5 to 10.

   **build/bin/gLucifer output/gLucifer.gldb -m -5 -10**

To output a movie file using gLucifer, a video encoding library must be installed when Underworld is configured (see :ref:`uw-additional-software`).

Currently only the libavcodec/libavformat (ffmpeg) library is supported. The pre-built viewer binaries have built in video encoding support.

For more details on command line options and the gLucifer Viewer/Renderer see :ref:`gl-interactive`.

.. _gl-database:

Database Output
===============

As well as exporting image and video files, gLucifer writes a database file containing the 3d model objects created during visualisation. The database component is now integral to gLucifer and is required, so if none is specified in the input XML then each window uses a default database file, named *gLucifer.gldb* in the default output directory.

If you wish to change the default options to control the database output, you'll need to manually create the database component:

.. code-block:: xml

 <struct name="components">
  <struct name="db">
    <param name="Type">lucDatabase</param>
    <param name="deleteAfter">0</param>
    <param name="writeimage">True</param>
    <param name="splitTransactions">False</param>
    <param name="transparent">False</param>
    <param name="compressed">True</param>
    <param name="singleFile">True</param>
    <param name="filename">gLucifer</param>
    <param name="vfs"></param>
    <param name="timeUnits"></param>
  </struct> 

  <struct name="window">
    <param name="Type">lucWindow</param>
    <param name="Database">db</param>
    ...
  </struct>
 </struct>

For descriptions of the database parameters see :ref:`gl-reference-database`

.. _gl-windows-and-viewports:

Windows and Viewports
=====================

Visualisation is done by opening at least one rendering window. This window can then be divided
into different viewports. The viewports are really *sub-windows* allowing display of different
objects and views of a model inside the single parent window:

.. _gl-wp-w-fig-01:

.. figure:: _static/base.*
   :align: center

The orientation of the Viewports in one window is decided by the way they are listed in the
visualisation file :ref:`gl-reference-viewport` list. The number of *param* lines determines the number of
vertical divisions of the window, the number of named viewports listed inside each param line
decides the horizontal divisions of the window.

For example:

.. code-block:: xml

 <struct name="components" mergeType="merge">
    <struct name="window">
       <param name="Type">lucWindow</param>
       <list name="Viewport">
          <param>ParticleDensityVP</param>
          <param>StrainRateInvariantVP</param>
       </list>
    </struct>
 </struct>

This will display the two viewports on top of each other in one window: Vertical viewport layout

.. figure:: _static/viewportsVertical.*

Another example:

.. code-block:: xml
 
 <struct name="components" mergeType="merge">
    <struct name="window">
       <param name="Type">lucWindow</param>
       <list name="Viewport">
          <param>ParticleDensityVP StrainRateInvariantVP</param>
       </list>
    </struct>
 </struct>

This will display the two viewports side by side: Horizontal viewport layout

.. figure:: _static/viewportsHorizontal.*

You can combine these methods too: Horizontal and Vertical viewport layout


.. code-block:: xml

 <struct name="components" mergeType="merge">
    <struct name="window">
       <param name="Type">lucWindow</param>
       <list name="Viewport">
          <param>ParticleDensityVP StrainRateInvariantVP</param>
          <param>StrainRateInvariantVP</param>
       </list>
    </struct>
 </struct>

.. figure:: _static/viewportsBoth.*

.. note:: If you don't specify the window width and height, the size of the window will be automatically calculated to allow at least 400 x 400 pixels for each viewport. If you do specify the window size the space will be divided equally between each viewport in the horizontal and vertical directions.

For more details of window and viewport parameters see :ref:`gl-reference-window` and :ref:`gl-reference-viewport`.

.. _gl-camera:

Camera Adjustment
=================

The easiest way to set the camera is supply it with a field variable to centre on and allow
the default settings for the camera to automatically zoom to fit the model in the viewport.

If zooming in or out further than the default range, the model position can be set using the
*translateX*, *translateY*, *translateZ* parameters. Adjusting the *translateZ* parameter
will give the effect of zooming in or out by moving the model closer (negative) or further away (positive).

For example:

.. code-block:: xml
 
 <struct name="camera" mergeType="replace">
    <param name="Type">lucCamera</param>
    <param name="CentreFieldVariable">PressureField</param>
    <param name="translateZ">-1.5</param>
 </struct>

Similarly the translateX parameter will move the model to the left (negative) and right
(positive) and translateY will move it up (positive) and down (positive). To rotate the model,
the following rotation parameters are available:

 * **rotateX** - rotate about X axis, tilt forward and back (*pitch*).
 * **rotateY** - rotate about Y axis, turn left and right (*yaw*).
 * **rotateZ** - rotate about Z axis, twist clockwise and anti-clockwise (*roll*).

For example:

.. code-block:: xml
 
 <struct name="camera" mergeType="replace">
    <param name="Type">lucCamera</param>
    <param name="rotateX">90</param>
 </struct>

This will rotate the model about the x-axis by 90-degrees so the model will be viewed from above.

If the simulation bounding area will change as the model runs (expanding or compressing) then setting the 
*autoZoomTimestep* parameter to a higher value than the default is a good idea. 

The default is 0 which will automatically zoom the camera once only when the model is first displayed. To automatically adjust
the zoom every timestep set it to 1.

For example:

.. code-block:: xml
 
 <struct name="camera" mergeType="replace">
    <param name="Type">lucCamera</param>
    <param name="autoZoomTimestep">1</param>
 </struct>

To disable all automatic camera zoom set autoZoomTimestep to -1.

For more details of the camera object parameters see :ref:`gl-reference-camera`.

.. _drawing-objects:

Drawing Objects
===============

The list below outlines the available Drawing Objects in gLucifer which provide means to visualise particular sets of data in specific ways.

 * **General**
 
   * *luColourBar* Draws a colour bar at the bottom of a :ref:`gl-reference-viewport` with labels showing the range of colours mapped to plotted values on the scale of a particular :ref:`gl-reference-colourmap`.

   .. figure:: _static/colourBar.*

   * *lucHistoricalSwarmTrajectory* Plot lines following the path of particles in a swarm.

   .. figure:: _static/tracers.*

   * *lucMeshViewer* Draws a grid to represent a StgFEM mesh.

   .. figure:: _static/meshViewer.*

   * *lucContour* - Visualises a scalar field by the use of contour lines at specific intervals. It gets the value of the field on a grid and uses a 2D form of the marching cubes algorithm to draw the contours.

   * *lucContourCrossSection* - Draws contour lines at specific intervals over a specified 2d
     cross-section plane.

   .. figure:: _static/contours.*

   * *lucCrossSection* - Draws a 2d cross-section plane.

   .. figure:: _static/crossSection.*

   * *lucCapture* - Capture drawing commands, for use in plugins.

   ..

   * *lucPlot* - Draw line and bar charts from data in text files.

   .. figure:: _static/plot.*

   * *lucFieldSampler* - Sample a field in a regular grid, display as points.

   .. figure:: _static/fieldSampler.*

   * *lucTextureMap* - Draws an image from a source file on disk into the rendered scene.

   .. figure:: _static/texture.*

 * **Surfaces**

   * *lucFeVariableSurface* - Plots a 3D surface showing the values of an *FeVariable* as a height map.

   .. figure:: _static/feVariableSurface.*

   * *lucScalarField* - Tiles the side walls of the model with colours derived from the value of a scalar field at the vertex of each tile.
   * *lucScalarFieldOnMesh* - As above, but plots on the mesh nodes rather than arbitrary points.

   .. figure:: _static/scalarField.*

   * *lucScalarFieldCrossSection* - Tiles a plane with colours derived from the value of a scalar field at the vertex of each tile.
   * *lucScalarFieldOnMeshCrossSection* - As above, but plots on the mesh nodes rather than arbitrary points.

   .. figure:: _static/scalarFieldCrossSection.*

   * *lucIsosurface* - Visualises a scalar field in 3D by the use of a surface over which all the values of the field are constant (an isosurface). It gets the value of the field on a 3D grid and uses the marching Cubes algorithm to construct the surface.

   .. figure:: _static/isosurface.*

   * *lucIsosurfaceCrossSection* - 2d cross section of an isosurface.

   .. figure:: _static/isosurface2d.*

 * **Particles**

   * *lucSwarmViewer* - Plots particles in a swarm as small dots.

   .. figure:: _static/swarmViewer.*

   * *lucSwarmShapes* - Plots a 3d shape for each of the particles in a swarm, can be scaled in each direction using a variable.

   .. figure:: _static/swarmShapes.*

   * *lucSwarmVectors* - Plots a vector quantity on particles in a swarm as arrows.

   .. figure:: _static/swarmVectors.*

   * *lucSwarmRGBColourViewer* - Same as *lucSwarmViewer* but allows mapping a different field to each of the red, green and blue components of the plotted particle colour.

   .. figure:: _static/swarmRGB.*


 * **Vectors**
 
   * *lucVectorArrows* - Samples the values of a vector field in a grid and draws arrows to represent the direction, scaled to represent the magnitude.

   .. figure:: _static/vectorArrows.*

   * *lucVectorArrowCrossSection* - Samples the values of a vector field in a plane and draws arrows to represent the direction, scaled to represent the magnitude.

   .. figure:: _static/vectorArrowCrossSection.*

   * *lucEigenvectors* - Displays vector arrows of eigenvector values.
   * *lucEigenvectorsCrossSection* - Displays vector arrows of eigenvector values over a specified cross section plane

   .. figure:: _static/eigenVectors.*

.. note:: The title, timestep, border and axis objects have been removed from gLucifer 2 as all these options are now available as parameters on the viewport. The components are still allowed for backwards compatibility but should not be used in new models.

For descriptions of the global drawing object parameters see :ref:`gl-reference-drawing-object`.
For descriptions of parameters for setting up colour maps for drawing objects see :ref:`gl-reference-colourmap`.

Details of parameters for individual drawing objects in the latest version of Underworld/gLucifer can be found in the component codex: http://underworldproject.org/codex-bleeding-edge/gLucifer.html

Creating 2D Slices
------------------
Cross sections of the 3D model can be taken in gLucifer and visualised as 2d slices. To take a slice of a scalar field use the *lucScalarFieldCrossSection* object and set either:

* **crossSectionX**, **crossSectionY** or **crossSectionZ** parameters to fixed coordinate values.
* **crossSection** to *x=Value*, *y=Value* and/or *z=Value*, where *Value* can be a proportion of the range in that direction (eg: x=0.5 for half of the X range) or a percentage (eg: y=75% to place at 3/4 of the way in to the Y axis range) or x/y/z=min/max to place at the minimum or maximum of the range (eg: z=min to place at back of Z range).

The **resolutionA** and **resolutionB** parameters define the sampling resolution of each of the cardinal direction of the cross-section plane. In the example below they are set to the element resolution.

The *lucSwarmViewer* object plots particles and can be set to colour the particles by the particle index number highlighting the different material areas. 
To show a slice through the particles you can limit the volume over which particles are plotted by setting **positionRange=true** and setting the **minPositionX** and **maxPositionX** and/or corresponding Y or Z positions as desired.

Examples:

.. code-block:: xml

 <!-- For a top view showing velocity mag field and velocity arrows in horizontal cross sections -->
 <struct name="velocityMagScalarFieldTop">
    <param name="Type">lucScalarFieldCrossSection</param>
    <param name="FieldVariable">VelocityMagnitudeField</param>
    <param name="ColourMap">velocityMagColourMap</param>
    <param name="crossSectionY">y=0.8</param>
 </struct>
 <struct name="velocityArrowsTop">
    <param name="Type">lucVectorArrowCrossSection</param>
    <param name="VectorVariable">VelocityField</param>
    <param name="colour">white</param>
    <param name="crossSection">y=85%</param>
    <param name="lengthScale">0.5</param>
    <param name="resolutionA">elementResI</param>
    <param name="resolutionB">elementResK</param>
 </struct>

.. code-block:: xml

 <!-- For a side view showing materials, velocity arrows and a strain rate invariant cross-section -->
 <struct name="materialParticleDots">
    <param name="Type">lucSwarmViewer</param>
    <param name="Swarm">materialSwarm</param>
    <param name="pointSize">3.0</param>
    <param name="pointSmoothing">true</param>
    <param name="ColourVariable">materialSwarm-MaterialIndex</param>
    <param name="ColourMap">materialColourMap</param>
    <param name="positionRange">true</param>
    <param name="minPositionZ">2.3</param>
    <param name="maxPositionZ">2.5</param>
 </struct>

 <struct name="velocityArrowsSide">
    <param name="Type">lucVectorArrowCrossSection</param>
    <param name="VectorVariable">VelocityField</param>
    <param name="colour">Bisque</param>
    <param name="crossSection">z=20%</param>
    <param name="resolutionA">elementResI</param>
    <param name="resolutionB">elementResJ</param>
 </struct>

 <struct name="strainRateInvSide">
    <param name="Type">lucScalarFieldCrossSection</param>
    <param name="FieldVariable">StrainRateInvariantField</param>
    <param name="ColourMap">strainRateColourMap</param>
    <param name="crossSection">z=20%</param>
 </struct>

Texture Map
-----------

A texture mapping draws an image in a rendered scene, using a .ppm, .png or .tga file. 
To insert an image into the domain, a *lucTextureMap* component must be provided.

For example:

.. code-block:: xml

 <struct name="sampleImage">
    <param name="Type">lucTextureMap</param>
    <param name="image">SampleImage.png</param>
    <param name="bottomLeftX">0.0</param>
    <param name="bottomLeftY">1.0</param>
    <param name="bottomLeftZ">4.0</param>
    <param name="bottomRightX">4.0</param>
    <param name="bottomRightY">1.0</param>
    <param name="bottomRightZ">4.0</param>
    <param name="topRightX">4.0</param>
    <param name="topRightY">1.0</param>
    <param name="topRightZ">0.0</param>
    <param name="topLeftX">0.0</param>
    <param name="topLeftY">1.0</param>
    <param name="topLeftZ">0.0</param>
 </struct>

Where *SampleImage.png* is the filename of the image to be used.

Analysis Component Parameters
=============================

Listed here are the base components of the gLucifer Analysis module with the parameters that can be set to control their behaviour.

.. _gl-reference-window:

Window
------
Create one or more as required. Each contains a visualisation: a set of views and objects that will be output as a single image or shown in a single window when loaded using the viewer. A *Window* requires a list of :ref:`gl-reference-viewport`\s and a :ref:`gl-reference-database`. It provides visualisation data to the *Database* to be collated and written to disk for later viewing or output.

* **width** Integer, sets the width of the window in pixels.
* **height** Integer, sets the height of the window in pixels.
* **backgroundColour** Colour, the background colour of the window.
* **useModelBounds** True/False, set to False to prevent the window using the default model size to set the camera, useful for Plot objects.
* **disable** True/False, set to True to prevent this window producing output.
* **Database** Name of the database component, if not provided a default is created.
* **Viewport** A list of viewport components to display in the window. (see :ref:`gl-windows-and-viewports`)

.. _gl-reference-database:

Database
--------
Only one required (but more can be used). Manages the parallel output of visualisation data for all windows and provides some options for controlling the output. Writes the 3D graphics data provided by the analysis components to an SQLite database file on disk for later viewing. Runs the gLucifer renderer for image output automatically when requested. 

* **deleteAfter** deletes records from the database after N timesteps have passed, useful if you want to reduce disk usage and don't care about re-visualising the data from the database later.
* **writeimage** calls the renderer to write images after every output timestep, disable if you will be using the database for visualisation later and don't want to waste CPU cycles rendering on the fly.
* **splitTransactions** default is off, this writes all output from each timestep in a single database transaction which is faster but requires more memory. Enable this if memory usage is a problem, will cause gLucifer to write objects out in separate transactions to the database as soon as they become available.
* **transparent** image output will have transparent background (only works if libPNG is available).
* **compressed** zlib compression of database geometry records, very slight overhead to create much smaller database files.
* **singleFile** by default all output is to a single database file, if set to False output will be split into separate files per timestep: filename.gldb + filenameXXXXX.gldb where XXXXX is the current timestep.
* **filename** base filename of database output, default is "*gLucifer*", produces outputdir/gLucifer.gldb.
* **vfs** specify an alternate virtual file system for SQLite3 to control file-locking, eg: "unix-dotfile" (may be necessary on some nfs systems).
* **timeUnits** units to use when writing time step data, eg: "s" for seconds, default is dimensionless (no units).

.. _gl-reference-viewport:

Viewport
--------
Define as many as required and include them in the :ref:`gl-reference-window` viewport lists.
A *Viewport* requires a list of :ref:`gl-reference-drawing-object`\s and a :ref:`gl-reference-camera`. When a display update is required it will request all its *DrawingObjects* to produce their output.

* **title** text, sets a title to be displayed at the top of the viewport.
* **axis** True/False, if true an axis will be drawn in the viewport.
* **axisLength** Number, sets the length of the axis arrows as a ratio to the viewport size [0,1].
* **antialias** True/False, set to False to disable anti-aliasing.
* **rulers** True/False, draw rulers along model boundaries.
* **timestep** True/False, display a time step printout at the edge of the viewport.
* **border** Integer, 0 to disable border, 1 to enable, 2 for a filled frame.
* **borderColour** Colour, colour to draw the border in.
* **margin** Integer, margin in pixels to leave at the viewport edge when automatically zooming the camera.
* **nearClipPlane** Number, override the OpenGL near clip position.
* **farClipPlane** Number, override the OpenGL far clip position.
* **scaleX** Number, scaling factor to apply to all geometry in X axis (width).
* **scaleY** Number, scaling factor to apply to all geometry in Y axis (height).
* **scaleZ** Number, scaling factor to apply to all geometry in Z axis (depth).
* **disable** True/False, set to True to prevent this viewport producing output.
* **Camera** Name of the camera component to use.
* **DrawingObject** A list of drawing object components to display in the viewport. (see :ref:`gl-windows-and-viewports`)

.. _gl-reference-camera:

Camera
------
Create one for each different viewpoint to be applied on the scene. Include it in the :ref:`gl-reference-viewport` definition to apply this camera view to a set of *DrawingObjects* (see :ref:`gl-camera`).

* **CentreFieldVariable** Name of a field variable used to define region to set the camera focal point automatically.
* **CoordinateSystem** LeftHanded/RightHanded, set to LeftHanded to flip the coordinate system to have the Z-axis pointing into the screen.
* **rotate[X/Y/Z]** Number, rotation in degrees about axis.
* **translate[X/Y/Z]** Number, translate the model view in each axis.
* **focalPoint[X/Y/Z]** Number, override the point the camera is looking at.
* **aperture** Number, override the camera aperture in degrees, default 45.
* **autoZoomTimestep** Integer, when to apply auto zoom (set the camera so the whole model fits within the viewport), set to -1 to disable, 0 for once on setup, or N for every Nth timestep. Default is 0.

.. _gl-reference-drawing-object:
   
Drawing Object
--------------
Parent type for all visual elements to be displayed from the available data.
Include in viewport’s *DrawingObject* lists. Executed by viewports as required. When first called each timestep, the data is re-created from the new results (see :ref:`drawing-objects`).

Global Parameters: (these may be ingnored or interpreted differently by some objects)

* **lit** True/False, set to False to disable lighting of this object.
* **lineWidth** Integer, set the width of lines drawn by this object.
* **colour** Colour, base colour to draw the object in.
* **ColourMap** Name of the colourmap component to use when drawing the object.
* **opacity** Number [0,1], set the transparency level, 1.0 is fully opaque, 0 is invisible.
* **wireframe** True/False, set to True to draw object surfaces as wireframes instead of filled polygons.
* **disable** True/False, set to True to prevent this object producing output.

.. _gl-reference-colourmap:

Colour Map
----------
Required by some :ref:`gl-reference-drawing-object`\s, and many may be defined or the same one used for various *DrawingObjects*. It maps a colour range to the values of a variable (field or swarm).

* **dynamicRange** True/False, set to False to manually set the minimum/maximum of the colour range, if True the global minimum and maximum of the variable mapped will be used.
* **minimum** Number, set the minimum value to map to the start of the colour range.
* **maximum** Number, set the maximum value to map to the end of the colour range.
* **centreValue** Number, if set and dynamic range is True, this value will be at the centre of the map, the minimum or maximum will be adjusted out in order to keep the scale linear.
* **discrete** True/False, set to True to disable colour interpolation, colours will not be blended but plotted as exact discrete values.
* **logScale** True/False, set to True to use a logarithmic scale on the map.
* **colours** Colour List, list of colours separated by blank space, commas or semi-colons. Each colour can be one of the names listed here http://en.wikipedia.org/wiki/X11_color_names or a hexidecimal RGB value in the format #RRGGBB. An alpha (opacity) value can be specified with a colon, #RRGGBB:A, eg: #AAFF00:0.5. If a colour value is immediately preceded by a numerical value in brackets that colour will be locked to that value. Eg: (990)red, red will represent the value 990 in the map and will be positioned accordingly. Such values need to be selected sensibly, in ascending or descending order.

.. _gl-interactive:

Interactive visualisation
=========================
The database output file generated from the simulation can be visualised interactively using the gLucifer viewer.
This is particularly useful for 3D model runs when a set of 2D image views of the output is too limiting.

The interactive viewer is built with Underworld by default if the required visualisation dependencies
(see :ref:`uw-additional-software`) are available. The *gLucifer* viewer executable is found
together with the *Underworld* executable, usually in *$PATHTOUNDERWORLD/build/bin*.
If not, pre-built binary installs of the gLucifer Viewer for Ubuntu/Mac/Windows can be downloaded from the project website.

The database output can be visualised by simply passing its filename into the *gLucifer* executable.

For example::
 
  build/bin/gLucifer output/gLucifer.gldb

A window showing the first timestep should appear. The mouse and 
keyboard can be used to interact with the model. 
The user interface to set viewing parameters and other actions is keyboard driven, pressing the *F1* key will display a list of 
available interaction commands in the terminal. 

.. note:: On Macs, holding the apple key with the left mouse button can be used if the mouse has no right button.


.. figure:: _static/viewer.*

   A 3D Model loaded in the gLucifer Interactive Viewer

Command Line Options
--------------------
* **-v** verbose: print debugging output showing details of the viewer processing
* **-o** output: echo entered mouse and keyboard commands to standard output, allowing capture to a script
* **-i** input: read commands from standard input, allows loading a script
* **-x** output width: set a fixed image width for writing output images, the window will be set to this width when writing images, maintaining the original aspect ratio
* **-a** alpha: override the global opacity setting for all objects [0-255]
* **-d** request SDL window, if available SDL will be used to create the viewer window
* **-g** request GLUT window, if available GLUT will be used to create the viewer window
* **-h** hide window, don't display the window
* **-s** request stereo, if available a stereo context will be created allowing 3D stereo viewing (must also be turned on in viewer with the "\" key)
* **-f** full screen window, open the viewer in full screen mode
* **-w** write images, writes all selected timesteps as images then quits
* **-W** write images to the database location, as above but saves to the directory containing the database instead of the current directory
* **-m** write movie, writes all selected timesteps to a video file then quits
* **-t** transparent png, png images will have a transparent background
* **-#** where # is a number, the first time this occurs will set the initial timestep, the second time will set the final timestep for batch writing of images and video.

The following switches stop the viewer loading geometry of the specified type from the database:

* **-B** disable label text output
* **-P** disable point output
* **-S** disable surface output
* **-U** disable isosurface output
* **-V** disable vector output
* **-T** disable tracer output
* **-L** disable line output
* **-H** disable shape output

Viewer Controls
---------------

Mouse controls

* **Left button (click & drag)** Rotate about the X & Y axes
* **Right button (click & drag)** Pan (left/right/up/down)
* **Middle button (click & drag)** Rotate about the Z axis
* **Scroll wheel** Zoom in and out.
* **[Shift] + Scroll wheel** Move the near clip plane in and out.

Keyboard controls: single key

* **F1** Print help
* **Down** Load next model/window at current time-step if data available
* **Up** Load previous model/window at current time-step if data available
* **Right** Next time-step
* **Left** Previous time-step
* **Page Down** Next viewport if available
* **Page Up** Previous viewport if available
* **Home** View All mode ON/OFF, shows all objects in a single viewport
* **End** ViewPort mode ON/OFF, shows all viewports in window together

* **a** Hide/show axis
* **b** Background colour switch: White/Black
* **B** Background colour switch: Light grey/Dark grey
* **c** Camera info: XML output of current camera parameters
* **d** Draw quad surfaces as triangle strips On/Off
* **f** Frame box mode On/Filled/Off
* **g** Colour map log scales override Default/On/Off
* **j** Experimental: localise colour scales, minimum and maximum calibrated to each object drawn
* **J** Restore original colour scale min & max
* **k** Lock colour scale calibrations to current values On/Off
* **l** Lighting On/Off
* **m** Model bounding box update - resizes based on min & max vertex coords read
* **n** Recalculate surface normals
* **o** Print list of object names with id numbers.
* **r** Reset camera viewpoint
* **s** Take screen-shot and save as png/jpg image file
* **q** or [ESC] Quit program
* **u** Back face Culling On/Off
* **w** Wireframe On/Off
* **`** Full screen On/Off
* **\*** Auto zoom to fit On/Off
* **/** Stereo On/Off
* **"*"** Switch coordinate system Right-handed/Left-handed
* **|** Switch rulers On/Off
* **@** Zero camera - set to coord (0,0,0)
* **;** Flat tracer rendering On/Off
* **:** Tracer scaling by time-step On/Off
* **,** Switch to next particle rendering texture
* **+** More particles (reduce sub-sampling)
* **=** Less particles (increase sub-sampling)

* **v** Increase vector size scaling
* **V** Reduce vector size scaling
* **t** Increase tracer size scaling
* **T** Reduce tracer size scaling
* **p** Increase particle size scaling
* **P** Reduce particle size scaling
* **h** Increase shape size scaling
* **H** Reduce shape size scaling

Keyboard controls: key combinations

* **Alt + p**  hide/show all particle swarms
* **Alt + v**  hide/show all vector arrow fields
* **Alt + t**  hide/show all tracer trajectories
* **Alt + s**  hide/show all quad surfaces (scalar fields, cross sections etc.)
* **Alt + u**  hide/show all triangle surfaces (isosurfaces)
* **Alt + h**  hide/show all shapes
* **Alt + i**  hide/show all lines

Keyboard controls: type a number and then press:

* **Enter** skip to time-step entered
* **Page Up** skip back entered timesteps relative to current timestep
* **Page Down** skip forward entered timesteps relative to current timestep
* **o** hide/show objects by global id number.
* **p** hide/show particle swarms by id number.
* **v** hide/show vector arrow fields by id
* **t** hide/show tracer trajectories by id
* **s** hide/show quad surfaces by id (scalar fields etc.)
* **u** hide/show triangle surfaces by id (isosurfaces)
* **h** hide/show shapes by id
* **i** hide/show lines by id
* **e** override tracer trajectory steps with entered number
* **E** clears the tracer step override.

