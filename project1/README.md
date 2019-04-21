Introduction
Monte Carlo simulation is used to determine the range of outcomes for a series of parameters, each of which has a probability distribution showing how likely each option is to happen. In this project, you will take a scenario and develop a Monte Carlo simulation of it, determining how likely a particular output is to happen.

A laser is pointed at a circle (circle, in this case). The circle is defined by a center point (xc,yc) and a radius (r). The beam comes out at a 45° angle. It bounces off the circle. Underneath, even with the laser origin, is an infinite plate. Given all of this, does the beam hit the plate?

Normally this would be a pretty straightforward geometric calculation, but the circle is randomly changing location and size. So now, the laser beam might hit the plate or it might not, depending on the values of (xc, yc, r ). OK, since it is not certain, what is the probability that it hits the plate? This is a job for multicore Monte Carlo simulation!

Because of the variability, the beam could miss the circle entirely (A). The circle might totally engulf the laser pointer (B). It could bounce off the circle and miss the plate entirely (C). Or, it could bounce off the circle and actually hit the plate (D).

So, now the question is "What is the probability that the beam hits the plate?"

Requirements:
1. Ranges should be:

Variable  Range
xc        -1.0 - 1.0
yc        0.0 - 2.0
r         0.5 - 2.0

2. Run this for some combinations of trials and threads. Do timing for each combination. Like we talked about in the Project Notes, run each experiment some number of tries, NUMTRIES, and record just the peak performance.
Do a table and two graphs showing performance versus trials and threads.

Chosing one of the runs (the one with the maximum number of trials would be good), tell me what you think the actual probability is.

Compute Fp, the Parallel Fraction, for this computation.
