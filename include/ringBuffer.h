#include <memory>


class RingBuffer {

    public:
        static constexpr int SIZE = 1500; 

        RingBuffer() : start{0}, end{0} {}

        bool isEmpty() const { return start == end; }

        void insert(uint16_t item) {
            buffer[end++] = item;
            end %= SIZE;
        }

        void insert(uint16_t data[]) {
            
        }

        uint16_t get() { 
            int item = buffer[start++];
            start %= SIZE;
            return item;
        }


    private:
        uint16_t buffer[SIZE];
        uint8_t start;
        uint8_t end;
};