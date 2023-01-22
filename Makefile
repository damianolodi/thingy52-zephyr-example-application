VERBOSE ?= 0

ifeq ($(VERBOSE),1)
export Q :=
export VERBOSE := 1
else
export Q := @
export VERBOSE := 0
endif

export CMAKE_GENERATOR=Ninja

BUILDRESULTS ?= _build
CONFIGURED_BUILD_DEP = $(BUILDRESULTS)/build.ninja

OPTIONS ?=
INTERNAL_OPTIONS =

BUILD_TYPE ?= Debug
ifneq ($(BUILD_TYPE),)
	INTERNAL_OPTIONS += -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
endif

BOARD ?= 
ifneq ($(BOARD),)
	INTERNAL_OPTIONS += -DBOARD=$(BOARD)
endif

all: default

.PHONY: default
default: | $(CONFIGURED_BUILD_DEP)
	$(Q)ninja -C $(BUILDRESULTS)

.PHONY: debug
debug: | $(CONFIGURED_BUILD_DEP)
	$(Q)ninja -C $(BUILDRESULTS) debug

.PHONY: flash
flash: default
	$(Q)ninja -C $(BUILDRESULTS) flash

# Runs whenever the build has not been configured successfully
$(CONFIGURED_BUILD_DEP):
	$(Q)cmake -B $(BUILDRESULTS) $(OPTIONS) $(INTERNAL_OPTIONS)

.PHONY: cc
cc:
	$(Q)ccmake $(BUILDRESULTS)

# Reconfigure the build type to Release
.PHONY: rconfig
rconfig:
	make reconfig BUILD_TYPE=Release

# Reconfigure the build type to Debug
.PHONY: dconfig
dconfig:
	make reconfig BUILD_TYPE=Debug

# Manually reconfigure a target, esp. with new options
.PHONY: reconfig
reconfig:
	$(Q) cmake -B $(BUILDRESULTS) $(OPTIONS) $(INTERNAL_OPTIONS)

.PHONY: clean
clean:
	$(Q) if [ -d "$(BUILDRESULTS)" ]; then ninja -C $(BUILDRESULTS) clean; fi

.PHONY: pristine
pristine:
	$(Q) rm -rf $(BUILDRESULTS)

#################
# Board targets #
#################

.PHONY: thingy52
thingy52: pristine
	$(Q)make BOARD="thingy52_nrf52832"

.PHONY: nrf52840dk
nrf52840dk: pristine
	$(Q)make BOARD="nrf52840dk_nrf52840"

###################
# Utility targets #
###################

# Open the board compiled devicetree file
.PHONY: dts
dts:
	$(Q)code $(BUILDRESULTS)/zephyr/zephyr.dts

.PHONY : help
help :
	@echo "usage: make [OPTIONS] <target>"
	@echo "  Options:"
	@echo "    > VERBOSE Show verbose output for Make rules. Default 0. Enable with 1"
	@echo "    > BUILDRESULTS Directory for build results. Default _build"
	@echo "    > OPTIONS Configuration options to pass to a build. Default empty"
	@echo "    > BUILD_TYPE Specify the build type (default: RelWithDebInfo)"
	@echo "			Option are: Debug Release MinSizeRel RelWithDebInfo"
	@echo "Targets:"
	@echo "  Main:"
	@echo "    default: 	builds all default targets ninja knows about"
	@echo "    flash: 	flash the current board"
	@echo "    clean: 	cleans build artifacts, keeping build files in place"
	@echo "    pristine: 	removes the configured build output directory. Keep the log file"
	@echo "  Configuration:"
	@echo "    cc: 	call ccmake in the build directory"
	@echo "    dconfig: 	change build configuration to Debug"
	@echo "    rconfig: 	change build configuration to Release"
	@echo "    reconfig: 	reconfigure an existing build output folder with new settings"
	@echo "  Helpers:"
	@echo "    thingy52:	pristine build using BOARD=thingy52_nrf52832"
	@echo "    nrf52840dk:	pristine build using BOARD=nrf52840dk_nrf52840"
	@echo "    dts:	open the compiled devicetree file for the selected board"
