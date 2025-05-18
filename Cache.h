#pragma once

#include "Memory.h"
#include "Constants.h"

enum Policy {
    LRU,
    pLRU
};

class Interpreter {
public:
    static size_t offset(size_t address) {
        return address & ((1ULL << Constants::CACHE_OFFSET_LEN) - 1);
    }
    static size_t index(size_t address) {
        return (
            (address >> Constants::CACHE_OFFSET_LEN) & 
            ((1ULL << Constants::CACHE_INDEX_LEN) - 1)
        );
    }
    static size_t tag(size_t address) {
        return (
            (address >> (Constants::CACHE_OFFSET_LEN + Constants::CACHE_INDEX_LEN)) &
            ((1ULL << Constants::CACHE_TAG_LEN) - 1)
        );
    }

    static size_t make_address(size_t tag, size_t index) {
        return (tag << (Constants::CACHE_INDEX_LEN + Constants::CACHE_OFFSET_LEN)) |
            (index << Constants::CACHE_OFFSET_LEN);
    }
};

struct CacheLine {
    CacheLine() 
    : tag(0)
    , valid(false) {
        data = std::vector<uint8_t>(Constants::CACHE_LINE_SIZE);
    }

    void WriteByte(size_t address, uint8_t value) {
        data[address] = value;
    }

    void Write2Bytes(size_t address, uint16_t value) {
        data[address] = (uint8_t)(value & (0xFF));
        data[address + 1] = (uint8_t)((value >> 8) & (0xFF));
    }

    void Write4Bytes(size_t address, uint32_t value) {
        data[address] = (uint8_t)(value & (0xFF));
        data[address + 1] = (uint8_t)((value >> 8) & (0xFF));
        data[address + 2] = (uint8_t)((value >> 16)& (0xFF));
        data[address + 3] = (uint8_t)((value >> 24) & (0xFF));
    }

    size_t ReadByte(size_t address) {
        return (size_t)data[address];
    }

    size_t Read2Bytes(size_t address) {
        size_t ans = 0;
        ans |= (size_t)data[address];
        ans |= ((size_t)data[address + 1] << 8);
        return ans;
    }

    size_t Read4Bytes(size_t address) {
        size_t ans = 0;
        ans |= (size_t)data[address];
        ans |= ((size_t)data[address + 1] << 8);
        ans |= ((size_t)data[address + 2] << 16);
        ans |= ((size_t)data[address + 3] << 24);
        return ans;
    }

    std::vector<uint8_t> data;
    size_t tag;
    bool valid;
};

class CacheSet {
public:
    CacheSet(Policy p, Memory& mem_)
    : policy(p)
    , mem(mem_) {
        lines = std::vector<CacheLine>(Constants::CACHE_WAY);
        time_cnt = std::vector<size_t>(Constants::CACHE_WAY);
    }

