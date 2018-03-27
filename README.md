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
Arduino15
|-- packages/spresense           - SPRESENSE packages
|   |-- hardware
|   |   `--spresense-ext         - SPRESENSE reference board codes
|   |      `-- 1.0.0
|   |          |-- cores
|   |          |-- libraries
|   |          `-- variants
|   |-- tools                    - Necessary tool chain and prebuilt binaries
|   |   |-- spresense-tools      - SPRESENSE tools
|   |   |   `-- 1.0.0
|   |   |-- spresense-sdk        - SPRESENSE SDK prebuild binaries
|   |   |   `-- 2.2
|   |   `-- gcc-arm-none-eabi    - GCC compiler
|   |       `-- 5.4.1
|   `-- sketches                 - Sample Arduino sketches
|       `-- spresense-ext
`package_spresense_index.json    - Arduino IDE configuration json file
```

# How to use
## Import local SDK build

```
./tools/prepare_arduino.sh -H <Arduino IDE Host> -S <SDK build root> -p
```

### Options

| Option | Argument                   | Note                                |
|-------:|:---------------------------|:------------------------------------|
| -H     | Windows or Linux64 or Mac  | Arduino IDE Host OS                 |
| -S     | path/to/spresense-sdk-path | Local Spresense SDK build root path |
| -p     | -                          | Private access option               |

# Tools

Import/Export, download tools instructions are documented at [tools directory](http://code.sonymobile.net/spritzer-sdk/spresense-arduinolibs/tree/master/tools).
