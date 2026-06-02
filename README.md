
# About

Simple application for completing [sBuildInfo](https://github.com/silvio3105/sBuildInfo) struct after embedded application build. Application writes `--file` size and calculated checksum.
It is possible to add pre and/or post salt string to checksum calculation for additional security during device update.

Application is built for 64-bit Windows 10 or newer using GCC 16.1.0 and MinGW-w64 14.0.0.


# Checksum algorithms

For now only Modbus CRC16 is supported.


# Salt

It is possible to add pre and/or post salt to checksum calculation. Salt is used to prevent installing unoffical application onto device.


# Arguments

List of supported arguments

| Argument 					| Description 												|
| --- 						| --- 														|
| --file * 					| Path to `.bin` file to process							|
| --checksum-offset *		| Offset of checksum bytes in `--file` in bytes. In hex		|
| --size-offset	*			| Offset of size bytes in `--file` in bytes. In hex			|
| --big-endian				| Set output format to big endian							|
| --salt-pre				| Salt string to process before `--file` 					|
| --salt-post 				| Salt string to process after `--file`						|
| --algorithm				| Checksum algorithm. Modbus CRC16 is used if not provided 	|
| --alignment				| `--file` size must be divisible with number given with `--alignment`. Check is skipped if not provided or `0` provided. |

*: Required


# Example

`sBuildProbe --file fw.bin --checksum-offset 100 --size-offset FC --salt-pre Hello --salt-post World --alignment 4`

Application will write build info to `fw.bin` file. Checksum word is offseted `0x100` bytes. Size word is offseted `0xFC` bytes. Output is in little endian and Modbus CRC16 is used for checksum algorithm. File size must be divisible by 4(32-bit).

If `--alignment` is provided(and not `0`) file size will be checked. If size is not divisible, program will exit.
Before calculating checksum file size will be written at `--size-offset`.

Before processing data from input file, pre salt(if provided) `Hello` will be added in calculation. After file is processed, post salt(if provided) `World` will be added.
Calculated checksum is written at `--checksum-offset`. Checksum word at `--checksum-offset` is skipped.


---

Copyright (c) 2026, silvio3105
