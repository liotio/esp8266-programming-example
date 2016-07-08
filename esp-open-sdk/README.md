# esp-open-sdk programming example

## Setup

### SDK

Setup the esp-open-sdk as described on it's project readme.
See https://github.com/pfalcon/esp-open-sdk

You will need a linux machine for this, as a windows user, you can use a virtual machine.
Use Ubuntu or Debian if you are not sure which distribution you should choose.
Both are supporting the `apt-get` package manager that is used in the above-named readme.

The most important step is to add an environment variable called `ESP_OPEN_SDK` that is pointing to the esp-open-sdk root directory.
You can do this by adding `export ESP_OPEN_SDK=/path/to/esp-open-sdk` to your `~/.bashrc` file.

### IDE

As an unexperienced user I suggest to use an IDE for programming.
[Eclipse IDE for C/C++ Developers](https://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/neonr) is a good choice.
There are only a few steps that need to be gone through to set up this project

Download, install (Java needed) and open Eclipse.

Afterwars choose `File`, `Import...`, `C/C++`, `Existing Code as Makefile Project`.
Choose an arbitrary `Project Name` and brose to the `Existing Code Location`.
Leave all other fields on their defauls.

Right click the just created project and choose `Properties` at the bottom.
Navigate to `C/C++ General`, `Paths and Symbols` on the left side.
Below the `Includes` tab, add two new `Include directories` by adding `${ProjDirPath}/include` and `${ESP_OPEN_SDK}/sdk/include`
and selecting `Add to all configurations` and `Add to all languages` both times.

As a last step, add all needed `Make Targets` to your project.
To do this, make sure you opened the `Make Targets` view (`Window`, `Show View`, ...), right click your project and click `New...`.
You simply have to type in the names at the top and press `OK`.
You can choose from:

```bash
all          # builds the whole project
clean        # removes build artefacts
rebuild      # clean and all
flash        # flash the firmware to the board
erase_flash  # erase the flash of the board
test         # flash and open a serial monitor
size         # show size of different sections
help         # print all named make options
```

Double click the `all` make target to start compilation.
There should be no errors now.
Wait with executing `flash` until you read the next section!

## Configuration

Makefile, Settings, ...

### PROGRAM

Choose a name for you program.

### FLASH_SIZE

!!!

### FLASH_MODE

Depends on how the flash is connected.
Every flash should support `dio`, but only some support `qio`.
If you are unsure, choose `dio` as flashing with `qio` may render the flash and thus your module useless.

### ESPBAUD

You can choose the baud rate from the following values.
Try the fastest baud rate first, if flashing fails, simply choose the next lower value:

```bash
921600
460800
230400
115200
```

It should not be necessary to choose a lower value than the listed above.

### ESPPORT

...

## Programming

...
