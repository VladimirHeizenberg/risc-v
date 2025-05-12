#pragma once

namespace Constants
{
const size_t MEMORY_SIZE = 256 * 1024;
const size_t ADDRESS_LEN = 18; // log2(MEMORY_SIZE)
const size_t CACHE_TAG_LEN = 8;
const size_t CACHE_INDEX_LEN = 5; // 1. log(CACHE_SET_COUNT)
const size_t CACHE_OFFSET_LEN = 5; // 2. ADDRESS_LEN - CACHE_INDEX_LEN - CACHE_TAG_LEN
const size_t CACHE_SIZE = 4096; // 5. CACHE_LINE_COUNT * CACHE_LINE_SIZE
const size_t CACHE_LINE_SIZE = 32; // 3. 2^(CACHE_OFFSET_LEN)
const size_t CACHE_LINE_COUNT = 128; // 4. CACHE_SET_COUNT * CACHE_WAY 
const size_t CACHE_SET_COUNT = 32;
const size_t CACHE_WAY = 4;
} // namespace Constants