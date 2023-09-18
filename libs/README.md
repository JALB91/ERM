# ERM Libs (WIP)

*ERM Libs* are an easy way of adding self-contained code that can be shared with other libraries across the engine.

---

## Project structure

The common structure of an *ERM Lib* is as follow (**!!! NAMING IS IMPORTANT !!!**):

```
<NAME>
│    CMakeLists.txt 
│    ...
│
└─── Sources
│   │
│   └─── <TARGET_API>
│   │   │    <file>.h/.cpp
│   │   │    ...
│   │
│   └─── common
│       │    <file>.h/.cpp
│       │    ...
│
└─── NN_Data
│   │
│   └─── <TARGET_API>
│   │   │    <file>.nn/.nns
│   │   │    ...
│   │
│   └─── common
│       │    <file>.nn/.nns
│       │    ...
```

## CMakeLists.txt

There are two cmake helper macros available to setup your project, they will will gather sources based on the `ERM_TARGET_API` (see the available options [here](https://github.com/JALB91/ERM/blob/master/README.md)), create a library/executable (based on which method you call), set common C++ options, properties and definitions, they are called respectively:

- `erm_library_project(VERSION DESCRIPTION)`
- `erm_executable_project(VERSION DESCRIPTION)`

They will internally call the cmake command `project` so from there you will be able to access the `PROJECT_NAME` variable as usual.

After that you can finish to manually setup your project (i.e. linking with other libraries).

## Sources

Simply contains your source files (.h/.cpp).

## NN_Data (WIP)
