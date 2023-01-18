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

BUILD_TYPE ?=
ifneq ($(BUILD_TYPE),)
	INTERNAL_OPTIONS += -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
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
	$(Q) if [ -d "$(BUILDRESULTS)" ]; then ninja -C $(BUILDRESULTS) pristine; fi

########################
# File display helpers #
########################

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
	@echo "  default: 	builds all default targets ninja knows about"
	@echo "  flash: 	flash the current board"
	@echo "  cc: 		call ccmake in the build directory"
	@echo "  dconfig: 	change build configuration to Debug"
	@echo "  rconfig: 	change build configuration to Release"
	@echo "  reconfig: 	reconfigure an existing build output folder with new settings"
	@echo "  clean: 	cleans build artifacts, keeping build files in place"
	@echo "  pristine: 	removes the configured build output directory. Keep the log file"
	@echo "  dts:		open the compiled devicetree file for the selected board"
