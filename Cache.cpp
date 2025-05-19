#include "Cache.h"

template<>
void CacheLine::WriteBytes<uint8_t>(size_t address, uint8_t value) {
    data[address] = value;
}

template<>
void CacheLine::WriteBytes<uint16_t>(size_t address, uint16_t value) {
    data[address] = (uint8_t)(value & (0xFF));
    data[address + 1] = (uint8_t)((value >> 8) & (0xFF));
}

template<>
void CacheLine::WriteBytes<uint32_t>(size_t address, uint32_t value) {
    data[address] = (uint8_t)(value & (0xFF));
    data[address + 1] = (uint8_t)((value >> 8) & (0xFF));
    data[address + 2] = (uint8_t)((value >> 16)& (0xFF));
    data[address + 3] = (uint8_t)((value >> 24) & (0xFF));
}

template<>
uint8_t CacheLine::ReadBytes<uint8_t>(size_t address) {
    return data[address];
}

template<>
uint16_t CacheLine::ReadBytes<uint16_t>(size_t address) {
    size_t ans = 0;
    ans |= (size_t)data[address];
    ans |= ((size_t)data[address + 1] << 8);
    return (uint16_t)ans;
}

template<>
uint32_t CacheLine::ReadBytes<uint32_t>(size_t address) {
    size_t ans = 0;
    ans |= (size_t)data[address];
    ans |= ((size_t)data[address + 1] << 8);
    ans |= ((size_t)data[address + 2] << 16);
    ans |= ((size_t)data[address + 3] << 24);
    return ans;
}