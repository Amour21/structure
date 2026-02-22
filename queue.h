#include <vector>
#include <cstddef>
#include <stdexcept>
#include <utility>

template <class T, template <typename, typename = std::allocator<T>> class Container = std::vector>
class Queue {
    Container<T> data;      
    std::size_t head = 0;   
    std::size_t tail = 0;   
    std::size_t count = 0;  

public:
    explicit Queue(std::size_t initial_capacity = 8) {
        if (initial_capacity == 0) initial_capacity = 1;
        data.resize(initial_capacity);
        head = tail = 0;
        count = 0;
    }

    void push(const T& el) {
        ensure_capacity_for_push();
        data[tail] = el;
        tail = (tail + 1) % data.size();
        ++count;
    }
    void push(T&& el) {
        ensure_capacity_for_push();
        data[tail] = std::move(el);
        tail = (tail + 1) % data.size();
        ++count;
    }
    void pop() {
        if (empty()) throw std::out_of_range("pop from empty queue");
        head = (head + 1) % data.size();
        --count;
    }
    T front() const {
        if (empty()) throw std::out_of_range("front from empty queue");
        return data[head];
    }
    T back() const {
        if (empty()) throw std::out_of_range("back from empty queue");
        std::size_t idx = (tail + data.size() - 1) % data.size();
        return data[idx];
    }
    std::size_t size() const noexcept { return count; }
    bool empty() const noexcept { return count == 0; }
    void clear() noexcept {
        head = tail = 0;
        count = 0;
    }

private:
    void ensure_capacity_for_push() {
        std::size_t cap = data.size();
        if (cap == 0) { 
            data.resize(1);
            head = tail = 0;
            return;
        }
        if (count < cap) return; 
        std::size_t new_cap = cap * 2;
        if (new_cap == 0) new_cap = 1;
        Container<T> new_buf;
        new_buf.resize(new_cap);
        for (std::size_t i = 0; i < count; ++i) {
            new_buf[i] = std::move(data[(head + i) % cap]);
        }
        data.swap(new_buf);
        head = 0;
        tail = count;
    }
};