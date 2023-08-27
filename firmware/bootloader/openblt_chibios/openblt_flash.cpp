#include "pch.h"
#include "flash_int.h"

extern "C" {
	#include "boot.h"
	#include "flash.h"
}

void FlashInit() { }

blt_bool FlashVerifyChecksum() {
	// Naive check: if the first block is blank, there's no code there
	return intFlashIsErased(FlashGetUserProgBaseAddress(), 4) ? BLT_FALSE : BLT_TRUE;
}

// Bootloader size imported from the linker script
extern uint32_t bootloader_size;

blt_addr FlashGetUserProgBaseAddress() {
	return 0x08000000 + bootloader_size;
}

blt_bool FlashWrite(blt_addr addr, blt_int32u len, blt_int8u *data) {
	return (FLASH_RETURN_SUCCESS == intFlashWrite(addr, (const char*)data, len)) ? BLT_TRUE : BLT_FALSE;
	
	return BLT_TRUE;
}

blt_bool FlashErase(blt_addr addr, blt_int32u len) {
	if (!intFlashIsErased(addr, len)) {
		return (FLASH_RETURN_SUCCESS == intFlashErase(addr, len)) ? BLT_TRUE : BLT_FALSE;
	}

	return BLT_TRUE;
}

blt_bool FlashDone() {
	return BLT_TRUE;
}

blt_bool FlashWriteChecksum() {
	return BLT_TRUE;
}
