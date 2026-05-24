#include "pch.h"
#include "md5.h"
#include <stdexcept>

using namespace std;

static const uint32_t INIT_A = 1732584193;
static const uint32_t INIT_B = -271733879;
static const uint32_t INIT_C = -1732584194;
static const uint32_t INIT_D = 271733878;

static const char* T = "78A46AD756B7C7E8DB702024EECEBDC1AF0F7CF52AC68747134630A8019546FDD8988069AFF7448BB15BFFFFBED75C892211906B937198FD8E4379A62108B44962251EF640B340C0515A5E26AAC7B6E95D102FD65314440281E6A1D8C8FBD3E7E6CDE121D60737C3870DD5F4ED145A4505E9E3A9F8A3EFFCD9026F678A4C2A8D4239FAFF81F6718722619D6D0C38E5FD44EABEA4A9CFDE4B604BBBF670BCBFBEC67E9B28FA27A1EA8530EFD4051D880439D0D4D9E599DBE6F87CA21F6556ACC4442229F497FF2A43A72394AB39A093FCC3595B6592CC0C8F7DF4EFFFD15D84854F7EA86FE0E62CFE144301A3A111084E827E53F735F23ABDBBD2D72A91D386EB";
static const char* S = "070000000C0000001100000016000000070000000C0000001100000016000000070000000C0000001100000016000000070000000C000000110000001600000005000000090000000E0000001400000005000000090000000E0000001400000005000000090000000E0000001400000005000000090000000E00000014000000040000000B0000001000000017000000040000000B0000001000000017000000040000000B0000001000000017000000040000000B0000001000000017000000060000000A0000000F00000015000000060000000A0000000F00000015000000060000000A0000000F00000015000000060000000A0000000F00000015000000";

bool ParseArray(const char* hex_str, uint32_t out[64]) {
    if (!hex_str || strlen(hex_str) != 512) return false;

    for (int i = 0; i < 64; ++i) {
        uint32_t val = 0;
        if (sscanf_s(hex_str + i * 8, "%8x", &val) != 1) {
            return false;
        }

        out[i] = (val >> 24) | ((val >> 8) & 65280) | ((val << 8) & 16711680) | (val << 24);
    }

    return true;
}

uint32_t F(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) | (~x & z);
}

uint32_t G(uint32_t x, uint32_t y, uint32_t z) {
    return (x & z) | (y & ~z);
}

uint32_t H(uint32_t x, uint32_t y, uint32_t z) {
    return x ^ y ^ z;
}

uint32_t I(uint32_t x, uint32_t y, uint32_t z) {
    return y ^ (x | ~z);
}

uint32_t RotateLeft(uint32_t x, uint32_t n) {
    if (n == 0) {
        return x;
    }
    return (x << n) | (x >> (32 - n));
}

void padding(vector<uint8_t>& input) {
    size_t original_length = input.size();
    input.push_back(128);
    while (input.size() % 64 != 56) {
        input.push_back(0);
    }

    uint64_t bit_length = original_length * 8;
    for (int i = 0; i < 8; i++) {
        input.push_back((bit_length >> (i * 8)) & 255);
    }
}

void ProcessBlock(const uint8_t* block, uint32_t& A, uint32_t& B, uint32_t& C, uint32_t& D) {
    uint32_t M[16];
    for (int i = 0; i < 16; i++) {
        M[i] = (block[i * 4 + 0] << 0) |
            (block[i * 4 + 1] << 8) |
            (block[i * 4 + 2] << 16) |
            (block[i * 4 + 3] << 24);
    }

    uint32_t a = A, b = B, c = C, d = D;

    uint32_t S_arr[64];
    uint32_t T_arr[64];

    if (!ParseArray(S, S_arr))
        throw runtime_error("解析 S 数组失败");

    if (!ParseArray(T, T_arr))
        throw runtime_error("解析 T 数组失败");

    for (int i = 0; i < 64; i++) {
        uint32_t F_value, g;
        if (i < 16) {
            F_value = F(b, c, d);
            g = i;
        }
        else if (i < 32) {
            F_value = G(b, c, d);
            g = (5 * i + 1) % 16;
        }
        else if (i < 48) {
            F_value = H(b, c, d);
            g = (3 * i + 5) % 16;
        }
        else {
            F_value = I(b, c, d);
            g = (7 * i) % 16;
        }

        uint32_t temp = d;
        d = c;
        c = b;
        b = b + RotateLeft(a + F_value + M[g] + T_arr[i], S_arr[i]);
        a = temp;
    }

    A += a;
    B += b;
    C += c;
    D += d;
}

vector<uint8_t> C(const vector<uint8_t>& input) {
    vector<uint8_t> data = input;
    padding(data);

    uint32_t A = INIT_A, B = INIT_B, C = INIT_C, D = INIT_D;

    for (size_t i = 0; i < data.size(); i += 64) {
        ProcessBlock(&data[i], A, B, C, D);
    }

    vector<uint8_t> result(16);
    result[0] = (A) & 255;
    result[1] = (A >> 8) & 255;
    result[2] = (A >> 16) & 255;
    result[3] = (A >> 24) & 255;

    result[4] = (B) & 255;
    result[5] = (B >> 8) & 255;
    result[6] = (B >> 16) & 255;
    result[7] = (B >> 24) & 255;

    result[8] = (C) & 255;
    result[9] = (C >> 8) & 255;
    result[10] = (C >> 16) & 255;
    result[11] = (C >> 24) & 255;

    result[12] = (D) & 255;
    result[13] = (D >> 8) & 255;
    result[14] = (D >> 16) & 255;
    result[15] = (D >> 24) & 255;

    return result;
}
