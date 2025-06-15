#include <common.h>
#include "ipq40xx_api.h"
#include "ipq40xx_cdp.h"
#include <command.h>

extern int do_checkout_firmware(void);
extern board_ipq40xx_params_t *gboard_param;
extern int openwrt_firmware_start;
extern int openwrt_firmware_size;

static void print_size_info(const char *prefix, unsigned int size) {
	if (size >= (1024 * 1024)) {
		unsigned int mb = size / (1024 * 1024);
		unsigned int remainder = size % (1024 * 1024);
		printf("%s%d.%02d MB (%u bytes)", prefix, mb, (remainder*100)/(1024*1024), size);
	} else if (size >= 1024) {
		unsigned int kb = size / 1024;
		unsigned int remainder = size % 1024;
		printf("%s%d.%02d KB (%u bytes)", prefix, kb, (remainder*100)/1024, size);
	} else {
		printf("%s%u bytes", prefix, size);
	}
}
static void format_size_string(char *buffer, size_t buffer_size, unsigned int size) {
	if (size >= (1024 * 1024)) {
		unsigned int mb = size / (1024 * 1024);
		unsigned int remainder = size % (1024 * 1024);
		snprintf(buffer, buffer_size, "%d.%02d MB (%u bytes)",
			mb, (remainder*100)/(1024*1024), size);
	} else if (size >= 1024) {
		unsigned int kb = size / 1024;
		unsigned int remainder = size % 1024;
		snprintf(buffer, buffer_size, "%d.%02d KB (%u bytes)",
			kb, (remainder*100)/1024, size);
	} else {
		snprintf(buffer, buffer_size, "%u bytes", size);
	}
}
static const char* get_board_type_string(void) {
	switch (gboard_param->machid) {
		case MACH_TYPE_IPQ40XX_AP_DK04_1_C1:
			return "IPQ40XX_AP_DK04_1_C1";
		case MACH_TYPE_IPQ40XX_AP_DK04_1_C3:
			return "IPQ40XX_AP_DK04_1_C3";
		case MACH_TYPE_IPQ40XX_AP_DK01_1_C1:
			return "IPQ40XX_AP_DK01_1_C1";
		case MACH_TYPE_IPQ40XX_AP_DK01_1_C2:
			return "IPQ40XX_AP_DK01_1_C2";
		case MACH_TYPE_IPQ40XX_AP_DK01_AP4220:
			return "IPQ40XX_AP_DK01_AP4220";
		default:
			return "Unknown";
	}
}
// Global variables for web interface
static int firmware_loaded_to_ram = 0;
static unsigned int last_firmware_size = 0;
static unsigned int last_firmware_start = 0;
int read_firmware(void) {
	char cmd[128];
	int fw_type = do_checkout_firmware();
	switch (gboard_param->machid) {
		case MACH_TYPE_IPQ40XX_AP_DK04_1_C1:
		case MACH_TYPE_IPQ40XX_AP_DK04_1_C3:
			if (fw_type == FW_TYPE_OPENWRT_EMMC) {
				printf("Reading %d.%02d MB (%u bytes) from eMMC flash at offset 0x%x\n",
					openwrt_firmware_size/(1024*1024), (openwrt_firmware_size%(1024*1024)*100)/(1024*1024), openwrt_firmware_size, openwrt_firmware_start);
				unsigned long blocks = (openwrt_firmware_size + 511) / 512;
				snprintf(cmd, sizeof(cmd),
					"mmc read 0x88000000 0x%x 0x%lx", openwrt_firmware_start, blocks);
			} else {
				printf("Reading %d.%02d MB (%u bytes) from SPI flash at offset 0x%x\n",
					openwrt_firmware_size/(1024*1024), (openwrt_firmware_size%(1024*1024)*100)/(1024*1024), openwrt_firmware_size, openwrt_firmware_start);
				snprintf(cmd, sizeof(cmd),
					"sf probe && sf read 0x88000000 0x%x 0x%x", openwrt_firmware_start, openwrt_firmware_size);
			}
			break;
		case MACH_TYPE_IPQ40XX_AP_DK01_1_C1:
			printf("Reading %d.%02d MB (%u bytes) from SPI flash at offset 0x%x\n",
				openwrt_firmware_size/(1024*1024), (openwrt_firmware_size%(1024*1024)*100)/(1024*1024), openwrt_firmware_size, openwrt_firmware_start);
			snprintf(cmd, sizeof(cmd),
				"sf probe && sf read 0x88000000 0x%x 0x%x", openwrt_firmware_start, openwrt_firmware_size);
			break;
		case MACH_TYPE_IPQ40XX_AP_DK01_1_C2:
		case MACH_TYPE_IPQ40XX_AP_DK01_AP4220:
			printf("Reading %d.%02d MB (%u bytes) from NAND flash at offset 0x%x\n",
				openwrt_firmware_size/(1024*1024), (openwrt_firmware_size%(1024*1024)*100)/(1024*1024), openwrt_firmware_size, openwrt_firmware_start);
			snprintf(cmd, sizeof(cmd),
				"nand device 1 && nand read 0x88000000 0x%x 0x%x", openwrt_firmware_start, openwrt_firmware_size);
			break;
		default:
			printf("Error: Unsupported board type!\n");
			return -1;
	}
	printf("Loading firmware to RAM... ");
	int ret = run_command(cmd, 0);
	if (ret == 0) {
		printf("Success: Loaded 0x%x-0x%x to RAM at 0x88000000 - ", openwrt_firmware_start, openwrt_firmware_start + openwrt_firmware_size - 1);
			print_size_info("", openwrt_firmware_size);
			// Set firmware loaded flag and save parameters
			firmware_loaded_to_ram = 1;
			last_firmware_size = openwrt_firmware_size;
			last_firmware_start = openwrt_firmware_start;
			printf("\n");
			return 0;
		} else {
			printf("Error: Failed to load firmware (code:%d)\n", ret);
			// Reset firmware loaded flag on failure
			firmware_loaded_to_ram = 0;
			return -1;
	}
}
int do_readfw(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return read_firmware();
}
U_BOOT_CMD(
	readfw, 1, 0, do_readfw,
	"Read firmware image into RAM",
	"\nUsage: readfw\n"
	"Read firmware from flash storage into RAM at 0x88000000\n"
	"for recovery or upgrade purposes."
);
int do_backupfw(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (read_firmware() != 0) {
		return CMD_RET_FAILURE;
	}
	char cmd[128];
	printf("\n");
	snprintf(cmd, sizeof(cmd), 
		"tftpput 0x88000000 0x%x firmware_backup.bin", openwrt_firmware_size);
	if (run_command(cmd, 0) == 0) {
		printf("Success: Firmware backup completed\n");
		printf("Backup filename: firmware_backup.bin\n");
		return CMD_RET_SUCCESS;
	}
	printf("Error: Firmware backup failed\n");
	return CMD_RET_FAILURE;
}
U_BOOT_CMD(
	backupfw, 1, 0, do_backupfw,
	"Backup firmware via TFTP",
	"\nUsage: backupfw\n"
	"Read firmware from flash to RAM and transfers it via TFTP\n"
	"Requires TFTP server to be running on the network"
);
// Web interface function to handle firmware read request
int web_handle_read(char *response_buffer, size_t buffer_size) {
	int result = read_firmware();
	char size_str[64];

	if (result == 0) {
		format_size_string(size_str, sizeof(size_str), openwrt_firmware_size);
		snprintf(response_buffer, buffer_size,
			"Success: 固件读取成功\n"
			"Size: %s\n"
			"Address: 0x%x-0x%x\n"
			"RAM: 0x88000000\n"
			"Board: %s",
			size_str,
			openwrt_firmware_start,
			openwrt_firmware_start + openwrt_firmware_size - 1,
			get_board_type_string()
		);
	} else {
		snprintf(response_buffer, buffer_size,
			"Error: 固件读取失败\n"
			"请检查存储设备连接状态"
		);
	}

	return result;
}
// Web interface function to handle firmware download request
int web_handle_download(unsigned char **firmware_data, unsigned int *firmware_size) {
	if (!firmware_loaded_to_ram) {
		printf("Error: No firmware loaded in RAM\n");
		return -1;
	}

	*firmware_data = (unsigned char *)0x88000000;
	*firmware_size = last_firmware_size;

	printf("Preparing firmware download: %u bytes from RAM address 0x88000000\n", last_firmware_size);
	return 0;
}
// Check if firmware is loaded in RAM
int is_firmware_loaded(void) {
	return firmware_loaded_to_ram;
}
// Get loaded firmware size
unsigned int get_loaded_firmware_size(void) {
	return firmware_loaded_to_ram ? last_firmware_size : 0;
}
