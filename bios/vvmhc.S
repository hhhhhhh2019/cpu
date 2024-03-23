#ifndef VVMHC_H
#define VVMHC_H


#include "devices.S"


; id: short
init_vvmhc:
pushl r10
pushl r1

	add r10, pc, devices - $
	loas r1, sp, 8 + 8 + 8
	mul r1, r1, 1 + 2 + 4 + 4
	add r10, r10, r1

	loai r10, r10, 1 + 2 + 4 ; device MMIO offset

	add r1, r0, 1
	stob r1, r10, VVMHC_STATE_OFFSET

	.loop:
		loab r1, r10, VVMHC_STATE_OFFSET
	add r0, r0, r1
	adde pc, pc, .loop - $

popl r1
popl r10
popl pc


; id: short
vvmhc_wait:
pushl r10
pushl r1

	add r10, pc, devices - $
	loas r1, sp, 8 + 8 + 8
	mul r1, r1, 1 + 2 + 4 + 4
	add r10, r10, r1

	loai r10, r10, 1 + 2 + 4 ; device MMIO offset

	.loop:
		loab r1, r10, VVMHC_STATE_OFFSET
		and r1, r1, VVMHC_STATE_BUSY
		add r0, r1, r0
	addne pc, pc, .loop - $

popl r1
popl r10
popl pc


; id: short, disk_id: short, count: short, lba: long, dest_addr: long
vvmhc_read_sectors:
pushl r1
pushl r10

	add r10, pc, devices - $
	loas r1, sp, 8 + 8 + 8
	mul r1, r1, 1 + 2 + 4 + 4
	add r10, r10, r1

	loai r10, r10, 1 + 2 + 4 ; device MMIO offset

	loas r1, sp, 8 + 8 + 8 + 2 ; disk_id
	stob r1, r10, VVMHC_ID_OFFSET

	loas r1, sp, 8 + 8 + 8 + 2 + 2 ; count
	stob r1, r10, VVMHC_BLOCK_COUNT_OFFSET

	loal r1, sp, 8 + 8 + 8 + 2 + 2 + 2 ; lba
	stol r1, r10, VVMHC_ADDRESS_OFFSET

	loal r1, sp, 8 + 8 + 8 + 2 + 2 + 2 + 8 ; dest_addr
	stol r1, r10, VVMHC_STARTADDR_OFFSET

	add r1, r0, VVMHC_COMMAND_READ
	stob r1, r10, VVMHC_COMMAND_OFFSET

popl r10
popl r1
popl pc


#endif