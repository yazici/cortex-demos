/*******************************************************************************
    Copyright 2018 Google LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        https://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*******************************************************************************/

#pragma once

#include <iostream>

#include <cstdint>
#include <list>
#include <map>
#include <tuple>
#include <vector>

namespace mock {

class Memory;

class IOHandlerStub {
    public:
        virtual uint32_t write32(uint32_t addr, uint32_t old_value, uint32_t new_value) {
            (void)addr;
            (void)old_value;
            return new_value;
        }

        virtual uint32_t read32(uint32_t addr, uint32_t value) {
            (void)addr;
            return value;
        }

        void set_memory(Memory* mem) {
            mem_ = mem;
        }

    protected:
        void set_mem_value(uint32_t addr, uint32_t value);
        uint32_t get_mem_value(uint32_t addr) const;

    private:
        Memory* mem_;
};

class RegSetClearStub : public IOHandlerStub {
    public:
        RegSetClearStub(uint32_t addr, uint32_t set_addr, uint32_t clr_addr) :
            rw_addr_{addr}, set_addr_{set_addr}, clr_addr_{clr_addr} {}

        uint32_t write32(uint32_t addr, uint32_t old_value, uint32_t new_value) override {
            auto write_value = new_value;
            if (addr == set_addr_) {
                write_value = (old_value | new_value);
            } else if (addr == clr_addr_) {
                write_value = (old_value & (~new_value));
            }

            if (addr != rw_addr_) {
                set_mem_value(rw_addr_, write_value);
                set_mem_value(set_addr_, write_value);
                set_mem_value(clr_addr_, write_value);
            }
            return write_value;
        }

        uint32_t read32(uint32_t addr, uint32_t value) override {
            if (addr == set_addr_ || addr == clr_addr_) {
                return get_mem_value(rw_addr_);
            }

            return value;
        }

    protected:
        const uint32_t rw_addr_;
        const uint32_t set_addr_;
        const uint32_t clr_addr_;
};

class IgnoreWrites : public IOHandlerStub {
    public:
        uint32_t write32(uint32_t addr, uint32_t old_value, uint32_t new_value) override {
            (void)addr;
            (void)new_value;
            return old_value;
        }
};

class SourceIOHandler : public IOHandlerStub {
    public:
        uint32_t read32(uint32_t addr, uint32_t value) override {
            (void)addr;
            uint32_t ret = value;
            if (!read_seq_.empty()) {
                ret = *(read_seq_.begin());
                read_seq_.pop_front();
            }

            return ret;
        }

        void add_value(uint32_t value) {
            read_seq_.push_back(value);
        }

        size_t get_seq_len() const {
            return read_seq_.size();
        }

    private:
        std::list<uint32_t> read_seq_;
};

template <typename T>
class WriteSink : public IOHandlerStub {
    public:
        uint32_t write32(uint32_t addr, uint32_t old_value, uint32_t new_value) override {
            (void)addr;
            (void)old_value;

            sink_.push_back(static_cast<T>(new_value));
            return new_value;
        }

        void clear() {
            sink_.clear();
        }

        const std::vector<T>& get_data() const {
            return sink_;
        }

    private:
        std::vector<T> sink_;
};

class Memory {
    public:
        Memory() = default;
        Memory(const Memory& ) = delete;
        enum class Op {
            READ8,
            READ16,
            READ32,
            READPTR,
            WRITE8,
            WRITE16,
            WRITE32,
            WRITEPTR,
        };

        using JournalEntry = std::tuple<Op, uint32_t, uint32_t>;
        using JournalT = std::vector<JournalEntry>;

        const JournalT& get_journal() const;

        void reset();

        void* readptr(uint32_t addr) const;
        uint32_t read32(uint32_t addr) const;
        uint16_t read16(uint32_t addr) const;
        uint8_t read8(uint32_t addr) const;

        void writeptr(uint32_t addr, void* ptr);
        void write32(uint32_t addr, uint32_t value);
        void write16(uint32_t addr, uint16_t value);
        void write8(uint32_t addr, uint8_t value);

        void set_value_at(uint32_t addr, uint32_t value);
        uint32_t get_value_at(uint32_t addr) const;
        uint32_t get_value_at(uint32_t addr, uint32_t default_value) const;

        void* get_ptr_at(uint32_t addr) const;
        void set_ptr_at(uint32_t addr, void* ptr);

        void set_addr_io_handler(uint32_t addr, IOHandlerStub* io_handler);
        void set_addr_io_handler(uint32_t range_start, uint32_t range_end, IOHandlerStub* io_handler);

        unsigned int get_op_count(Op op) const;
        unsigned int get_op_count(Op op, uint32_t addr) const;

        void print_journal() const;

    private:
        void priv_write32(uint32_t addr, uint32_t value);
        uint32_t priv_read32(uint32_t addr) const;

        std::map<uint32_t, uint32_t> mem_map_;
        std::map<uint32_t, void*> mem_ptr_map_;
        // Note, memory mock does not own the pointers.
        // It is the responsibility of the caller to clean them up.
        // Special care needs to be taken in the case of the global
        // memory map object.
        std::map<uint32_t, IOHandlerStub*> addr_handler_map_;
        mutable JournalT journal_;
};

Memory& get_global_memory();

}  // namespace mock
