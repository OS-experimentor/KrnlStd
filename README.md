# KrnlStd
Standard header for kernel dev --- used for monolithic krnls and educational kernels.


## If you want to use it as a modular kernel:
Change the `static` declarations to `extern`, then define the variable in the C file. For example:
### krnlstd.h:
```c
static size vx = 0; // change ts to extern size vx;
```

### Your C file:

```c
size vx = 0; // now we're defining it here!
```

