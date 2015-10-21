# creamteaRootMcp
The ROOT MCP package is small package that provides DAQ, event display and rudimentary analysis functionality for the CREAM TEA electronics. Clearly the functionality will need to evolve somewhat. But this is what we have for now.

If you are using this on the UCL HEP machines, you can:
* Setup ROOT --  downloading and sourcing the script  http://www.hep.ucl.ac.uk/~rjn/creamtea/rootMcp/setupUclRoot.sh (or putting similar lines in bash_profile)
* Type make -- On machines other than pclab6 then one needs to install ROOT, FFTW3 and libRootFftwWrapper (the last two of which aren't strictly necessary). These can be obtained from:
 http://www.fftw.org 

Running:

There are a number of different scripts that one can use by typing root macroName.C

runTargetDisplay.C::
  This is the standard way to run the display directly as a data acquisition and event display program. By default (I think) it sends software triggers and reads out the data. We need to add more hooks for changing thresholds and various other things.

createOfflineFile.C::
  This is a batch mode program that creates an output ROOT file that can be read offline by...

runOfflineTargetDisplay.C::
  Reads in and displays the events stored in offline ROOT files.

runPedestal.C::
  Generates a pedestal file using software triggers.

loadTargetLibs.C::
  Just a wrapper that exposes some of the underlying nuts and bolts of the DAQ interface.

Links
Doxygen "documentation"::
  http://www.hep.ucl.ac.uk/~rjn/creamtea/rootMcp/doxygen/
  
GitHub:
  https://github.com/nichol77/creamteaRootMcp
