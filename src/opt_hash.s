
global hash_crc32

section .text

hash_crc32:
    xor eax, eax

    crc32 eax, dword[rdi]
    crc32 eax, dword[rdi + 4]
    crc32 eax, dword[rdi + 8]
    crc32 eax, dword[rdi + 12]
    crc32 eax, dword[rdi + 16]
    crc32 eax, dword[rdi + 20]
    crc32 eax, dword[rdi + 24]
    crc32 eax, dword[rdi + 28]
    
    ret