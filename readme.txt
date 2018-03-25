ResourceAllocator by Tudor Boran (tmb341) for CS-GA-2250 (OS)

To build:

Unpack the .tar.gz into a directory, then run

 module load gcc-5.2.0 && make

To run:

./ResourceAllocator <path-to-input-file>

Contents:
./include
	/data_types.h - contains the declaration for the Action class, Task class, ResourceManager (parent class),
				  - and the two inheriting classes OptimisticResourceManager and BankerResourceManager
				  
./src
	/Action.cpp   - contains the definitions for the Action class. Actions are immutable and are acted upon by the ResourceManager
	/Task.cpp 	  - contains getters and setters for the Task class
	/ResourceManager.cpp - getters and setters that are used by both resource managers
	/OptimisticResourceManager.cpp - implementation of dispatchAction, which breaks down into 
								   - dispatchInitiate, dispatchRequest, dispatchRelease, dispatchTerminate
								   - also contains the definitions for detectDeadlock and handleDeadlock (as well as canSatisfyAnyProcess)
    /BankerResourceManager.cpp 	   - implementation of the dispatchActions like in Optimistic Manager
    							   - the error checking occurs in dispatchInitiate and dispatchRequest. 
    							   - dispatchRequest checks if the state is safe
    /ResourceAllocator.cpp 	- contains main(), reads in the given input file, and has two loops, one for Optimistic and one for Banker
     						- finally, it also contains function to print the result (contents of tasklist) after resource managers run