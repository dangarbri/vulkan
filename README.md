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
- SHOW\_AVAILABLE\_EXTENSIONS - Print queried device extensions to stdout
- SHOW\_QUEUE\_CREATION - Print queues requested on the device to stdout
- SHOW\_RESOURCE\_ALLOCATION - Prints all create/destroy calls to stdout

## Running the program

```bash
src/vulkan
```
