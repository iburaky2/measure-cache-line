# measure-cache-line

Experiment to measure the size of a cache line describe in [blog post](https://iburaky.com/measure-cache-line).

## Build and Run
If you do not have `g++-14` modify the Makefile.
You may also want to [pin to other cores](https://github.com/iburaky2/measure-cache-line/blob/main/main.cpp#L46) depending on your CPU.

```
make
./out
```