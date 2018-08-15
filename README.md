# Welcome to Spresense Arduino compatible project

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
- spresense-core-lib     - Spresense Arduino core library sources
- spresense-prebuilt-sdk - Spresense SDK header and libraries
- spresense-tools        - Spresense Arduino flash tools
- spresense-sketches     - Spresense Arduino sample sketches
- gcc-arm-none-eabi      - Gcc compile tool
```

# Directory structure

```
spresense-arduinolibs
|-- Arduino15
|   |-- packages/SPRESENSE           - Spresense packages
|   |   |-- hardware
|   |   |   `--spresense             - Spresense reference board codes
|   |   |      `-- 1.0.0
|   |   |          |-- cores
|   |   |          |-- libraries
|   |   |          `-- variants
|   |   `-- tools                    - Necessary tool chain and prebuilt binaries
|   |       |-- spresense-tools      - Spresense tools
|   |       |   `-- 1.0.0
|   |       |-- spresense-sdk        - Spresense SDK prebuild binaries
|   |       |   `-- 1.0.0
|   |       `-- gcc-arm-none-eabi    - GCC compiler
|   |           `-- 5.4.1
|   `-- package_spresense_index.json - Arduino IDE configuration json file
`-- tools                            - Import/Export tools
```

# Getting started
### [developer_guide_arduino_en (English)](https://github.com/sonydevworld/spresense-docs/blob/master/developer_guide/arduino/developer_guide_arduino_en.adoc)

### [developer_guide_arduino_ja (日本語)](https://github.com/sonydevworld/spresense-docs/blob/master/developer_guide/arduino/developer_guide_arduino_ja.adoc)

# How to prepare Arduino environment
## Pull or Import GCC and Prebuilt SDK

```
./tools/prepare_arduino.sh [OPTIONS...]
```

### Options

#### For using local archive

| Option | Argument                          | Note                                           |
|-------:|:----------------------------------|:-----------------------------------------------|
| -g     | path/to/GCC-archive-path          | GCC archive path                               |
| -s     | path/to/SDK-archive-path          | Prebuilt SDK archive path                      |

#### For using local source code

| Option | Argument                          | Note                                           |
|-------:|:----------------------------------|:-----------------------------------------------|
| -S     | path/to/spresense-sdk-path        | Local Spresense SDK build root path            |
| -v     | Board_variant                     | Target board variant (default:spresense)       |
| -k     | release or debug                  | Target kerneo configuration (default: release) |
| -M     | "SDK" or "Kernel" or "SDK/Kernel" | Manual configuration by menuconfig             |
| -G     | "SDK" or "Kernel" or "SDK/Kernel" | Manual configuration by gconfig                |
| -Q     | "SDK" or "Kernel" or "SDK/Kernel" | Manual configuration by qconfig                |
| -i     | -                                 | Do not change Kernel/SDK configuration         |	

#### Other option

| Option | Argument                          | Note                                           |
|-------:|:----------------------------------|:-----------------------------------------------|
| -H     | Windows or Linux64 or Mac         | Arduino IDE Host OS                            |
| -p     | -                                 | No network access option                       |
| -h     | -                                 | Show help                                      |

### Example

#### Pull GCC and Spresense SDK prebuilt from network

```
./tools/prepare_arduino.sh
```

#### Pull GCC and Spresense SDK prebuilt from local archive

```
./tools/prepare_arduino.sh -s path/to/spresense-sdk.tar.gz -g path/to/gcc-arm-none-eabi-5.4.1-linux.tar.gz
```

#### Update SDK prebuilt by using default configuration

```
./tools/prepare_arduino.sh -H Windows -S /home/user1/spresense -k release -v spresense -p
```

#### Update SDK prebuilt by using default configuration and manual configuration change

```
./tools/prepare_arduino.sh -H Windows -S /home/user1/spresense -k release -v spresense -M SDK/Kernel -p
```

* Menu configuration will open twice as 'NuttX Configuration' and 'SDK configuration'

