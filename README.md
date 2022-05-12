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

## Running the program

```bash
src/vulkan
```
