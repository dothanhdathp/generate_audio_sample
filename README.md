# Generate audio sample for Windown

## Technical

- ftxui: terminal ui
- audio output: raw and wav file

## Request

1. MSYS2

Download tool build

```bash
pacman -S mingw-w64-x86_64-make
```

Check g++:

```bash
which g++
```

If g++ not avaible, can download by:

```bash
pacman -S mingw-w64-x86_64-gcc
```

Then change the flag in `makefile` to `x86_64-w64-mingw32-g++.exe`

## Build

```bash
make all
```
