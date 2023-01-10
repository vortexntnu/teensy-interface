#include <memory>
#pragma once

/// @brief a ring buffer is a concept to store a maximum amount of data in a "ring". when a new value has to be savec, it will be put at the "end"
// the end will be incremented so that the next value will be written one adress further. When reading it will read the first value not read.
// this way reading and writing can happen in any order, as long as no more data than the SIZE is written before reading. If not, some data will be lost
// without processing it
class RingBuffer
{

public:
    static constexpr int SIZE = 1500; /// 1500 uint16_t can be stored in 1 ringbuffer

    RingBuffer()
        : start{0}, end{0}
    {
    }

    bool isEmpty() const { return start == end; }

    void insert(uint16_t item)
    {
        buffer[end++] = item;
        end %= SIZE; /// when at the end of ringbuffer, just restarts at the start of the buffer
    }

    void insert(uint16_t data[])
    {
        /// not yet implemented??
    }

    uint16_t get()
    {
        int item = buffer[start++];
        start %= SIZE;
        return item;
    }

    uint16_t operator++(int)
    { /// redefines the operator ++, because it is not defined by default (Ringbuffer++ would not work)
        return this->get();
    }

private:
    uint16_t buffer[SIZE];
    uint8_t start;
    uint8_t end;
};
