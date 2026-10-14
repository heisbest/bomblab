#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

static uint32_t phase4_walk(uint32_t target, uint32_t lo, uint32_t hi, uint32_t key) {
    uint32_t mid = lo + (hi - lo) / 2U;
    if (target == mid) return (mid + 0x26U) ^ key;
    if (target < mid)
        return (phase4_walk(target, lo, mid - 1U, key) * 3U) ^ (mid + key) ^ 0xeU;
    return (phase4_walk(target, mid + 1U, hi, key) * 3U) ^ (mid + key) ^ 6U;
}

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    uint32_t hi = (uint32_t)(challenge >> 32), mid = (uint32_t)(challenge >> 16);
    uint32_t low = (uint32_t)challenge;
    uint32_t b = (hi + mid * 8U) ^ 0x554fU;
    uint32_t a = (hi + 0x83dcU) ^ low;
    values[0] = ((b * 3U) ^ (low + 0x4ab8U)) ^ a;
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    uint32_t prev = (uint32_t)(((challenge >> 16) & 0xfU) * 3U);
    values[0] = prev;
    for (unsigned i = 1; i < 6; ++i) {
        uint32_t part = (uint32_t)((challenge >> (i + 21)) & 0x3fU);
        prev = (part ^ (prev * 2U)) + 15U * i;
        values[i] = prev;
    }
    count = 6;
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    uint32_t index = (uint32_t)challenge & 7U;
    uint32_t arg = (uint32_t)(challenge >> 32) ^ (uint32_t)(challenge >> 3);
    uint32_t x = arg, result;
    switch (index) {
    case 0: result = (x ^ 0xd62dU) + (x >> 2); break;
    case 1: result = (x * 2U) ^ 0x28feU; break;
    case 2: result = x * 5U + 0x35e7bU; break;
    case 3: result = x * 5U + 0x5763U; break;
    case 4: result = (x ^ 0xaf00U) + (x >> 2); break;
    case 5: result = x * 5U + 0x5f60U; break;
    case 6: result = (x * 4U) ^ 0xbea6U; break;
    default: result = x * 5U + 0x2adf5U; break;
    }
    values[0] = index;
    values[1] = result;
    count = 2;
    return write_answer(out, cap, values, count);
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    uint32_t target = (uint32_t)challenge & 0x7fU;
    uint32_t key = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);
    uint32_t result = phase4_walk(target, 0, 127, key);
    values[0] = result;
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    struct node { unsigned next, tag, value; };
    static const struct node nodes[] = {
        {6, 0x74b, 0xdfb1}, {5, 0x18, 0x4ebd}, {10, 0x63c, 0x42e2},
        {8, 0x470, 0xa84f}, {9, 0x102, 0xffe9}, {11, 0x550, 0xbef3},
        {3, 0xb1a, 0xa78e}, {0, 0x334, 0xbab2}, {2, 0x863, 0x1936},
        {1, 0x227, 0xd30b}, {4, 0xa5d, 0x1219}, {11, 0x988, 0xe1f3}
    };
    static const unsigned heads[8] = {7, 0, 6, 3, 8, 10, 10, 4};
    unsigned n = heads[(unsigned)challenge & 7U];
    uint32_t fold = (uint32_t)(((challenge >> 6) ^ (challenge >> 32)) & 0xffffffffU) ^ 0xec77U;
    fold = fold * 3U + nodes[n].value;
    uint32_t tag = nodes[n].tag;
    fold ^= tag;
    for (unsigned i = 0; i < ((challenge >> 3) & 3U); ++i) {
        n = nodes[n].next; fold = fold * 3U + nodes[n].value;
        tag = nodes[n].tag; fold ^= tag;
    }
    values[0] = fold; values[1] = tag; count = 2;
    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    uint32_t x = (uint32_t)((challenge >> 6) ^ (challenge >> 32));
    switch ((unsigned)challenge & 7U) {
    case 0: x = (x ^ 0x62dbU) + (x << 7); break;
    case 1: x = (x + 0x4c7dU) ^ (x >> 2); break;
    case 2: x = (x >> 3) | (x << 29); break;
    case 3: x = (x << 5 | x >> 27) ^ 0xb8d0U; break;
    case 4: x = (x + 0x4c7dU) ^ (x >> 2); break;
    case 5: x = (x ^ (x >> 3)) + 0xc82dU; break;
    default: x = ((x << 7) + 0x8e02U) ^ (x * 5U); break;
    }
    x ^= 0x5387U;
    switch ((challenge >> 3) & 7U) {
    case 0: x = (x ^ 0x62dbU) + (x << 7); break;
    case 1: x = (x + 0x4c7dU) ^ (x >> 2); break;
    case 2: x = (x >> 3) | (x << 29); break;
    case 3: x = (x << 5 | x >> 27) ^ 0xb8d0U; break;
    case 4: x = (x + 0x4c7dU) ^ (x >> 2); break;
    case 5: x = (x ^ (x >> 3)) + 0xc82dU; break;
    default: x = ((x << 7) + 0x8e02U) ^ (x * 5U); break;
    }
    values[0] = x + 0xb61aU; count = 1;
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t a = (uint32_t)context & 3U;
    uint32_t r8 = (uint32_t)(challenge >> 32) ^ (uint32_t)challenge;
    uint32_t esi = (uint32_t)(context >> 32) ^ (uint32_t)context;
    uint32_t edi = esi ^ r8;
    uint32_t mix = ((edi >> 1) ^ 0xd243U) + (edi ^ 0xce66U);
    uint32_t b = mix ^ (a * 0xce66U) ^ (uint32_t)(challenge >> 16);
    uint32_t c = b ^ (r8 + a + 0xd243U + (esi >> 1));
    int n = snprintf(out, cap, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOPEN %u %u %u", a, b, c);
    return (n < 0 || (size_t)n >= cap) ? 1 : 0;
}
