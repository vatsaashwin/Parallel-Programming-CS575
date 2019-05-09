Introduction
There are many problems in scientific and engineering computing where you want to multiply arrays of numbers (matrix manipulation, Fourier transformation, convolution, etc.).

This project is in two parts. The first part is to test array multiplication, SIMD and non-SIMD. The second part is to test array multiplication and reduction, SIMD and non-SIMD.

Use the gcc or g++ compilers for both parts, but... Because simd.p4.cpp uses assembly language, this code is not portable. I know for sure it works on flip, using gcc/g++ 4.8.5. You are welcome to try it other places, but there are no guarantees. It doesn't work on rabbit. Do not use "-O3".

For the "control groups" benchmarks, do not use OpenMP parallel for-loops. Just use straight C/C++ for-loops. In this project, we are only using OpenMP for the timing.

Requirements
1. Use the supplied SIMD SSE code to run an array multiplication timing experiment. Run the same experiment a second time using your own C/C++ array multiplication code.
2. Use the supplied SIMD SSE code to run an array multiplication + reduction timing experiment. Run the same experiment a second time using your own C/C++ array multiplication + reduction code.
3. Use different array sizes from 1K to "big". The choice of in-between values is up to you, but pick something that will make for a good graph.
4. You should run each array-size test a certain number of trials. Use the peak value for the performance you record.
5. Create a table and a graph showing SSE/Non-SSE speed-up as a function of array size. As there are two experiments (array-multiply and array-multiply-reduction) there will be two curves. Plot both curves on the same set of axes. Speedup in this case will be S = Psse/Pnon-sse = Tnon-sse/Tsse (P = Performance, T = Elapsed Time).
6. Note: this is not a multithreading assignment, so you don't need to worry about a NUMT.
7. The Y-axis performance units in this case will be "Speed-Up", i.e., dimensionless.
8. Your commentary write-up (turned in as a PDF file) should tell:
--1 What machine you ran this on
--2 Show the table and graph
--3 What patterns are you seeing in the speedups?
--4 Are they consistent across a variety of array sizes?
--5 Why or why not, do you think?
--6 Knowing that SSE SIMD is 4-floats-at-a-time, why could you get a speed-up of < 4.0 or > 4.0 in the array-mutiplication?
--7 Knowing that SSE SIMD is 4-floats-at-a-time, why could you get a speed-up of < 4.0 or > 4.0 in the array-mutiplication-reduction?
