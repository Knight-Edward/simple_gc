
# Pointer and Address, int* and uintptr_t type
Pointer in nature is just an `Integer`, which represents the memory address.
In C standard, `uintptr_t` is capable of storing the bit pattern of any
data pointer.

```c
// demo
void test() {
    int* p1 = malloc(sizeof(int));
    double* p2 = malloc(sizeof(double));
    uintptr_t x1 = (uintptr_t)p1;
    uintptr_t x2 = (uintptr_t)p2;
    printf("addr of p1 is: %d, addr of p2 is: %d \n", x1, x2);
}

```

Lets imagine a use case for `uintptr_t` type. We have a `scan_region` function
which scans a region of memory and see if the ...

# 


