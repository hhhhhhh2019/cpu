#ifndef DISKS_H
#define DISKS_H


#include "vvmhc.S"


; id: short, disk_id: short, count: short, lba: long, dest_addr: long
read_sectors:
pushl r10
pushl r1

	add r10, pc, devices - $
	loas r1, sp, 8 + 8 + 8
	mul r1, r1, 1 + 2 + 4 + 4
	add r10, r10, r1

	loas r10, r10, 1 ; device type

	add r0, r10, VVMHC_TYPE
	adde pc, pc, .vvmhc - $

	add pc, pc, .exit - $

	.vvmhc:
		loal r1, sp, 8 * 3 + 2 + 2 + 2 + 8 ; dest_addr
		pushl r1
		loal r1, sp, 8 * 3 + 2 + 2 + 2 ; lba
		pushl r1
		loas r1, sp, 8 * 3 + 2 + 2 ; count
		pushs r1
		loas r1, sp, 8 * 3 + 2 ; disk_id
		pushs r1
		loas r1, sp, 8 * 3 ; id
		pushs r1

		add r1, pc, vvmhc_read_sectors - $
		call r1

		add sp, sp, 2 + 2 + 2 + 8 + 8
	add pc, pc, .exit - $

.exit:
popl r1
popl r10
popl pc


#endif
