// Created by John Åkerblom 2012-07-??

#include "hook_utils.h"

#ifndef NO_UDIS86
    #include <libudis86/extern.h>
#endif

#include <Psapi.h>
#include <Windows.h>

#define JMP_SIZE_X86 5 

// hotpatch and unhotpatch functions based off some internet
// code in 2012

// Orig: http://www.codeproject.com/Articles/27339/API-hooking-for-hotpatchable-operating-systems
VOID hook_hotpatch_x86(void *old_proc, const void *new_proc, void **orig_proc)
{
    DWORD old_protect = 0;
    WORD *back_jmp = (WORD *)old_proc;
    BYTE *long_jmp = ((BYTE *)old_proc - 5);
    DWORD *long_jmpaddr = ((DWORD *)old_proc - 1);

    VirtualProtect(long_jmp, 7, PAGE_EXECUTE_WRITECOPY, &old_protect);

    *long_jmp = 0xE9; // long jmp
    *long_jmpaddr = ((DWORD)new_proc) - ((DWORD)old_proc);
    *back_jmp = 0xF9EB; // short jmp back -7 (back 5, plus two for this jmp)

    if (NULL != orig_proc)
    {
        *orig_proc = ((BYTE *)old_proc) + 2;
    }
    
    VirtualProtect(long_jmp, 7, old_protect, &old_protect);
}

BOOL hook_unhotpatch_x86(void *old_proc)
{
    BOOL ret = FALSE;
    DWORD old_protect = 0;
    WORD * back_jmp = (WORD *)old_proc;

    VirtualProtect(back_jmp, 2, PAGE_EXECUTE_WRITECOPY, &old_protect);

    if (0xF9EB == *back_jmp)
    {
        *back_jmp = 0xFF8B; // mov edi, edi
        ret = TRUE;
    }

    VirtualProtect(back_jmp, 2, old_protect, &old_protect);

    return ret;
}

#ifndef NO_UDIS86

// trampoline hook using a little simple disassembler logic
// for some versatility (simple instruction boundary check->
// carry over into trampoline + jmp detection for dest 
// recalculation)
BOOL hook_trampoline_dis_x86(void **old_proc, const void *new_proc)
{
    DWORD old_protect       = 0;
    void *source            = *old_proc;
    void *trampoline        = NULL;
    unsigned char *dest     = NULL;
    DWORD offset_source     = 0;
    DWORD offset_trampoline = 0;
    DWORD total             = 0;
    DWORD bytes             = 0;
    ud_t dis;

    ud_init(&dis);

    ud_set_mode(&dis, 32);
    ud_set_syntax(&dis, UD_SYN_INTEL);
    ud_set_pc(&dis, (uint64_t)source);
    ud_set_input_buffer(&dis, source, 0x1000);

    if (NULL == (trampoline = VirtualAlloc(NULL, 2 * JMP_SIZE_X86, 
                    MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)))
    {
        return FALSE;
    }

    offset_source = (DWORD)new_proc - (DWORD)source - JMP_SIZE_X86;
    offset_trampoline = (DWORD)source - (DWORD)trampoline - JMP_SIZE_X86;

    // Use disassembler to make sure the last instruction copied into
    // trampoline will be complete. If jmp instruction, recalculate dest.
    for (total = 0; total < JMP_SIZE_X86;)
	{
		if (0 == (bytes = ud_disassemble(&dis)))
		{
			return FALSE;
		}

		total += bytes;

		if (UD_Ijmp == dis.mnemonic)
		{
			// recalculate relative jump
			DWORD current_jmpdest;
			DWORD new_jmpdest;
			
			dest = (unsigned char *)(DWORD)source + (total-bytes) + 1;

			current_jmpdest	= (DWORD)source + total + *(DWORD *)dest;
			new_jmpdest		= current_jmpdest - total - (DWORD)trampoline;

			if (FALSE == VirtualProtect(dest, 5, 4, &old_protect))
			{
				return FALSE;
			}

			memcpy(dest, &new_jmpdest, sizeof(new_jmpdest));

			if (FALSE == VirtualProtect(dest, 5, old_protect, &old_protect))
			{
				return FALSE;
			}
		}
	}

    if (FALSE == VirtualProtect(source, total, PAGE_EXECUTE_READWRITE, &old_protect))
    {
        return FALSE;
    }

    memcpy(trampoline, source, total);
    *((BYTE *)trampoline + total) = 0xE9;
    dest = trampoline;

    dest += total + 1;
    memcpy(dest, &offset_trampoline, sizeof(DWORD));

    *((BYTE *)source) = 0xE9;
    dest = source;

    dest += 1;
    memcpy(dest, &offset_source, sizeof(DWORD));

    VirtualProtect(source, total, old_protect, &old_protect);

    *old_proc = trampoline;

    return TRUE;
}

