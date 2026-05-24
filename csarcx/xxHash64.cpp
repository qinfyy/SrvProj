#include "pch.h"
#include "xxHash64.h"
#include <cstdint>
#include "defs.h"

// UInt64 XXH64_hash(Void*, Int32, UInt64)
uint64_t K4os_Hash_xxHash_XXH64_XXH64_hash(uint8_t* input, int32_t len, uint64_t seed)
{
    __int64 v4; // r14
    uint64_t v5; // rdi
    uint8_t* v6; // rbx
    uint8_t* v7; // rbp
    uint64_t v8; // rax
    uint64_t v9; // rsi
    uint64_t v10; // rdx
    uint64_t v11; // rcx
    uint64_t v12; // rax
    unsigned __int64 v13; // rax
    uint8_t* i; // r8
    __int64 v15; // rcx
    unsigned __int64 v16; // rax
    __int64 v17; // rcx
    __int64 v18; // rcx
    unsigned __int64 v19; // rdx

    v4 = len;
    v5 = seed;
    v6 = input;
    v7 = &input[len];
    if (len >= 32)
    {
        v9 = seed + 0x60EA27EEADC0B5D6LL;
        v10 = seed - 0x3D4D51C2D82B14B1LL;
        v11 = seed + 0x61C8864E7A143579LL;
        do
        {
            v9 = 0x9E3779B185EBCA87uLL
                * (((v9 - 0x3D4D51C2D82B14B1LL * *(_QWORD*)v6) << 31) | ((v9 - 0x3D4D51C2D82B14B1LL * *(_QWORD*)v6) >> 33));
            v10 = 0x9E3779B185EBCA87uLL
                * (((v10 - 0x3D4D51C2D82B14B1LL * *(_QWORD*)&v6[8]) << 31)
                    | ((v10 - 0x3D4D51C2D82B14B1LL * *(_QWORD*)&v6[8]) >> 33));
            v5 = 0x9E3779B185EBCA87uLL
                * (((v5 - 0x3D4D51C2D82B14B1LL * *(_QWORD*)&v6[16]) << 31)
                    | ((v5 - 0x3D4D51C2D82B14B1LL * *(_QWORD*)&v6[16]) >> 33));
            v12 = v11 - 0x3D4D51C2D82B14B1LL * *(_QWORD*)&v6[24];
            v6 += 32;
            v11 = 0x9E3779B185EBCA87uLL * ((v12 << 31) | (v12 >> 33));
        } while (v6 <= &v7[-32]);
        v8 = 0x9E3779B185EBCA87uLL
            * ((0x9E3779B185EBCA87uLL * ((0x93EA75A780000000uLL * v11) | ((0xC2B2AE3D27D4EB4FuLL * v11) >> 33)))
                ^ (0x9E3779B185EBCA87uLL
                    * ((0x9E3779B185EBCA87uLL * ((0x93EA75A780000000uLL * v5) | ((0xC2B2AE3D27D4EB4FuLL * v5) >> 33)))
                        ^ (0x9E3779B185EBCA87uLL
                            * ((0x9E3779B185EBCA87uLL * ((0x93EA75A780000000uLL * v10) | ((0xC2B2AE3D27D4EB4FuLL * v10) >> 33)))
                                ^ (0x9E3779B185EBCA87uLL
                                    * ((0x9E3779B185EBCA87uLL * ((0x93EA75A780000000uLL * v9) | ((0xC2B2AE3D27D4EB4FuLL * v9) >> 33)))
                                        ^ (((2 * v9) | (v9 >> 63))
                                            + ((v10 << 7) | (v10 >> 57))
                                            + ((v5 << 12) | (v5 >> 52))
                                            + ((v11 << 18) | (v11 >> 46))))
                                    - 0x7A1435883D4D519DLL))
                            - 0x7A1435883D4D519DLL))
                    - 0x7A1435883D4D519DLL))
            - 0x7A1435883D4D519DLL;
    }
    else
    {
        v8 = seed + 0x27D4EB2F165667C5LL;
    }
    v13 = v4 + v8;
    for (i = v6 + 8; i <= v7; v13 = 0x9E3779B185EBCA87uLL * ((v16 << 27) | (v16 >> 37)) - 0x7A1435883D4D519DLL)
    {
        v15 = *(_QWORD*)v6;
        v6 = i;
        i += 8;
        v16 = v13 ^ (0x9E3779B185EBCA87uLL * ((0x93EA75A780000000uLL * v15) | ((0xC2B2AE3D27D4EB4FuLL * v15) >> 33)));
    }
    if (&v6[4] <= v7)
    {
        v17 = *(_DWORD*)v6;
        v6 += 4;
        v13 = 0xC2B2AE3D27D4EB4FuLL
            * (((v13 ^ (0x9E3779B185EBCA87uLL * v17)) << 23) | ((v13 ^ (0x9E3779B185EBCA87uLL * v17)) >> 41))
            + 0x165667B19E3779F9LL;
    }
    for (;
        v6 < v7;
        v13 = 0x9E3779B185EBCA87uLL
        * (((v13 ^ (0x27D4EB2F165667C5LL * v18)) << 11) | ((v13 ^ (0x27D4EB2F165667C5LL * v18)) >> 53)))
    {
        v18 = (__int64)*v6++;
    }
    v19 = 0x165667B19E3779F9LL
        * ((0xC2B2AE3D27D4EB4FuLL * (v13 ^ (v13 >> 33))) ^ ((0xC2B2AE3D27D4EB4FuLL * (v13 ^ (v13 >> 33))) >> 29));
    return v19 ^ HIDWORD(v19);
}
