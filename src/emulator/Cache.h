#pragma once

#include <iostream>
#include <memory>
#include "Memory.h"
#include "Constants.h"

enum Policy {
    LRU,
    bit_pLRU
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
    , valid(false)
    , dirty(false) {
        data = std::vector<uint8_t>(Constants::CACHE_LINE_SIZE);
    }

    template<class T>
    void WriteBytes(size_t address, T value) {}

    template<class T>
    T ReadBytes(size_t address) {return T();}

    std::vector<uint8_t> data;
    size_t tag;
    bool valid;
    bool dirty;
};

template<>
inline void CacheLine::WriteBytes<uint8_t>(size_t address, uint8_t value) {
    data[address] = value;
}

template<>
inline void CacheLine::WriteBytes<uint16_t>(size_t address, uint16_t value) {
    data[address] = (uint8_t)(value & (0xFF));
    data[address + 1] = (uint8_t)((value >> 8) & (0xFF));
}

template<>
inline void CacheLine::WriteBytes<uint32_t>(size_t address, uint32_t value) {
    data[address] = (uint8_t)(value & (0xFF));
    data[address + 1] = (uint8_t)((value >> 8) & (0xFF));
    data[address + 2] = (uint8_t)((value >> 16)& (0xFF));
    data[address + 3] = (uint8_t)((value >> 24) & (0xFF));
}

template<>
inline uint8_t CacheLine::ReadBytes<uint8_t>(size_t address) {
    return data[address];
}

template<>
inline uint16_t CacheLine::ReadBytes<uint16_t>(size_t address) {
    size_t ans = 0;
    ans |= (size_t)data[address];
    ans |= ((size_t)data[address + 1] << 8);
    return (uint16_t)ans;
}

template<>
inline uint32_t CacheLine::ReadBytes<uint32_t>(size_t address) {
    size_t ans = 0;
    ans |= (size_t)data[address];
    ans |= ((size_t)data[address + 1] << 8);
    ans |= ((size_t)data[address + 2] << 16);
    ans |= ((size_t)data[address + 3] << 24);
    return ans;
}

class BaseCacheSet {
public:
    template<class T>
    std::pair<T, bool> ReadBytes(size_t address) {
        size_t tag = Interpreter::tag(address);
        size_t offset = Interpreter::offset(address);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                UpdateLine(i);
                return {lines[i].ReadBytes<T>(offset), true};
            }
        }
        UpdateSet(address);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                UpdateLine(i);
                return {lines[i].ReadBytes<T>(offset), false};
            }
        }
        throw std::runtime_error("smth went wrong");
    }

    template<class T>
    bool WriteBytes(size_t address, T value) {
        size_t tag = Interpreter::tag(address);
        size_t offset = Interpreter::offset(address);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                UpdateLine(i);
                lines[i].WriteBytes<T>(offset, value);
                lines[i].dirty = true;
                return true;
            }
        }
        UpdateSet(address);
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid && lines[i].tag == tag) {
                UpdateLine(i);
                lines[i].WriteBytes<T>(offset, value);
                lines[i].dirty = true;
                return false;
            }
        }
        throw std::runtime_error("smth went wrong");
    }
    virtual ~BaseCacheSet() = default;
    virtual void invalidate(size_t index) = 0;

protected:
    virtual void UpdateLine(size_t i) = 0; 
    virtual void UpdateSet(size_t address) = 0;
    std::vector<CacheLine> lines;
};

class LRUCacheSet : public BaseCacheSet {
public:
    LRUCacheSet(Memory& mem_)
    : mem(mem_)
    , time(0) {
        lines = std::vector<CacheLine>(Constants::CACHE_WAY);
        time_cnt = std::vector<size_t>(Constants::CACHE_WAY);
    }

    void invalidate(size_t index) override {
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid) {
                uint32_t begin_index = Interpreter::make_address(lines[i].tag, index);
                if (lines[i].dirty) {
                    mem.WriteBlock(begin_index, lines[i].data);
                }
                lines[i].valid = false;
            }
        }
    }
private:
    void UpdateLine(size_t i) override {
        time_cnt[i] = time++;
    }
    void UpdateSet(size_t address) override {
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
                time_cnt[i] = time++;
                return;
            }
            if (time - time_cnt[i] > diff) {
                diff = time - time_cnt[i];
                index = i;
            }
        }
        if (lines[index].dirty) {
            mem.WriteBlock(
                Interpreter::make_address(lines[index].tag, Interpreter::index(address)), 
                lines[index].data
            );
        }
        // std::cout << "CACHE OUT!\n";
        lines[index].data = mem.ReadBlock(begin_index, Constants::CACHE_LINE_SIZE);
        lines[index].tag = Interpreter::tag(address);
        time_cnt[index] = time++;
    }
    std::vector<size_t> time_cnt;
    Memory& mem;
    size_t time;
};