BOOL hook_trampoline_dis_x86_by_name(const char *dll_name,
    const char *proc_name,
    void *new_proc,
    void **old_proc)
{
    if (NULL == (*old_proc = GetProcAddress(GetModuleHandleA(dll_name), proc_name)))
    {
        return FALSE;
    }

    if (FALSE == hook_trampoline_dis_x86(old_proc, new_proc))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL hook_trampoline_dis_x86_by_needle(HANDLE module, BYTE *needle, DWORD nsize, int offset_from_needle, void **old_proc, const void *new_proc)
{
    *old_proc = (void *)hook_find_by_needle(module, needle, nsize);

    if (NULL == *old_proc)
    {
        return FALSE;
    }

    *old_proc = ((BYTE *)*old_proc) + offset_from_needle;

    return hook_trampoline_dis_x86(old_proc, new_proc);;
}

BOOL hook_trampoline_dis_x86_by_needle_preserve(HANDLE module, BYTE *needle, DWORD nsize, int offset_from_needle, void **old_proc, const void *new_proc, void **preserved_old_proc)
{
    *old_proc = (void *)hook_find_by_needle(module, needle, nsize);

    if (NULL == *old_proc)
    {
        return FALSE;
    }

    *old_proc = ((BYTE *)*old_proc) + offset_from_needle;
    *preserved_old_proc = *old_proc;

    return hook_trampoline_dis_x86(old_proc, new_proc);;
}

BOOL unhook_trampoline_dis_x86_preserved(const void *old_proc, void *preserved_old_proc)
{
    // TODO more sophisticated approach using atomic exchange / similar to avoid races
    DWORD old_protect = 0;
    BYTE *jmp_operand = NULL;
    uint32_t dest = 0;

    // if function was hooked with a trampoline hook there should be a jump (E9) at
    // the start, replace its destination with the old_proc trampoline which effectively
    // restores the function
    if (0xE9 != *(BYTE *)preserved_old_proc) 
    {
        return FALSE;
    }
    
    jmp_operand = (BYTE *)preserved_old_proc + 1;
    dest = (uint32_t)old_proc - (uint32_t)preserved_old_proc - sizeof(uint32_t) - sizeof(uint8_t);

    VirtualProtect(jmp_operand, sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &old_protect);
    *(uint32_t *)jmp_operand = dest;
    VirtualProtect(jmp_operand, sizeof(uint32_t), old_protect, &old_protect);

    return TRUE;
}

#endif

/*
* The memmem() function finds the start of the first occurrence of the
* substring 'needle' of length 'nlen' in the memory area 'haystack' of
* length 'hlen'.
*
* The return value is a pointer to the beginning of the sub-string, or
* NULL if the substring is not found.
*
* Original author: caf, http://stackoverflow.com/questions/2188914/c-searching-for-a-string-in-a-file
*/
static unsigned char *_memmem(unsigned char *haystack, size_t hlen, unsigned char *needle, size_t nlen)
{
    unsigned char needle_first;
    unsigned char *p = haystack;
    size_t plen = hlen;

    if (!nlen)
        return NULL;

    needle_first = *(unsigned char *)needle;

    while (plen >= nlen && (p = (unsigned char *)memchr(p, needle_first, plen - nlen + 1)))
    {
        if (!memcmp(p, needle, nlen))
            return p;

        p++;
        plen = hlen - (p - haystack);
    }

    return NULL;
}

BYTE *hook_find_by_needle(HANDLE module, BYTE *needle, DWORD nsize)
{
    MODULEINFO mi = { 0 };
    unsigned char *haystack = NULL;
    size_t hlen = 0;

    if (NULL == module)
        module = GetModuleHandle(NULL);

    GetModuleInformation(GetCurrentProcess(), (HMODULE)module, &mi, sizeof(mi));

    haystack = (unsigned char *)mi.lpBaseOfDll;
    hlen = mi.SizeOfImage;

    return _memmem(haystack, hlen, needle, nsize);
}
