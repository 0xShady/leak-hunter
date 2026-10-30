# Leak_Hunter

### Simple leak detector for your C projects

Leak_Hunter records every `malloc`, `calloc`, `realloc` and `strdup` made by your code, with the file and line, and writes a report of what was never freed when your program exits. It also catches double frees and frees of pointers that were never allocated.

No install, no external tools: two files to drop in your project.

## What is a memory leak
A memory leak happens when a program allocates memory and never releases it once it is no longer needed. More about [memory leaks](https://en.wikipedia.org/wiki/Memory_leak).

## How to use it

Let's say we want to track the leaks in `test.c`:

```c
#include <stdlib.h>

int     main(void)
{
    char    *s = malloc(10);
    int     *i = malloc(20 * sizeof(int));
    float   *f = malloc(5 * sizeof(float));
    /*some code*/
    return (0);
}
```

1. Copy `leak_hunter.h` and `leak_hunter.c` into your project.
2. Include `leak_hunter.h` in your project's header (or in each `.c` file you want to track), **after** the system headers.
3. Register the report at the start of `main`:

```c
#include <stdlib.h>
#include "leak_hunter.h"

int     main(void)
{
    atexit(leak_report);
    char    *s = malloc(10);
    int     *i = malloc(20 * sizeof(int));
    float   *f = malloc(5 * sizeof(float));
    /*some code*/
    return (0);
}
```

4. Compile `leak_hunter.c` with your files and run:

```sh
cc test.c leak_hunter.c -o test
./test
```

The report is written to `leak.log` (compile with `-DOUTPUT_FILE_NAME='"other.log"'` to use another path, or `-DLEAK_HUNTER_STDERR` to print it on stderr instead):

```log
┌─────────┤ LEAKS LOG ├─────────┐
│ Total leaks found:          3 │
│ Total leaks size:         110 │
├───────────────────────────────┤
│ address:       0x6020000000b0 │
│ size (bytes):              10 │
│ file:                  test.c │
│ line:                       7 │
├───────────────────────────────┤
│ address:       0x607000000020 │
│ size (bytes):              80 │
│ file:                  test.c │
│ line:                       8 │
├───────────────────────────────┤
│ address:       0x6030000019f0 │
│ size (bytes):              20 │
│ file:                  test.c │
│ line:                       9 │
└───────────────────────────────┘
```

Once every block is freed:

```log
┌─────────┤ LEAKS LOG ├─────────┐
│ Total leaks found:          0 │
│ Total leaks size:           0 │
├───────────────────────────────┤
│   your code is leaks free!!   │
└───────────────────────────────┘
```

### Bad frees

Freeing the same pointer twice, or a pointer that was never allocated, adds a section to the report with the file and line of the faulty `free`:

```log
├───────────────────────────────┤
│ Bad frees found:            1 │
├───────────────────────────────┤
│ address:       0x6020000000b0 │
│ file:                  test.c │
│ line:                      12 │
└───────────────────────────────┘
```

Bad frees are not passed to the real `free`, so your program keeps running and the report still gets written.

## How it works

`leak_hunter.h` defines macros that replace `malloc`, `calloc`, `realloc`, `strdup` and `free` with wrappers. Each allocation is stored in a list with its address, size, file and line; each `free` removes it from the list. `atexit(leak_report)` runs `leak_report()` when the program ends, and whatever is still in the list is a leak.

`print_mem_data()` prints the blocks that are still allocated at any point, if you want to look before the end.

### Limits
- Only files that include `leak_hunter.h` are tracked. Memory allocated by another library and freed in your code shows up as a bad free and is not released.
- It is a debugging tool: tracking costs a list lookup per `free`. Leave it out of release builds.
- Not thread safe.

## Development

```sh
make test      # builds the tests with -Wall -Wextra -Werror and checks each leak.log
make example   # builds test.c
```

## l3aaaz

Big thanks to [mamali](https://profile.intra.42.fr/users/mamali) & [hfadyl](https://profile.intra.42.fr/users/hfadyl) for all the emotional support

## License

[MIT](LICENSE)
