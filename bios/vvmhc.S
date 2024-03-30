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


; id: short
vvmhc_get_disks_count:
pushl r10
pushl r2
pushl r3

	add r10, pc, devices - $
	loas r1, sp, 8 + 8 + 8 + 8
	mul r1, r1, 1 + 2 + 4 + 4
	add r10, r10, r1

	loai r10, r10, 1 + 2 + 4 ; device MMIO offset

	add r1, r0, VVMHC_COMMAND_GET_CONNECTED
	stob r1, r10, VVMHC_COMMAND_OFFSET

	loal r2, r10, VVMHC_RESPONSE_OFFSET
	add r3, r0, 16

	or r1, r0, r0

	.loop:
		and r10, r2, 1
		add r1, r1, r10
		shr r2, r2, 1
	sub r3, r3, 1
	addne pc, pc, .loop - $
popl r3
popl r2
popl r10
popl pc


; id: short, disk_id: char
vvmhc_get_slot_by_id:
pushl r10
pushl r2
pushl r3

	add r10, pc, devices - $
	loas r1, sp, 8 * 4
	mul r1, r1, 1 + 2 + 4 + 4
	add r10, r10, r1

	loai r10, r10, 1 + 2 + 4 ; device MMIO offset

	loab r3, sp, 8 * 5 + 2 ; disk_id

	add r1, r0, VVMHC_COMMAND_GET_CONNECTED
	stob r1, r10, VVMHC_COMMAND_OFFSET

	loal r2, r10, VVMHC_RESPONSE_OFFSET

	or r1, r0, r0

	.loop:
		and r10, r2, 1
		shr r2, r2, 1
		add r1, r1, r10
		sub r3, r3, r10
	add r0, r2, r0
	adde pc, pc, .return - $
	add r0, r3, r0
	addne pc, pc, .loop - $

	.return:
popl r3
popl r2
popl r10
popl pc


#endif
