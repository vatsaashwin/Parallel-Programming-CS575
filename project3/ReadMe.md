Introduction

This project will use parallelism, not for speeding data computation, but for programming convenience. You will create a month-by-month simulation in which each agent of the simulation will execute in its own thread where it just has to look at the state of the world around it and react to it.
You will also get to exercise your creativity by adding an additional "agent" to the simulation, one that impacts the state of the other agents and is impacted by them.

Use of Threads:

The GrainGrowth and GrainDeer threads will each compute the next grain height and the next number of deer based on the current set of global state variables. They will compute these into local, temporary, variables. They both then will hit the DoneComputing barrier.
At that point, both of those threads are done computing using the current set of global state variables. Each thread should then copy the local variable into the global version. All 3 threads will then hit the DoneAssigning barrier.
At this point, the Watcher thread will print the current set of global state variables, increment the month count, and then use the new month to compute the new Temperature and Precipitation. Note that the GrainGrowth and GrainDeer threads can't proceed because there is a chance they would re-compute the global state variables before they are done being printed. All 3 threads will then hit the DonePrinting barrier.
After spawning the threads, the main program should wait for the parallel sections to finish.
Each thread should return when the year hits 2025 (giving us 6 years, or 72 months, of simulation).
Remember that this description is for the core part of the project, before you add your own agent to the simulation. That will involve another thread and some additional interaction among the global state variables.

Quantity Interactions

The Carrying Capacity of the graindeer is the number of inches of height of the grain. If the number of graindeer exceeds this value at the end of a month, decrease the number of graindeer by one. If the number of graindeer is less than this value at the end of a month, increase the number of graindeer by one.
Each month you will need to figure out how much the grain grows. If conditions are good, it will grow by GRAIN_GROWS_PER_MONTH. If conditions are not good, it won't.
You know how good conditions are by seeing how close you are to an ideal temperature (°F) and precipitation (inches). Do this by computing a Temperature Factor and a Precipitation Factor.

Results

1. What your own-choice quantity was and how it fits into the simulation.
2. A table showing values for temperature, precipitation, number of graindeer, height of the grain, and your own-choice quantity as a function of month number.
3. A graph showing temperature, precipitation, number of graindeer, height of the grain, and your own-choice quantity as a function of month number. Note: if you change the units to °C and centimeters, the quantities might fit better on the same set of axes.
cm = inches * 2.54 
°C = (5./9.)*(°F-32)
This will make your heights have larger numbers and your temperatures have smaller numbers.
4. A commentary about the patterns in the graph and why they turned out that way. What evidence in the curves proves that your own quantity is actually affecting the simulation correctly?
