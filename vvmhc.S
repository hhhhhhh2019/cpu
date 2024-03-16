#ifndef VVMHC_H
#define VVMHC_H


#include "MMIO.S"


vvmhc_init:
pushl r1
pushl r10
	loai r10, pc, devices_params.vvmhc - $ + 1

	add r1, r0, 1
	stob r1, r10, VVMHC_STATE_OFFSET

	.loop:
		loab r1, r10, VVMHC_STATE_OFFSET
		add r0, r1, 0
	adde pc, pc, .loop - $
popl r10
popl r1
popl pc


vvmhc_wait:
pushl r10
	loai r10, pc, devices_params.vvmhc - $ + 1

	.loop:
		loab r1, r10, VVMHC_STATE_OFFSET
		and r1, r1, VVMHC_STATE_BUSY

		add r0, r1, 0
	addne pc, pc, .loop - $
popl r10
popl pc


; id: short, count: short, lba: long, dest_addr: long
vvmhc_read_sectors:
pushl r1
pushl r10
	loai r10, pc, devices_params.vvmhc - $ + 1

	stol r0, r10, VVMHC_TP_OFFSET

	loas r1, sp, 8 + 8 + 8 ; id
	stos r1, r10, VVMHC_ID_OFFSET

	loas r1, sp, 8 + 8 + 8 + 2 ; count
	stos r1, r10, VVMHC_BLOCK_COUNT_OFFSET

	loal r1, sp, 8 + 8 + 8 + 2 + 2 ; lba
	stol r1, r10, VVMHC_STARTADDR_OFFSET

	loal r1, sp, 8 + 8 + 8 + 2 + 2 + 8 ; dest_addr
	stol r1, r10, VVMHC_ADDRESS_OFFSET

	add r1, r0, VVMHC_COMMAND_READ
	stob r1, r10, VVMHC_COMMAND_OFFSET

	add r1, pc, vvmhc_wait - $
	call r1
popl r10
popl r1
popl pc


#endif
