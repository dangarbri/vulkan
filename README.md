# Vulkan

Small demos implemented with vulkan as a learning experience

## Compiling

By default, compiling will include validation layers and debug
messages. To remove these, define NDEBUG as a CPPFLAG.

```bash
# Release build
CPPFLAGS=-DNDEBUG ./configure
make

# Debug build (default)
./configure
make
```

### Debug Flags

Extra flags can be added to increase/decrease verbosity.
Flags can be set in the environment variable CPPFLAGS.

For example:

```bash
CPPFLAGS=-DSHOW_AVAILABLE_EXTENSIONS ./configure
make
```

Available flags include:
- SHOW_AVAILABLE_EXTENSIONS

## Running the program

```bash
src/vulkan
```
