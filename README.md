
# About

Simple tool for completing [sBuildInfo](https://github.com/silvio3105/sBuildInfo) struct after embedded application build. Application writes `--file` size and calculated hash to provided location in `--file`.
It is possible to add pre file and/or post file salt string to hash calculation for additional layer of security before device update.

Application is built for 64-bit Windows 10 or newer using GCC 16.1.0 and MinGW-w64 14.0.0.


# Hash algorithms

For now only Modbus CRC is supported.


# Salt

It is possible to add pre file data and/or post file data salt to hash calculation. Salt is used to prevent updating device with unoffical application.


# Arguments

List of supported arguments

| Argument 					| Description 												|
| --- 						| --- 														|
| --file * 					| Path to `.bin` file to process							|
| --hash-offset *			| Offset of hash bytes in `--file` in bytes(eg. 0x4)		|
| --size-offset	*			| Offset of size bytes in `--file` in bytes(eg. 0x8)		|
| --big-endian				| Set output format to big endian							|
| --salt-pre				| Salt string to process before `--file` data				|
| --salt-post 				| Salt string to process after `--file`	data				|
| --algorithm				| Algorithm to use. Modbus CRC is used if not provided 		|
| --alignment				| `--file` size must be divisible with number given with `--alignment`. Check is skipped if not provided or `0` is provided. |
| --help, -h				| List of all supported options and flags					|
| --version, -v				| Application version										|

*: Required


# Example

`.\sPostBuild --file fw.bin --hash-offset 0x100 --size-offset 0xFC --salt-pre Hello --salt-post World --alignment 4`

Application will write build info to `fw.bin` file. Hash word is offset `0x100` bytes. Size word is offset `0xFC` bytes. Output is in little endian and Modbus CRC16 is used for hash algorithm. File size must be divisible by 4(32-bit).

If `--alignment` is provided(and not `0`) file size will be checked. If size is not divisible with number provided with `--alignment`, application will exit.
Before calculating hash file size will be written at `--size-offset`.

Before processing data from input file, pre salt `Hello` will be added in calculation. After file is processed, post salt `World` will be added.
Calculated hash is written at `--hash-offset`. hash word at `--hash-offset` is skipped.


---

Copyright (c) 2026, silvio3105
