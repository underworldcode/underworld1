.. _uw-journal:

Journalling
===========

Journalling can provide different levels of information about how the model is progressing.
In the case of a desktop machine, this information will display in the terminal window on the
screen as the model runs. When running a model on a computer cluster, journalling information
will not be displayed on the screen but will be dumped to a file *queuename.o* where
*queuename* is the up-to-ten-letter name assigned to the job in the pbs script.

Each category consists of an *info*, *debug* and *error* journal. Journals are controlled
from within an input file as follows:

 * The *info* journal is enabled (t or true) or disabled (f or false)
 * the level of information required for each category is specified with an integer, where 1 is
   the least amount of information. Available levels are as follows:

   * *1* - General (one line).
   * *2* - Medium (per timestep).
   * *3* - Verbose (lots of information, per inner loop per time step).
   * *< 3* - Even more information ...

Journal Status
--------------

Journal and stream status at the start of a simulation on a desktop machine can be viewed
to know which ones are enabled using the *--showJournalStatus* command line argument.

For example::
 
 --showJournalStatus=True
This will give a print out of all available streams and their activation status.

Outputting
----------

On a Desktop
^^^^^^^^^^^^

By default journalling information will be displayed in the terminal window as the model runs.
This information can be instead redirected to an output file by simply redirecting the output
to a file.

For example::
 
 ./Underworld MyInputFiles/Sample.xml > filename.txt
This will redirect the journal output into *filename.txt* file.

On a Cluster
^^^^^^^^^^^^

When running a model on a computer cluster, journalling information will not be displayed on the
screen but will be instead dumped to a file *queuename.o*. For this reason, when running a long-running
parallel job on a cluster, it is strongly advised to reduce the level of journal output to avoid
filling up the stdout buffer.

Journal XML Controls
--------------------

The following list outlines the various journal controls that can be toggled by the user.

Enabling/Disabling
^^^^^^^^^^^^^^^^^^

 * Enable or disable the entire journal.

   .. code-block:: xml 
 
    <param name="journal">on/off</param>
 * Enable or disable a typed stream.

   .. code-block:: xml

    <param name="journal.TypedStream">on/off</param>
 * Enable or disable a named stream or its sub-streams.

   .. code-block:: xml

    <param name="journal.TypedStream.NamedStream...">on/off</param>
 * Enable or disable a named stream, and all its children.

   .. code-block:: xml

    <param name="journal-enable-branch.TypedStream.NamedStream...">on/off</param>

Level of Output
^^^^^^^^^^^^^^^

 * Set the level of output for a typed stream.

   .. code-block:: xml

    <param name="journal-level.TypedStream">integer</param>
 * Set the level of output for a named stream.

   .. code-block:: xml

    <param name="journal-level.TypedStream.NamedStream...">integer</param>
 * Set the level of output for a typed stream, and all children of that type.

   .. code-block:: xml

    <param name="journal-level-branch.TypedStream">integer</param>
 * Set the level of output for a named stream, and all its children.

   .. code-block:: xml

    <param name="journal-level-branch.TypedStream.NamedStream...">integer</param>

Printing Rank
^^^^^^^^^^^^^

 * Set the rank number which will be printing from this typed stream.

   .. code-block:: xml

    <param name="journal-rank.TypedStream">integer</param>
 * Set the rank number which will be printing from this named stream.

   .. code-block:: xml

    <param name="journal-rank.TypedStream.NamedStream...">integer</param>

Dump Every
^^^^^^^^^^

 * How often a typed stream dumps output.

   .. code-block:: xml

    <param name="journal-dumpevery.TypedStream">integer</param>
 * How often a named stream or sub-stream dumps output.

   .. code-block:: xml

    <param name="journal-dumpevery.TypedStream.NamedStream...">integer</param>

Auto Flush
^^^^^^^^^^

 * Set whether this typed stream will flush after every output call.
 
   .. code-block:: xml

    <param name="journal-autoflush.TypedStream">on/off</param>
 * Set whether this named stream will flush after every output call.

   .. code-block:: xml

    <param name="journal-autoflush.TypedStream.NamedStream...">on/off</param>

Output Redirection
^^^^^^^^^^^^^^^^^^

 * Redirect a typed stream to a file output.

   .. code-block:: xml

    <param name="journal-file.TypedStream">filename</param>
 * Redirect a named stream to a file output.
 
   .. code-block:: xml

    <param name="journal-file.TypedStream.NamedStream...">filename</param>

MPI File Offsets
^^^^^^^^^^^^^^^^

 * Set the number of bytes offset for the MPI file associated with the stream.

   .. code-block:: xml

    <param name="journal-mpi-offset.TypedStream">filename</param>
 * Set the number of bytes offset for the MPI file associated with the stream.

   .. code-block:: xml

    <param name="journal-mpi-offset.TypedStream.NamedStream...">filename</param>

Examples
^^^^^^^^

 * **debug**

   * Enables (true) or disables (false) the *debug* stream.

     .. code-block:: xml

      <param name="journal.debug">true</param>
   * Enables (true) or disables (false) the *Plugin* stream of *debug* or its sub-streams.

     .. code-block:: xml

      <param name="journal.debug.Plugin">true</param>
 * **Context-verbose**

   * Enables (true) or disables (false) the *Context-verbose* stream or its sub-streams.

     .. code-block:: xml

      <param name="journal.info.Context-verbose">true</param>
 * **Components**

   * Sets the level (an integer, e.g. 2) of output for the *Stg_ComponentFactory* stream.

     .. code-block:: xml

      <param name="journal-level.info.Stg_ComponentFactory">2</param>
 * **StgFEM**

   * Enables (true) or disables (false) the *StgFEM* stream and all of its children.

     .. code-block:: xml

      <param name="journal-enable-branch.debug.StgFEM">true</param>
   * Sets the level (an integer, e.g. 2) of output for the *StgFEM* stream and all of its
     children.

     .. code-block:: xml

      <param name="journal-level-branch.debug.StgFEM">2</param>
 * **Swarm**

   * Enables (true) or disables (false) the *Swarm* stream and all of its children. 

     .. code-block:: xml

      <param name="journal-enable-branch.debug.Swarm">true</param>
   * Sets the level (an integer, e.g. 2) of output for the *Swarm* stream and all of its children.
 * **gLucifer**

   * Enables (true) or disables (false) the *lucDebug* stream and all of its children.

     .. code-block:: xml

      <param name="journal-enable-branch.debug.lucDebug">true</param>
   * Sets the level (an integer, e.g. 2) of output for the *lucDebug* stream and all of its
     children.

     .. code-block:: xml

      <param name="journal-level-branch.debug.lucDebug">2</param>
   * Sets the level (an integer, e.g. 2) of output for the *lucDebug* stream.

     .. code-block:: xml

      <param name="journal-level.debug.lucDebug">2</param>
   * Sets the level (an integer, e.g. 2) of output for the *lucInfo* stream.

     .. code-block:: xml

      <param name="journal-level.info.lucInfo">2</param>

.. _Underworld Downloads: http://www.underworldproject.org/downloads.html
