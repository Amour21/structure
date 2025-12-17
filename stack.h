#include <vector>
#include <cstddef> 
#include <stdexcept>

template <typename T, template <typename, typename = std::allocator<T>> class Container = std::vector>
class Stack {
    Container<T> data;  
public:
    void push(const T& el) { data.push_back(el); }   

    void pop() {
        if (!data.empty())
            data.pop_back();
        else
            throw std::out_of_range("Stack is empty");
    }

    T top() const {
        if (!data.empty())
            return data.back();
        else
            throw std::out_of_range("Stack is empty");
    }

    size_t size() const { return data.size(); }
    void clear() { data.clear(); }
    bool empty() const { return data.empty(); }
};