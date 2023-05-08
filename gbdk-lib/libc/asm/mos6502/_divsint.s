;-------------------------------------------------------------------------
;   _divsint.s - routine for division of 16 bit signed int
;
;   Copyright (C) 1998, Ullrich von Bassewitz
;   Copyright (C) 2022, Gabriele Gorla
;
;   This library is free software; you can redistribute it and/or modify it
;   under the terms of the GNU General Public License as published by the
;   Free Software Foundation; either version 2, or (at your option) any
;   later version.
;
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.
;
;   You should have received a copy of the GNU General Public License
;   along with this library; see the file COPYING. If not, write to the
;   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;   MA 02110-1301, USA.
;
;   As a special exception, if you link this library with other files,
;   some of which are compiled with SDCC, to produce an executable,
;   this library does not by itself cause the resulting executable to
;   be covered by the GNU General Public License. This exception does
;   not however invalidate any other reasons why the executable file
;   might be covered by the GNU General Public License.
;-------------------------------------------------------------------------

	.module _divsint

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl __divsint
	.globl ___sdivmod16

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define res "___SDCC_m6502_ret0"
	.define den "__divsint_PARM_2"
	.define rem "___SDCC_m6502_ret2"
	.define s1  "___SDCC_m6502_ret4"
	.define s2  "___SDCC_m6502_ret5"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE

__divsint:
	jsr	___sdivmod16
	lda	*res+0
	ldx	*res+1
	pha
	lda	*s1
	eor	*s2
	bpl	pos
	pla
	jmp 	___negax
pos:
	pla
	rts

___sdivmod16:
	stx	*s1
	jsr	_abs
	pha
	lda     *__divsint_PARM_2+1
	sta	*s2
	bpl	skip
	sec
	lda	#0x00
	sbc	*__divsint_PARM_2+0
        sta     *__divsint_PARM_2+0
	lda	#0x00
	sbc	*__divsint_PARM_2+1
        sta     *__divsint_PARM_2+1
skip:
	pla
	jmp 	___udivmod16

