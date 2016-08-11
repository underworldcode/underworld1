**********************
Underworld XML Changes
**********************

This file details xml input file changes required for making old input files
(compatible with Underworld-1.3), compatible with versions Underworld-1.4 and Underworld-1.5.

1. User must specify the “context” of the model 
===============================================

This must be done first before other components are read into the dictionary,
otherwise the simulation will halt in its 'read from input file' phase.

To perform this add::

   <struct name="components" mergeType="merge">
      <struct name="context" mergeType="merge">
         <param name="Type">UnderworldContext</param>
      </struct>
   </struct>

We recommend this be included directly after <list name=”import”>...</list>

2. Plugins are now defined in XML the same way as components
============================================================

The general syntax for adding a plugin is now::

   <list name="plugins" mergeType="merge">
       <struct>
           <param name="Type">PlugingName</param>
           <param name="Context">context</param>
           ...
       </struct>
   </list>

where the '...' represents arguments to pass into the plugin.

So what was, as of Underworld 1.2::

	<list name="plugins" mergeType="merge">
		<!-- Output Plugins -->
		<param>StgFEM_FrequentOutput</param>	
		<param>Underworld_Vrms</param>	
		<param>StgFEM_CPUTime</param>
	</list>

Is now::

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
	</list>

This adds verbosity to the plugins and requires the user to know what parameters to pass to a plugin.
As with the “Components”, parameters passed in are error checked and the code will halt
and report a problem if invalid parameters are found.
This change is necessary for the code restructure of late 2009.

3. Verticies ---- > Vertices
============================

The code now uses the correct spelling 'vertices' to define a list containing multiple vertex points.
The previous, incorrect spelling was 'verticies'.
Please check that your Shape definitions are consistent with this.

3. gLucifer
=====================
gLucifer is no longer a plugin and should be removed from the "plugins" list.
To use it, simply add it to the "import" list::

   <list name="import" mergeType="merge">
      <param>gLucifer</param>
   </list>

Isosurfaces in gLucifer now use the "resolution" parameter to define the sample count per element, not the global sample count.


5. OperatorFeVariable parameter change (FeVariable/s -> Operand/s)
==================================================================

OperatorFeVariable's FeVariable/s have been renamed to Operand/s to be generic.
For example::

   <struct name="StrainRateField">
      <param name="Type">OperatorFeVariable</param>
      <param name="Operator">TensorSymmetricPart</param>
      <param name="Operand">VelocityGradientsField</param>
      <param name="outputUnits">yr^-1</param>
   </struct>

   <struct name="ViscousDissipationField">
     <param name="Type">OperatorFeVariable</param>
     <param name="Operator">TensorInnerProduct</param>
     <list name="Operands">
        <param>StrainRateField</param>
        <param>StressField</param>
     </list>
   </struct>
