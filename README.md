# Wireshark plugin sandbox

This is a sandbox where I experiment with building Wireshark plugins.

## Native plugin

This is a plugin written in C, which can be compiled to `.so` or `.dll` files.

### Build using local toolchain

To build the plugin, assuming that you have installed all necessary packages:

```bash
./build.sh
```

### Build using toolchain in Docker

If you have Docker, you can use a plugin builder image that I have put together.
This builds the plugin for several Wireshark versions both for Linux and for Windows.

```bash
./build-in-docker.sh
```

### Using the plugin

The build scripts mentioned above install the plugin within the "config" directory.
You can use the `wireshark-native.sh` script which is set up to use that as a
configuration and plugin directory. You can also pass Wireshark arguments to
that script.

```bash
./wireshark-native.sh examples/single-requests-005.pcapng
```

## Lua plugin

This is a cross-platform plugin written in Lua. There is no need for compilation.

Note: the latest release of Wireshark (4.4.5) doesn't yet support
conversations in its Lua API, so this needs a development build.

### Using the plugin

You can use the `wireshark-lua.sh` script which loads the plugin automatically.

```bash
./wireshark-lua.sh examples/single-requests-005.pcapng
```