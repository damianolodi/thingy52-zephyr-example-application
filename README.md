# Thingy52 Example Application

This repository provides a template for a Zephyr-based application for Nordic's Thingy52.

The project is designed to be built using the command line. VSCode configuration is provided for debuggers and some C-C++ specific tools. 

It can be a good starting point for people who want to approach embedded programming without the use of an IDE.

## Table of Contents

1. [About the Project](#about-the-project)
2. [Project Status](#project-status)
3. [Getting Started](#getting-started)
    1. [Dependencies](#dependencies)
    2. [Building](#building)
    3. [Usage](#usage)
4. [Release Process](#release-process)
5. [How to Get Help](#how-to-get-help)
6. [Further Reading](#further-reading)
7. [Contributing](#contributing)
8. [License](#license)
9. [Authors](#authors)
10. [Acknowledgments](#acknowledgements)

## About the Project

WIP

**[Back to top](#table-of-contents)**

## Project Status

This project is under active development. It is currently used by myself to explore embedded programming.

**[Back to top](#table-of-contents)**

## Getting Started

In the following sections you will find instructions to set-up and use the project.

### Dependencies

The project dependencies are minimal since most of the required software/SDK will be downloaded during setup.

Since the project is based on the Zephyr RTOS, the user should follow the [Zephyr's Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html). The important paragpraph of the guide are *[Install Dependencies](https://docs.zephyrproject.org/latest/develop/getting_started/index.html#install-dependencies)* and *[Install Zephyr SDK](https://docs.zephyrproject.org/latest/develop/getting_started/index.html#install-zephyr-sdk)* since all the other dependencies will be downloaded during project setup in a Python virutal environment.

> [!warning]
> This project is developed and tested only on Ubuntu. It will probably run smoothly on macOS, but I have limited experience in CMake development on Windows.

The user should also intall `Make`. On Ubuntu, this is easily done using the command line:

```bash
sudo apt install make
```

Finally, some hardware is required to run the project correctly:

- One Thingy52.
- One Nordic's DK that will be used as a debugger (I use the nRF52840 DK).
- A SWD cable ([like the one provided by Adafruit](https://www.adafruit.com/product/1675)).

### Getting the Source

The source code of this project is [stored on Github](https://github.com/damianolodi/thingy52-zephyr-example-application). If you are interested in this project I assume you have some experience with the command line.

The best way to get the project and all the required dependecies is cloning the repository using Zephyr's `west` tool: 

```bash
# Clone the respository and setup the project structure correctly
west init -m https://github.com/damianolodi/thingy52-zephyr-example-application.git --mr v1.0.0 thingy52-zephyr-example
```

Once the project is cloned, move into the created directory and create a *Python virtual environment*.

```bash
cd thingy52-zephyr-example

# Create the virutal environment
python3 -m venv .venv
```

Finally, activate the viruatl environment and use `west` to install all the project dependencies into that environment.

```bash
# Activate the virtual environment
source .venv/bin/activate

# Download all the project dependencies
west update
```

> [!note]
> From now on, every time you work on the project you need to activate that virtual environment.

#### A Note on Virutal Environments

Using the virutal environement for this project is not strictly necessary. The main reason why it is used is for cross-compatibility with other Zephyr-based projects that use a different RTOS version.

Different RTOS version could have different dependencies. Using virutal environments one isolates dependencies on a project basis and increase compatibility. For this reason, if you plan to work on other Zephyr apps in the future, I strongly suggest to use virutal environments.

### Building

The project build system is based on CMake. Nonetheless, a Makefile is provided as an helper tool to build and debug the application.

The Makefile is commented and all the available targets are described calling `make help`.

To build the project type

```bash
make
```

The project will be built selecting the Thingy52 as the target board and in Debug mode. The default build directory is `_build`, but it can be changed either by changing the Makfile or by providing the `BUILDRESULT=<dir_name>` flag when calling Make.

The helper commands `make dconfig` and `make rconfig` will change the build configuration into *Debug* and *Release* respectively.

Finally, `make clean` will clean all the build artifacts while `make pristine` will delete the build folder completely.

### Usage

Once built, the app can be flashed using `make flash`. Before calling the target, connect the Thingy52 to the DK using the SWD cable and connect the DK to the PC using and USB cable. Both boards must be powered on.

#### Debugging

The target `make debug` can be used to debug the application. Nonetheless, I find easier using VSCode for this task. For this reason, the project provide a complete VSCode debug configuration.

> [!important]
> To run the debugger successfully, activate the Python virtual environment and open VSCode using `code <path-to-project>` from the terminal. This will set correctly all the envoronment variables for VSCode.

You need the following extensions:

- C/C++ (Microsoft)
- Cortex-Debug (marus25)

All the debug configurations are stored in the `.vscode/launch.json` file. To run the debugger go into the debugger tab, select the right configuration and press F5.

> [!important]
> At the current version, only the "JLink nRF52832 (RTT)" configuration is tested.

**[Back to top](#table-of-contents)**

## Release Process

The git branching strategy is based on [git flow](https://danielkummer.github.io/git-flow-cheatsheet/). Releases are on the `main` branch and will be tagged using the `vX.Y.Z` format.

I will push also the `develop` branch, in which my day-to-day experiments are stored. I try to push and commit only working code, but I am not enforcig it with any automation so YMMV.

**[Back to top](#table-of-contents)**

## How to Get Help

If you need help please contact me through one of my social media or through my website. I will try to help and clarify any doubts.

Please use the issue trucker only for bugs.

## Contributing

At the time of writing, I do not accept external contribution or pull requests. This is a personal project and I simply want to share it with other enthusiasts.

Nonethelss, feel free to contact me about any idea you have for a possible development or improvement in the project. I will be happy to discuss anything with you.

**[Back to top](#table-of-contents)**

## Further Reading

Some related documentation you can find useful:

- [Zephyr documentation](https://docs.zephyrproject.org/latest/index.html)
- [nRF Connect SDK documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/introduction.html)
- [CMake documentation](https://cmake.org/cmake/help/latest/)

**[Back to top](#table-of-contents)**

## License

Copyright (c) 2023 Damiano Lodi

This project is licensed under the MIT License - see [LICENSE.md](LICENSE.md) file for details.

**[Back to top](#table-of-contents)**

## Authors

* **[Damiano Lodi](https://github.com/damianolodi)** - *Initial work*

**[Back to top](#table-of-contents)**

## Acknowledgments

The current README is based on the template from [Embedded Artistry Template](https://github.com/embeddedartistry/templates/blob/master/oss_docs/README.md).

The *Makefile* shim and CMake project structures are inspired by [Embedded Artistry's CMake course](https://embeddedartistry.com/course/creating-a-cross-platform-build-system-for-embedded-projects-with-cmake/).

**[Back to top](#table-of-contents)**
