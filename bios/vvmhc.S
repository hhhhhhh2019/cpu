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

	loai r10, r10, 1 + 2 + 4 ; VVMHC MMIO offset

	add r1, r0, 1
	stob r1, r10, VVMHC_STATE_OFFSET

	.loop:
		loab r1, r10, VVMHC_STATE_OFFSET
	add r0, r0, r1
	adde pc, pc, .loop - $

popl r1
popl r10
popl pc


vvmhc_wait:
popl pc


; id: short, count: short, lba: long, dest_addr: long
vvmhc_read_sectors:
pushl r1
pushl r10
popl r10
popl r1
popl pc


#endif
