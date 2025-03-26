# Wireshark plugin sandbox

This is a sandbox where I experiment with building Wireshark plugins.

## Build using local toolchain

To build the plugin, assuming that you have installed all necessary packages:

```bash
./build.sh
```

## Build using toolchain in Docker

If you have Docker, you can use a plugin builder image that I have put together.
This builds the plugin for several Wireshark versions both for Linux and for Windows.

```bash
./build-in-docker.sh
```

## Using the plugin

The build scripts mentioned above install the plugin within the "config" directory.
You can use the `wireshark-native.sh` script which is set up to use that as a
configuration and plugin directory. You can also pass Wireshark arguments to
that script.

```bash
./wireshark-native.sh test/test.pcapng
```
