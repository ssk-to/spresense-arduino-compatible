# Welcome to SPRESENSE Arduino compatible project

Clone this repository and update submodules.

```
$ git clone --recursive git@code.sonymobile.net:spritzer-sdk/spresense-arduino-compatible.git
```

After repositories cloned, each submodules are in 'Detached HEAD'.
So you must checkout master before you getting started.
If you want to all of repository into master, just type like this.

```
$ git submodule foreach git checkout master
```

# Submodules

```
- spresense-core-lib     - SPRESENSE Arduino core library sources
- spresense-prebuilt-sdk - SPRESENSE SDK header and libraries
- spresense-tools        - SPRESENSE Arduino flash tools
- spresense-sketches     - SPRESENSE Arduino sample sketches
- gcc-arm-none-eabi      - Gcc compile tool
```

# Directory structure

```
spresense-arduinolibs
|-- Arduino15
|   |-- packages/SPRESENSE           - SPRESENSE packages
|   |   |-- hardware
|   |   |   `--spresense             - SPRESENSE reference board codes
|   |   |      `-- 1.0.0
|   |   |          |-- cores
|   |   |          |-- libraries
|   |   |          `-- variants
|   |   `-- tools                    - Necessary tool chain and prebuilt binaries
|   |       |-- spresense-tools      - SPRESENSE tools
|   |       |   `-- 1.0.0
|   |       |-- spresense-sdk        - SPRESENSE SDK prebuild binaries
|   |       |   `-- 1.0.0
|   |       `-- gcc-arm-none-eabi    - GCC compiler
|   |           `-- 5.4.1
|   `-- package_spresense_index.json - Arduino IDE configuration json file
|-- sketches                         - Sample Arduino sketches
|   `-- spresense
`-- tools                            - Import/Export tools
```

# How to use
## Import local SDK build

```
./tools/prepare_arduino.sh -H <Arduino IDE Host> -S <SDK build root> -p
```

### Options

| Option | Argument                          | Note                                           |
|-------:|:----------------------------------|:-----------------------------------------------|
| -S     | path/to/spresense-sdk-path        | Local Spresense SDK build root path            |
| -g     | path/to/GCC-archive-path          | GCC archive path                               |
| -s     | path/to/SDK-archive-path          | Prebuilt SDK archive path                      |
| -v     | Board_variant                     | Target board variant (default:spresense)       |
| -k     | release or debug                  | Target kerneo configuration (default: release) |
| -H     | Windows or Linux64 or Mac         | Arduino IDE Host OS                            |
| -M     | "SDK" or "Kernel" or "SDK/Kernel" | Manual configuration by menuconfig             |
| -G     | "SDK" or "Kernel" or "SDK/Kernel" | Manual configuration by gconfig                |
| -Q     | "SDK" or "Kernel" or "SDK/Kernel" | Manual configuration by qconfig                |
| -i     | -                                 | Do not change Kernel/SDK configuration         |
| -p     | -                                 | No network access option                       |

### Example

#### Update SDK prebuilt by using default configuration

```
./tools/prepare_arduino.sh -H Windows -S /home/user1/spresense -k release -v spresense -p
```

#### Update SDK prebuilt by using default configuration and manual configuration change

```
./tools/prepare_arduino.sh -H Windows -S /home/user1/spresense -k release -v spresense -M SDK/Kernel -p
```

* Menu configuration will open twice as 'NuttX Configuration' and 'SDK configuration'

# Tools

Import/Export, download tools instructions are documented at [tools directory](http://code.sonymobile.net/spritzer-sdk/spresense-arduinolibs/tree/master/tools).
