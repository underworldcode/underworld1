.. _uw-passive-tracers:

Passive Tracers
===============

Passive tracers are particles that can be added to a model which move passively within the
domain without influencing the model. They can be used to record x, y and z positions and also
field values from user-defined points or regions in a model at each timestep.

The key difference between this form of output as compared to outputting fields for the whole
domain, is the choice of the user-defined *passiveTracerSwarm*, as opposed to the whole
*materialSwarm*. If field value output are required see :ref:`gl-output`.

Requirements
------------

To make use of passive tracers, the following aspects must be covered:
 
 * **ParticleMovementHandler** - This is an essential component to be added to the XML input file.

   For example:

   .. code-block:: xml
   
    <struct name="passiveSwarmMovementHandler">
       <param name="Type"> ParticleMovementHandler </param>
    </struct>

 * **Particle Layout** - Defines the layout of the particles. This first layout to be defined
   is the *ElementCellLayout*.

   For example:

   .. code-block:: xml

    <struct name="ElementCellLayoutTracer">
       <param name="Type">ElementCellLayout</param>
       <param name="Mesh">linearMesh</param>
    </struct>
   Then one can define the required particle layout. Available layouts are as follows:

    * *FileParticleLayout* - Loads a global coordinate layout from a file, in either ASCII or
      HDF5 format. Used to define a particle swarm layout. It is also used in checkpointing data.
    * *LineParticleLayout* - Lays out equally spaced particles on a continuous set of line segment.

      For example:

      .. code-block:: xml

       <struct name="passiveTracerLayoutExample1">
          <param name="Type">LineParticleLayout</param>
          <param name="totalInitialParticles">6</param>
          <list name="vertices">
             <asciidata>
                <columnDefinition name = "x" type="double"/>
                <columnDefinition name = "y" type="double"/>
                <columnDefinition name = "z" type="double"/>
                   0.5 0.1 0.0
                   0.1 1.0 0.0
                   0.3 1.0 2.0
             </asciidata>
          </list>
       </struct>
     * *ManualParticleLayout* - Lays out a particle in each of the positions manually specified.

       For example:

       .. code-block:: xml

        <struct name="passiveTracerLayoutExample2">
           <param name="Type">ManualParticleLayout</param>
           <list name="manualParticlePositions">
              <asciidata>
                 <columnDefinition name = "x" type="double"/>
                 <columnDefinition name = "y" type="double"/>
                 <columnDefinition name = "z" type="double"/>
                    0.5 0.1 0.0
                    0.1 1.0 0.0
                    0.3 1.0 2.0
              </asciidata>
           </list>
        </struct>
       This creates three particles: (0.5, 0.1, 0.0)(0.1, 1.0, 0.0)(0.3, 1.0, 2.0).
     * *SpaceFillerParticleLayout* - Uses the SobolGenerator class to quasi-randomly fill particles
       throughout the whole global domain, by setting either *totalInitialParticles* or *averageInitialParticles*.

       For example:

       .. code-block:: xml

        <struct name="passiveTracerLayoutExample3">
           <param name="Type">SpaceFillerParticleLayout</param>
           <param name="totalInitialParticles">6000</param>
        </struct>

        <struct name="passiveTracerLayoutExample4">
           <param name="Type">SpaceFillerParticleLayout</param>
           <param name="averageInitialParticlesPerCell">20</param>
        </struct>
     * *PlaneParticleLayout* - Uses the *SpaceFillerParticleLayout* class to choose quasi-random
       particle positions and then projects them onto a plane.

       For example:

       .. code-block:: xml

        <struct name="passiveTracerLayoutExample5">
           <param name="Type">PlaneParticleLayout</param>
           <param name="totalInitialParticles">6000</param>
           <param name="planeAxis">y</param>
           <param name="planeCoord">1.0</param>
        </struct>
     * *WithinShapeParticleLayout* - Uses the *SpaceFillerParticleLayout* class to choose quasi-random
       particle positions, but it only places a particle there if it is within a particular shape.

       For example:

       .. code-block:: xml

        <struct name="passiveTracerLayoutExample6">
           <param name="Type">WithinShapeParticleLayout</param>
           <param name="totalInitialParticles">6000</param>
           <param name="shape">boxShape</param>
        </struct>
     * *UnionParticleLayout* - This combines several particle layouts together into one. when one particle
       layout finishes initialising all its particles, then the next one begins.

       For example:

       .. code-block:: xml

        <struct name="passiveTracerLayoutExample7">
           <param name="Type">UnionParticleLayout</param>
           <list name="ParticleLayoutList">
              <param>passiveTracerLayoutExample1</param>
              <param>passiveTracerLayoutExample2</param>
              <param>passiveTracerLayoutExample3</param>
           </list>
        </struct>
 * **Passive Tracer Swarms** - The *SwarmAdvector* component advects the particles of the
   *passiveTracerSwarm*, so that they move with the model. A swarm using a tracer layout neds to be created.

     For example:

     .. code-block:: xml

      <struct name="passiveSwarmMovementHandler">
         <param name="Type">ParticleMovementHandler</param>
      </struct>
      <struct name="ElementCellLayoutTracer">
         <param name="Type">ElementCellLayout</param>
         <param name="Mesh">linearMesh</param>
      </struct>
      <struct name="passiveTracerSwarm">
         <param name="Type">MaterialPointsSwarm</param>
         <param name="CellLayout">ElementCellLayoutTracer</param>
         <param name="ParticleLayout">passiveTracerLayoutExample1</param>
         <param name="FiniteElement_Mesh">linearMesh</param>
         <param name="FeMesh">elementMesh</param>
         <list name="ParticleCommHandlers">
            <param>passiveSwarmMovementHandler</param>
         </list>
      </struct>
     The tracer swarm can be avected using the *SwarmAdvector*.

     For example:

     .. code-block:: xml

      <struct name="passiveTracerAdvect">
         <param name="Type">SwarmAdvector</param>
         <param name="Swarm">passiveTracerSwarm</param>
         <param name="TimeIntegrator">timeIntegrator</param>
         <param name="VelocityField">VelocityField</param>
      </struct>

 * **Output Format** - There are three output swarm to choose from: *SwarmOutput*, *PressureTemperatureOutput*
   and *Underworld_SwarmOutput*.

   * *SwarmOutput* - This is the parent component to *PressureTemperatureOutput*. This component
     outputs x, y and z positions only, one file per particle in the swarm, appended at each timestep.
     The filename is set either by default to *<nameOfSwarmOutput>.<timeStep>.dat* or using the
     *baseFilename* tag.

     For example:

     .. code-block:: xml

      <struct name="passiveTracerSwarmOutput">
         <param name="Type">SwarmOutput</param>
         <param name="Swarm">passiveTracerSwarm</param>
         <param name="columnWidth">15</param>
         <param name="decimalLength">10</param>
         <param name="borderString">|</param>
      </struct>
     Formatting of the output can be controlled via the *columnWidth*, *decimalLength* and
     *borderString* tags. See :ref:`passive-tracer-swarm` for example output.

     .. _passive-tracer-swarm:

     .. figure:: _static/passiveTracerSwarmOutput.*

      Passive Tracer Swarm Output Data.
   * *PressureTemperatureOutput* - This component is a child of *SwarmOutput*. It outputs x, y and z
     positions, as well as pressure and temperature field values on a swarm.

     For example:

     .. code-block:: xml
 
      <struct name="passiveTracerSwarmPressureTemperatureOutput">
         <param name="Type">PressureTemperatureOutput</param>
         <param name="Swarm">passiveTracerSwarm</param>
         <param name="PressureField">PressureField</param>
         <param name="TemperatureField">TemperatureField</param>
      </struct>
   * *Underworld SwarmOutput* - This component allows for a given list of *FeVariable(s)* to be
     calculated on a selected swarm (a passive tracer or the whole materialSwarm). It outputs material id,
     x, y, z positions and the value of the specified field(s) to a file called
     *<nameOfField>.<nameOfSwarm>.<timeStep>.dat*, per timestep, per *FeVariable*.

     To output a list of *FeVariables* that are used in the model, the *StgFEM_FeVariableList* plugin must be
     added in the XML input file.

     For example:

     .. code-block:: xml

      <list name="plugins" mergeType="merge">
         <struct>
            <param name="Type">StgFEM_FeVariableList</param>
            <param name="Context">context</param>
         </struct>
      </list>
     The file *FeVariables.list* will be created in the output directory at the zero timestep.
     This can be enabled by incuding the *Underworld_SwarmOutput* component in the XML file.

     For example:

     .. code-block:: xml

      <struct name="swarmOutput">
         <param name="Type">Underworld_SwarmOutput</param>
         <param name="Swarm">passiveTracerSwarm</param>
         <list name="FeVariables">
            <param>Field1</param>
            <param>Field2</param>
         </list>
      </struct>

     .. figure:: _static/passiveTracerSwarmOutput3.*

      Fault Profile and Material Swarm.

     .. figure:: _static/passiveTracerSwarmOutput4.*

      Strain Rate Invariant Comparisons using output from *Underworld_SwarmOutput*.

 * **Visualising Passive Tracer Particles** - The passive tracer particles do not have to be visualised
   to output field values. To visualise the tracer particles or verify their locations the following 
   options are available:

   * *lucSwarmViewerBase*
   
     * lucSwarmViewer
   * *lucSwarmRGBColourViewer*
   * *lucHistoricalSwarmTrajectory*
   * *lucDrawingObject* - maskValue, maskTolerance, maskType.

     * *lucOpenGLDrawingObject* - No extra options to be added.

       * *lucSwarmViewerBase* - Extra options include *colour*, *ColourVariable*, *OpacityVariable*,
         *MaskVariable*, *drawParticleNumber*, *sameParticleColour*, *subSetEvery*, *positionRange*,
         *minPositionX*, *minPositionY*, *minPositionZ*, *maxPositionX*, *maxPositionY*, *maxPositionZ*.
      
       * *lucSwarmViewer* - Extra options include *pointSize*, *pointSmoothing*.
       * *lucSwarmRGBColourViewer* - Extra options include *ColourRedVariable*, *ColourGreenVariable*,
         *ColourBlueVariable*.
       * *lucHistoricalSwarmTrajectory* - Extra options include *colour*, *historySteps*, *historyTime*,
         *lineWidth*.
 