class BitPLRUCacheSet : public BaseCacheSet {
public:
    BitPLRUCacheSet(Memory& mem_)
    : mem(mem_) {
        lines = std::vector<CacheLine>(Constants::CACHE_WAY);
        pseudo_lru = std::vector<bool>(Constants::CACHE_WAY);
    }

    void invalidate(size_t index) override {
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (lines[i].valid) {
                uint32_t begin_index = Interpreter::make_address(lines[i].tag, index);
                if (lines[i].dirty) {
                    mem.WriteBlock(begin_index, lines[i].data);
                }
                lines[i].valid = false;
            }
        }
    }
private:
    void UpdateLine(size_t i) override {
        size_t cnt = 0;
        for (int j = 0; j < Constants::CACHE_WAY; ++j) {
            if (j != i) {
                cnt += pseudo_lru[j] ? 1 : 0;
            }
        }
        if (cnt == Constants::CACHE_WAY - 1) {
            pseudo_lru.assign(Constants::CACHE_WAY, false);
        }
        pseudo_lru[i] = true;
    }

    void UpdateSet(size_t address) override {
        size_t begin_index = (
            address & ~((1ULL << Constants::CACHE_OFFSET_LEN) - 1)
        );
        for (int i = 0; i < Constants::CACHE_WAY; ++i) {
            if (!lines[i].valid) {
                lines[i].data = mem.ReadBlock(begin_index, Constants::CACHE_LINE_SIZE);
                lines[i].tag = Interpreter::tag(address);
                lines[i].valid = true;
                return;
            }
            if (!pseudo_lru[i]) {
                if (lines[i].dirty) {
                    mem.WriteBlock(
                        Interpreter::make_address(lines[i].tag, Interpreter::index(address)),
                        lines[i].data);
                }
                lines[i].data = mem.ReadBlock(begin_index, Constants::CACHE_LINE_SIZE);
                lines[i].tag = Interpreter::tag(address);
                return;
            }
        }
    }
    std::vector<bool> pseudo_lru;
    Memory& mem;
};

class Cache {
public:
    Cache(Policy policy, Memory& mem)
    : hits_commands(0)
    , hits_data(0)
    , all_commands(0)
    , all_data(0) 
    , policy_(policy)
    , mem_(mem) {
        if (policy_ == Policy::LRU) {
            sets_.reserve(Constants::CACHE_SET_COUNT);
            for (size_t i = 0; i < Constants::CACHE_SET_COUNT; ++i) {
                sets_.emplace_back(std::make_unique<LRUCacheSet>(mem_));
            }
        } else if (policy_ == Policy::bit_pLRU) {
            sets_.reserve(Constants::CACHE_SET_COUNT);
            for (size_t i = 0; i < Constants::CACHE_SET_COUNT; ++i) {
                sets_.emplace_back(std::make_unique<BitPLRUCacheSet>(mem_));
            }
        }
    }

    uint8_t ReadByte(size_t address, bool is_command = false) {
        auto [res, hit] = sets_[Interpreter::index(address)]->ReadBytes<uint8_t>(address);
        Add(is_command, hit);
        return res;
    }

    uint16_t Read2Bytes(size_t address, bool is_command = false) {
        auto [res, hit] = sets_[Interpreter::index(address)]->ReadBytes<uint16_t>(address);
        Add(is_command, hit);
        return res;
    }

    uint32_t Read4Bytes(size_t address, bool is_command = false) {
        // std::cout << "reading on address: " << address << "; CacheSet: " << Interpreter::index(address) << "\n";
        auto [res, hit] = sets_[Interpreter::index(address)]->ReadBytes<uint32_t>(address);
        Add(is_command, hit);
        return res;
    }

    void WriteByte(size_t address, uint8_t value) {
        // std::cout << "writing in address: " << Interpreter::index(address) << "; CacheSet: " << Interpreter::index(address) << "\n";
        bool hit = sets_[Interpreter::index(address)]->WriteBytes<uint8_t>(address, value);
        Add(false, hit);
    }

    void Write2Bytes(size_t address, uint16_t value) {
        bool hit = sets_[Interpreter::index(address)]->WriteBytes<uint16_t>(address, value);
        Add(false, hit);
    }

    void Write4Bytes(size_t address, uint32_t value) {
        bool hit = sets_[Interpreter::index(address)]->WriteBytes<uint32_t>(address, value);
        Add(false, hit);
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
            sets_[i]->invalidate(i);
        }
    }
private:
    size_t hits_commands;
    size_t hits_data;
    size_t all_commands;
    size_t all_data;
    Policy policy_;
    Memory& mem_;
    std::vector<std::unique_ptr<BaseCacheSet>> sets_;
};