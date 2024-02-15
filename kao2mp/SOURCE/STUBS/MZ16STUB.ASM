;;
;; "MZ16STUB.ASM"
;; Instructions to generate a flat binary file for the "Netwide Assembler"
;;

cpu 8086

    dw 'MZ'       ; [0x0000]  WORD Signature ; zibo

    dw 0          ; [0x0002]  WORD LenMod512 ; Bytes on the last page
    dw 1          ; [0x0004]  WORD Pages     ; 512 Byes per page

        ; Minimum number of pages is 1.
        ; DOS will actually try to read 512 Bytes,
        ; ignoring the `LenMod512` field.

    dw 0          ; [0x0006]  WORD RelCount  ; number of relocations
    dw 0x20       ; [0x0008]  WORD HeadSize  ; in 16 Byte paragraphs

        ; "MS-DOS 1" only accepted headers which spanned
        ; a multiple of 512 Bytes. Even though no data is attached
        ; in this executable file, this configuration ensures
        ; that only the Environment and the Program Segment Prefix
        ; are allocated, and no more bytes are read from the file.

    dw 0          ; [0x000A]  WORD MinBSS    ; in 16 Byte paragraphs
    dw 1          ; [0x000C]  WORD MaxBSS    ; in 16 Byte paragraphs

        ; This field was treated as "LOADLOW" in "MS-DOS 1.25".
        ; Since "MS-DOS 2+" will try to allocate all available memory
        ; for the relocable executable, the best we can do is to
        ; choose `MinBSS=0` and `MaxBSS=1`. Note that this program
        ; does not actually require any additional paragraphs.

    dw 0xFFF0     ; [0x000E]  WORD InitSS
    dw 0x0100     ; [0x0010]  WORD InitSP

        ; * `SS:SP` = `FFF0:0100`
        ; * Stack Segment at "-16" paragraphs = at "-256" bytes = at PSP
        ; * PSP[256] => end of the command tail (a tiny 128-Byte stack)

    dw 0          ; [0x0012]  WORD Chksum    ; Never used by program loaders

    dw 0          ; [0x0014]  WORD InitIP
    dw 0xFFF0     ; [0x0016]  WORD InitCS

        ; * `CS:IP` = `FFF0:0000`
        ; * Code Segment at Program Segment Prefix
        ; * PSP[0] => contains `INT 20h` instruction (ABORT)
        ; * Since CS is now adjusted to be equal to PSP,
        ;  this program can safely return to DOS!

    dw 0x40       ; [0x0018]  WORD RelocTab  ; Absolute offset of the Relocation Table

        ; This exact value is required so that MS LINK
        ; does not throw a warning. There are actaully
        ; no relocation entries, but MS-DOS always reads
        ; the first few bytes (in a `do ... while` loop).

    dw 0          ; [0x001A]  WORD Overlay   ; Unused (usually 0)
    dw 1          ; [0x001C]  WORD SymTab    ; Unused (usually 1)

        ; Further bytes are not required for "MS-DOS 2+",
        ; only for the new (X86/AMD64) Microsoft Linker.

    times 30 db 0 ; [0x001E]  BYTE Pad[30]   ; Unused (normally would contain Relocation Table)

    dd 0          ; [0x003C] DWORD LfaNew    ; Long File Address of the New Header

;;
;; * "Microsoft Incremental Linker" (VS2022) notes:
;;
;; if (dwFileSize < 0x40) or ('MZ' != Stub.Signature)
;; {
;;   "fatal error LNK1132: invalid format for MS-DOS stub file 'filename'"
;; }
;;
;; if (Stub.RelocTab < 0x40)
;; {
;;   "warning LNK4060: stub file missing full MS-DOS header;
;;   rebuild stub with /KNOWEAS 16-bit LINK option"
;; }
;;
;; * "PE Header" is aligned on 8-byte boundary, memory allocated
;;  by the "DOS STUB" is optionally rounded up and zero-filled.
;;
;; * "LfaNew" field is discarded and replaced with
;;  the actual offset of "PE" header by the New Linker.
;;