    std::pair<uint8_t, bool> ReadByte(size_t address, size_t time) {
        size_t tag = Interpreter::tag(address);
        size_t offset = Interpreter::offset(address);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                return {lines[i].ReadByte(offset), true};
            }
        }
        UpdateLRU(address, time);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                return {lines[i].ReadByte(offset), false};
            }
        }
        throw std::runtime_error("smth went wrong");
    }

    std::pair<uint16_t, bool> Read2Bytes(size_t address, size_t time) {
        size_t tag = Interpreter::tag(address);
        size_t offset = Interpreter::offset(address);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                return {lines[i].Read2Bytes(offset), true};
            }
        }
        UpdateLRU(address, time);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                return {lines[i].Read2Bytes(offset), false};
            }
        }
        throw std::runtime_error("smth went wrong");
    }

    std::pair<uint32_t, bool> Read4Bytes(size_t address, size_t time) {
        size_t tag = Interpreter::tag(address);
        size_t offset = Interpreter::offset(address);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                return {lines[i].Read4Bytes(offset), true};
            }
        }
        UpdateLRU(address, time);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                return {lines[i].Read4Bytes(offset), false};
            }
        }
        throw std::runtime_error("smth went wrong");
    }

    bool WriteByte(size_t address, uint8_t value, size_t time) {
        size_t tag = Interpreter::tag(address);
        size_t offset = Interpreter::offset(address);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                lines[i].WriteByte(offset, value);
                return true;
            }
        }
        UpdateLRU(address, time);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                lines[i].WriteByte(offset, value);
                return false;
            }
        }
        throw std::runtime_error("smth went wrong");
    }

    bool Write2Bytes(size_t address, uint16_t value, size_t time) {
        size_t tag = Interpreter::tag(address);
        size_t offset = Interpreter::offset(address);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                lines[i].Write2Bytes(offset, value);
                return true;
            }
        }
        UpdateLRU(address, time);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                lines[i].Write2Bytes(offset, value);
                return false;
            }
        }
        throw std::runtime_error("smth went wrong");
    }

    bool Write4Bytes(size_t address, uint32_t value, size_t time) {
        size_t tag = Interpreter::tag(address);
        size_t offset = Interpreter::offset(address);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                lines[i].Write4Bytes(offset, value);
                return true;
            }
        }
        UpdateLRU(address, time);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                time_cnt[i] = time;
                lines[i].Write4Bytes(offset, value);
                return false;
            }
        }
        throw std::runtime_error("smth went wrong");
    }

    void UpdateLRU(size_t address, size_t time) {
        size_t index = 0;
        size_t diff = 0;
        size_t begin_index = (
            address & ~((1ULL << Constants::CACHE_OFFSET_LEN) - 1)
        );
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (!lines[i].valid) {
                lines[i].data = mem.ReadBlock(begin_index, Constants::CACHE_LINE_SIZE);
                lines[i].tag = Interpreter::tag(address);
                lines[i].valid = true;
                time_cnt[i] = time;
                return;
            }
            if (time - time_cnt[i] > diff) {
                diff = time - time_cnt[i];
                index = i;
            }
        }
        mem.WriteBlock(begin_index, lines[index].data);
        lines[index].data = mem.ReadBlock(begin_index, Constants::CACHE_LINE_SIZE);
        lines[index].tag = Interpreter::tag(address);
        time_cnt[index] = time;
    }

    void invalidate(int index) {
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            // std::cout << "line: " << i << "\n";
            if (lines[i].valid) {
                uint32_t begin_index = Interpreter::make_address(lines[i].tag, index);
                mem.WriteBlock(begin_index, lines[i].data);
                lines[i].valid = false;
            }
        }
    }

private:
    std::vector<CacheLine> lines;
    std::vector<size_t> time_cnt;
    Memory& mem;
    Policy policy;
};

class Cache {
public:
    Cache(Policy policy, Memory& mem)
    : policy_(policy)
    , mem_(mem)
    , hits_commands(0)
    , hits_data(0)
    , all_commands(0)
    , all_data(0)
    , total_time(0) {
        sets_ = std::vector<CacheSet>(Constants::CACHE_SET_COUNT, CacheSet(policy, mem));
    }

    uint8_t ReadByte(size_t address, bool is_command = false) {
        auto [res, hit] = sets_[Interpreter::index(address)].ReadByte(address, total_time);
        Add(is_command, hit);
        ++total_time;
        return res;
    }

    uint16_t Read2Bytes(size_t address, bool is_command = false) {
        auto [res, hit] = sets_[Interpreter::index(address)].Read2Bytes(address, total_time);
        Add(is_command, hit);
        ++total_time;
        return res;
    }

    uint32_t Read4Bytes(size_t address, bool is_command = false) {
        auto [res, hit] = sets_[Interpreter::index(address)].Read4Bytes(address, total_time);
        Add(is_command, hit);
        ++total_time;
        return res;
    }

    void WriteByte(size_t address, uint8_t value) {
        bool hit = sets_[Interpreter::index(address)].WriteByte(address, value, total_time);
        Add(false, hit);
        ++total_time;
    }

    void Write2Bytes(size_t address, uint16_t value) {
        bool hit = sets_[Interpreter::index(address)].Write2Bytes(address, value, total_time);
        Add(false, hit);
        ++total_time;
    }

    void Write4Bytes(size_t address, uint32_t value) {
        bool hit = sets_[Interpreter::index(address)].Write4Bytes(address, value, total_time);
        Add(false, hit);
        ++total_time;
    }

    void Add(bool is_command, bool hit) {
        if (is_command) {
            hits_commands += hit ? 1 : 0;
            ++all_commands;
        }
        else {
            hits_data += hit ? 1 : 0;
            ++all_data;
        }
    }
    
    size_t GetHitsCommands() {
        return hits_commands;
    }

    size_t GetHitsData() {
        return hits_data;
    }

    size_t GetAllCommands() {
        return all_commands;
    }

    size_t GetAllData() {
        return all_data;
    }

    void invalidate_all() {
        for (int i = 0; i < Constants::CACHE_SET_COUNT; ++i) {
            // std::cout << "set: " << i << "\n";
            sets_[i].invalidate(i);
        }
    }
private:
    size_t hits_commands;
    size_t hits_data;
    size_t all_commands;
    size_t all_data;
    size_t total_time;
    Policy policy_;
    Memory& mem_;
    std::vector<CacheSet> sets_;
};