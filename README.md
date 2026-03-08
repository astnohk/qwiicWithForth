# qwiicWithForth

Use sensors or other hardware connected via I2C from Forth.

## Build

Build with CMake.

```
cmake -B build
cmake --build build
```

## Run

`pforth/fth/pforth_standalone` will be generated when the build process is successfully finished.
Just run the executable and use custom added words `I2CREAD` and `I2CWRITE`.

## About this implementation

This project uses [pForth](https://www.softsynth.com/pforth/) ([philburk/pforth](philburk/pforth)) for using custom wrapper functions.
`pf_i2c.c` includes wrapper functions of i2c-dev so this project only runs on Linux currently.

## LICENSE

This project distributed under 0BSD License (in honor of pForth).