Examples
--------

The following shows examples on the use of passive tracers in gLucifer.

 * **lucSwarmViewer** - For visualising and verifying particle locations, the *lucSwarmViewer* component
   must be included in the XML file.

   For example:

   .. code-block:: xml

    <struct name="passiveTracerDots">
       <param name="Type">lucSwarmViewer</param>
       <param name="Swarm">passiveTracerSwarm</param>
       <param name="colour">Black</param>
       <param name="pointSize">5.0</param>
    </struct>
   Where the *colour* and *pointSize* parameters can be varied.
   See :ref:`pt-ex-so-01` for sample output image.
   This example so that a variable can be assigned on the particles.

   For example:

   .. code-block:: xml

    <struct name="BCAStrainRateParticleVariable">
       <param name="Type">FeSwarmVariable</param>
       <param name="FeVariable">StrainRateInvariantField</param>
       <param name="Swarm">materialSwarm</param>
    </struct>
   The following example masks everything but the material and 3D space selected.

   .. code-block:: xml

    <struct name="Block">
       <param name="Type">lucSwarmViewer</param>
       <param name="Swarm">materialSwarm</param>
       <param name="ColourVariable">BCAStrainRateParticleVariable</param>
       <param name="ColourMap">LowerMantleColourMap</param>
       <param name="MaskVariable">materialSwarm-MaterialIndex</param>
       <param name="maskType">EqualTo</param>
       <param name="maskValue">2.0</param>
       <param name="pointSize">1.0</param>
       <param name="positionRange">True</param>
       <param name="minPositionX">1.0</param>
       <param name="maxPositionX">1.5</param>
       <param name="minPositionY">-10</param>
       <param name="maxPositionY">10</param>
       <param name="minPositionZ">1.5</param>
       <param name="maxPositionZ">10</param>
    </struct>
   See :ref:`pt-ex-so-02` for sample image output.

   .. _pt-ex-so-01:

   .. figure:: _static/passiveTracerExample1.*

    Passive Tracer Output using *lucSwarmViewer*.

   .. _pt-ex-so-02:

   .. figure:: _static/passiveTracerExample4.*

     Tiramisu 3D Cutout from *lucSwarmViewer*.
 * **lucSwarmRGBColourViewer** - This is equivalent to the lucSwarmViewer, but one can apply a
   different variable to each of the R (*red*), G (*green*) and B (*blue*) channels individually.
   If the channel is not attached to a swarm variable, then it assumes the value *0.0*
 * **lucHistoricalSwarmTrajectory** - This allows trajectories to be viewed as part of the passive
   tracer particles. The *lucHistoricalSwarmTrajectory* must be included in the XML input file.

   For example:

   .. code-block:: xml

    <struct name="TracerTrajectoriesColourMap">
       <param name="Type">lucColourMap</param>
       <param name="colours">Red Orange Green</param>
       <param name="dynamicRange">true</param>
    </struct>
    <struct name="passiveTracerLines">
       <param name="Type">lucHistoricalSwarmTrajectory</param>
       <param name="Swarm">passiveTracerSwarm</param>
       <param name="ColourMap">TracerTrajectoriesColourMap</param>
       <param name="historyTime">50</param>
       <param name="historySteps">20</param>
       <param name="lineSize">2.0</param>
    </struct>
   See :ref:`pt-ex-st-01` for sample image output.
   
   .. _pt-ex-st-01:

   .. figure:: _static/passiveTracerExample7.*
  
    Plane Particle View of *lucHistoricalSwarmTrajectory*.

Other Options
-------------

The following list outlines additional options that can be set when using passive tracers:

 * The length of the trajectory lines can be controlled using the *historySteps* and *historyTime*
   parameters.
 * *historySteps* controls the number of timesteps for which the *passiveTracer* particle coordinates
   are stored. The default is set to *500*. This can be set to match the value passed to the *maxTimeSteps*
   parameter.
 * *historyTime* is not essential. If included, this limits which of the stored coordinates are
   displayed based on the total time. If using *historyTime*, make sure that historySteps is set
   to a value large enough to store all the required coordinates.
 * Trajectory lines can be plotted using one colour by creating a *ColourMap* that uses that particular
   colour as the variable *Colour* cannot be used in place of the *ColourMap* component.

