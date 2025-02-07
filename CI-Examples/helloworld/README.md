# Hello World

This directory contains a Makefile and a manifest template for running a simple
"Helloworld" sandboxed program supported by [Erebor](https://github.com/ASTERISC-Release/Erebor).

## Description

This is a simple demo program, without any client input.
This program's output will be `'AAAAAAAA' (0x414141...41)`.

You will see the prompt when executing this program.
> [!NOTE]
> For testing purpose, sandboxes are allowed to print stats to the VM console.

## Building

Run `make` in the directory.

## Execution

```sh
gramine-encos helloworld
```

## See output

```sh
sudo cat /sys/kernel/debug/encos-output-emulate/out
```