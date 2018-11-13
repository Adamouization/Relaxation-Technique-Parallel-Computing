# TODO

## System-critical

* [X] Implement relaxation technique sequentially
* [X] Run relaxation in a single thread
* [ ] Implement relaxation on multiple threads (lock array for now, no need to be efficient)
* [ ] Implement relaxation on multiple threads efficiently (synchronization, lock specific values only)
* [ ] Write report in LaTeX

## Code design

* [X] Move all code from the main() to individual functions.
* [X] Move log print statements to own funciton.
* [X] Create a makefile for compiling the code with GCC then running it.
* [X] Move array-related functions in own C file.
* [X] Move print-related functions in own C file.
* [X] Refactor code to fit all within 80 chars per line.

## Balena

* [X] Add email address in job script for notifications when job finishes running.
