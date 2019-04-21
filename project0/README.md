Introduction
A great use for parallel programming is identical operations on large arrays of numbers.

Requirements
1. Pick an array size to do the arithmetic on. Don't pick something too huge, as your machine may not allow you to use that much memory. Don't pick something too small, as the overhead of using threading might dominate the parallelism gains.

2. Using OpenMP, pairwise multiply two large floating-point arrays, putting the results in another array.

3. Do this for one thread and do this for four threads: 
#define NUMT 1 
and 
#define NUMT 4

4. Time the two runs. Convert the timing results into "Mega-Multiplies per Second".

5. Review the Project Notes pages on "How Reliable is the Timing?"

6. What speedup, S, are you seeing when you move from 1 thread to 4 threads? 
S = (Execution time with one thread) / (Execution time with four threads)

7. If your 4-thread-to-one-thread speedup is S, compute the parallel fraction:
float Fp = (4./3.)*( 1. - (1./S) );
Don't worry what this means just yet. This will become more meaningful soon.

8. Your written commentary (turned in as a PDF file) should include:
Tell what machine you ran this on
What performance results did you get?
What was your 4-thread-to-one-thread speedup?
If the 4-thread-to-one-thread speedup is less than 4.0, why do you think it is this way?
What was your Parallel Fraction, Fp?
