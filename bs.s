!============================================================================
        .globl _main,_prints,_color             ! IMPORT symbols
        .globl _getc,_putc,_readfd,_error       ! EXPORT symbols

        mov  ax,#0x9000     ! set ES to 0x9000
        mov  es,ax
        xor  bx,bx          ! clear BX = 0


        xor  dx,dx          ! drive 0, head 0
        xor  cx,cx          ! cyl 0,   sector 0
        incb cl             ! cyl 0, sector 1
        mov  ax, #0x0202    ! READ 1 block
        int  0x13

        jmpi    start,0x9000            ! CS=0x9000, IP=start

start:
        mov     ax,cs                   ! Set segment registers for CPU
        mov     ds,ax                   ! we know ES,CS=0x9000. Let DS=CS
        mov     ss,ax                   ! SS = CS ===> all point at 0x9000
        mov     es,ax
        mov     sp,#8192                ! SP = 8KB above SS=0x9000

        mov     ax,#0x0012              ! 640x480 color
	int     0x10

        call _main                      ! call main() in C

        jmpi 0,0x1000


!======================== I/O functions =================================
       !---------------------------------------
       ! readfd(cyl, head, sector, buf)
       !        4     6     8      10
       !---------------------------------------
_readfd:
        push  bp
	mov   bp,sp            ! bp = stack frame pointer

        movb  dl, #0x00        ! drive 0=FD0
        movb  dh, 6[bp]        ! head
        movb  cl, 8[bp]        ! sector
        incb  cl               ! BIOS count sector from 1
        movb  ch, 4[bp]        ! cyl
        mov   bx, 10[bp]       ! BX=buf ==> memory addr=(ES,BX)
        mov   ax, #0x0202      ! READ 2 sectors to (EX, BX)

        int  0x13              ! call BIOS to read the block
        jb   _error            ! to error if CarryBit is on [read failed]

        pop  bp
	ret

        !---------------------------------------------
        !  char getc()   function: returns a char
        !---------------------------------------------
_getc:
        xorb   ah,ah           ! clear ah
        int    0x16            ! call BIOS to get a char in AX
        ret

        !----------------------------------------------
        ! void putc(char c)  function: print a char
        !----------------------------------------------
_putc:
        push   bp
	mov    bp,sp

        movb   al,4[bp]        ! get the char into aL
        movb   ah,#14          ! aH = 14
!        movb   bl,#0x0D        ! bL = cyan color
        movb   bl, _color
        int    0x10            ! call BIOS to display the char

        pop    bp
	ret

        !------------------------------
        !       error & reboot
        !------------------------------
_error:
        mov  bx, #bad
        push bx
        call _prints

        int  0x19                       ! reboot

bad:    .asciz  "Error!"
