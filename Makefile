export BUILD_TOPDIR=$(PWD)
export STAGING_DIR=/home/a/1980490718/openwrt-sdk-ipq806x-qsdk53/staging_dir
export TOOLPATH=$(STAGING_DIR)/toolchain-arm_cortex-a7_gcc-4.8-linaro_uClibc-1.0.14_eabi/
export PATH:=$(TOOLPATH)/bin:${PATH}
export MAKECMD=make --silent ARCH=arm CROSS_COMPILE=arm-openwrt-linux-

# Copyright (C) 2023 OpenWrt QSDK for IPQ40xx boards
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# SPDX-License-Identifier: GPL-2.0-or-later

# U-Boot Builder for IPQ40xx Boards
# Features:
# - Auto-detects supported boards from config headers
# - Supports single or batch board compilation
# - Provides clean/clean_all targets for maintenance
# - Generates MD5 checksums for built images
# - Creates compressed packages with verification files
#
# Usage Examples:
# make [board1] [board2]  # Build specific boards
# make all                # Build all detected boards
# make clean              # Clean build artifacts
# make clean_all          # Remove all generated files
# make help               # Show usage information

# Board Support:
# - Boards are auto-detected from uboot/include/configs/ipq40xx_*.h
# - Naming convention: ipq40xx_<board_name>.h
# - Output files:
#   * Default board: openwrt-ipq40xx-u-boot-stripped.elf
#   * Others: openwrt-<board_name>-u-boot-stripped.elf
# - Generates corresponding .zip packages

# The boards list is generated dynamically by finding all the ipq40xx_*.h files in the uboot/include/configs directory
BOARDS := $(shell find uboot/include/configs -maxdepth 1 -name 'ipq40xx_*.h' 2>/dev/null | sed 's|.*/ipq40xx_||; s|\.h$$||' | sort)

# boot delay (time to autostart boot command)
export CONFIG_BOOTDELAY=1

# Default target
all: $(BOARDS)

# Help message
help:
	@echo "Usage: make [board-name1] [board-name2 ...]"
	@echo ""
	@echo "Command list:"
	@echo "  all           Build all boards"
	@echo "  clean         Clean build files"
	@echo "  clean_all     Clean build files and remove bin/ products"
	@echo "  help          Show this help message"
	@echo ""
	@echo "Supported boards:"
	@for board in $(BOARDS); do \
		echo "  - $$board"; \
	done

# Board compilation template
define BOARD_template
$(1): 
	@mkdir -p $$(BUILD_TOPDIR)/bin
	@echo "===> Building board: $(1)"
	@cd $$(BUILD_TOPDIR)/uboot/ && $$(MAKECMD) ipq40xx_$(1)_config
	@cd $$(BUILD_TOPDIR)/uboot/ && $$(MAKECMD) ENDIANNESS=-EB V=1 all
	@if [ "$(1)" = "cdp" ]; then \
		cp $$(BUILD_TOPDIR)/uboot/u-boot $$(BUILD_TOPDIR)/bin/openwrt-ipq40xx-u-boot-stripped.elf; \
		$(STAGING_DIR)/host/bin/sstrip $$(BUILD_TOPDIR)/bin/openwrt-ipq40xx-u-boot-stripped.elf; \
		echo "Build completed: openwrt-ipq40xx-u-boot-stripped.elf"; \
		md5sum $$(BUILD_TOPDIR)/bin/openwrt-ipq40xx-u-boot-stripped.elf | tee $$(BUILD_TOPDIR)/bin/openwrt-ipq40xx-u-boot-stripped.elf.md5; \
		echo "===> Creating ZIP package for ipq40xx"; \
		cd $$(BUILD_TOPDIR)/bin && zip -9 -j openwrt-ipq40xx-u-boot-stripped.zip openwrt-ipq40xx-u-boot-stripped.elf openwrt-ipq40xx-u-boot-stripped.elf.md5; \
		echo "Package created: openwrt-ipq40xx-u-boot-stripped.zip"; \
	else \
		cp $$(BUILD_TOPDIR)/uboot/u-boot $$(BUILD_TOPDIR)/bin/openwrt-$(1)-u-boot-stripped.elf; \
		$(STAGING_DIR)/host/bin/sstrip $$(BUILD_TOPDIR)/bin/openwrt-$(1)-u-boot-stripped.elf; \
		echo "Build completed: openwrt-$(1)-u-boot-stripped.elf"; \
		md5sum $$(BUILD_TOPDIR)/bin/openwrt-$(1)-u-boot-stripped.elf | tee $$(BUILD_TOPDIR)/bin/openwrt-$(1)-u-boot-stripped.elf.md5; \
		echo "===> Creating ZIP package for $(1)"; \
		cd $$(BUILD_TOPDIR)/bin && zip -9 -j openwrt-$(1)-u-boot-stripped.zip openwrt-$(1)-u-boot-stripped.elf openwrt-$(1)-u-boot-stripped.elf.md5; \
		echo "Package created: openwrt-$(1)-u-boot-stripped.zip"; \
	fi
endef

# Generate build rules for each board
$(foreach board,$(BOARDS),$(eval $(call BOARD_template,$(board))))

clean:
	@echo "Cleaning u-boot build files..."
	@cd $(BUILD_TOPDIR)/uboot/ && $(MAKECMD) distclean
	@echo "Removing httpd/fsdata.c..."
	@rm -f $(BUILD_TOPDIR)/uboot/httpd/fsdata.c
	@echo "Clean completed"

clean_all:	clean
	@echo Removing all binary images
	@rm -f $(BUILD_TOPDIR)/bin/*.elf
	@rm -f $(BUILD_TOPDIR)/bin/*.zip
	@echo "Clean all completed"