# ERM Libs (WIP)

*ERM Libs* are an easy way of adding self-contained code that can be shared with other libraries across the engine.

---

The common structure of an *ERM Lib* is as follow:

```
<NAME>
│   CMakeLists.txt 
│
└───Sources
    │
    └───<TARGET_API>
    │   │
    │   │   ...
    │
    └───common
        │
        │   ...
```

## CMakeLists.txt

There are two `cmake` helper functions to setup your project, they both will gather the sources based on the `TARGET_API` (see the available options in https://github.com/JALB91/ERM/blob/master/README.md), create a library/executable (based on which method you will call), set common C++ options and compile definitions, they are called:

- `erm_setup_library()`
- `erm_setup_executable()`

After that you can finish to manually setup your project (i.e. linking with other libraries).
