#pragma once
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <iterator>

template <class T> class ForwardList;
template <class Value, class NodePtr> class ForwardListIterator;
template <class T>
class ForwardList {
private:
    struct Node {
        T data;
        Node* next = nullptr;
        Node(const T& v) : data(v), next(nullptr) {}
        Node(T&& v) : data(std::move(v)), next(nullptr) {}
    };

    Node* first = nullptr;
    std::size_t sz = 0;

public:
    using iterator = ForwardListIterator<T, Node*>;
    using const_iterator = ForwardListIterator<const T, const Node*>;
    ForwardList() noexcept = default;
    ForwardList(std::size_t n, const T& default_value) {
        if (n == 0) return;
        first = new Node(default_value);
        Node* cur = first;
        ++sz;
        for (std::size_t i = 1; i < n; ++i) {
            cur->next = new Node(default_value);
            cur = cur->next;
            ++sz;
        }
    }
    ForwardList(const ForwardList& other) {
        if (!other.first) return;
        first = new Node(other.first->data);
        ++sz;
        Node* cur_new = first;
        Node* cur_old = other.first->next;
        while (cur_old) {
            cur_new->next = new Node(cur_old->data);
            cur_new = cur_new->next;
            cur_old = cur_old->next;
            ++sz;
        }
    }
    ForwardList(ForwardList&& other) noexcept
        : first(other.first), sz(other.sz)
    {
        other.first = nullptr;
        other.sz = 0;
    }
    ForwardList& operator=(const ForwardList& other) {
        if (this == &other) return *this;
        clear();
        if (!other.first) return *this;
        first = new Node(other.first->data);
        ++sz;
        Node* cur_new = first;
        Node* cur_old = other.first->next;
        while (cur_old) {
            cur_new->next = new Node(cur_old->data);
            cur_new = cur_new->next;
            cur_old = cur_old->next;
            ++sz;
        }
        return *this;
    }
    ForwardList& operator=(ForwardList&& other) noexcept {
        if (this == &other) return *this;
        clear();
        first = other.first;
        sz = other.sz;
        other.first = nullptr;
        other.sz = 0;
        return *this;
    }
    ~ForwardList() { clear(); }
    std::size_t size() const noexcept { return sz; }
    bool empty() const noexcept { return first == nullptr; }
    T& operator[](std::size_t index) {
        if (index >= sz) throw std::out_of_range("ForwardList: index out of range");
        Node* cur = first;
        for (std::size_t i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }
    const T& operator[](std::size_t index) const {
        if (index >= sz) throw std::out_of_range("ForwardList: index out of range");
        const Node* cur = first;
        for (std::size_t i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }
    iterator push_front(const T& value) {
        Node* tmp = new Node(value);
        tmp->next = first;
        first = tmp;
        ++sz;
        return iterator(first);
    }
    iterator push_front(T&& value) {
        Node* tmp = new Node(std::move(value));
        tmp->next = first;
        first = tmp;
        ++sz;
        return iterator(first);
    }
    iterator insert_after(const iterator& pos, const T& value) {
        if (pos.ptr == nullptr) throw std::out_of_range("insert_after: position is end()");
        Node* node = new Node(value);
        node->next = pos.ptr->next;
        pos.ptr->next = node;
        ++sz;
        return iterator(node);
    }
    iterator insert_after(const iterator& pos, T&& value) {
        if (pos.ptr == nullptr) throw std::out_of_range("insert_after: position is end()");
        Node* node = new Node(std::move(value));
        node->next = pos.ptr->next;
        pos.ptr->next = node;
        ++sz;
        return iterator(node);
    }
    iterator erase_after(const iterator& pos) {
        if (first == nullptr) return end();
        if (pos.ptr == nullptr) return erase_front();
        Node* target = pos.ptr->next;
        if (!target) return end();
        pos.ptr->next = target->next;
        delete target;
        --sz;
        return iterator(pos.ptr->next);
    }
    iterator erase_front() {
        if (!first) return end();
        Node* tmp = first->next;
        delete first;
        first = tmp;
        --sz;
        return iterator(first);
    }
    void clear() noexcept {
        Node* cur = first;
        while (cur) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        first = nullptr;
        sz = 0;
    }
    iterator find(const T& value) {
        Node* cur = first;
        while (cur) {
            if (cur->data == value) return iterator(cur);
            cur = cur->next;
        }
        return end();
    }
    const_iterator find(const T& value) const {
        const Node* cur = first;
        while (cur) {
            if (cur->data == value) return const_iterator(cur);
            cur = cur->next;
        }
        return cend();
    }
    iterator begin() noexcept { return iterator(first); }
    iterator end() noexcept { return iterator(nullptr); }
    const_iterator begin() const noexcept { return const_iterator(first); }
    const_iterator end() const noexcept { return const_iterator(nullptr); }
    const_iterator cbegin() const noexcept { return const_iterator(first); }
    const_iterator cend() const noexcept { return const_iterator(nullptr); }
    friend class ForwardListIterator<T, Node*>;
    friend class ForwardListIterator<const T, const Node*>;
};

template <class Value, class NodePtr>
class ForwardListIterator {
    NodePtr ptr = nullptr;
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Value;
    using difference_type = std::ptrdiff_t;
    using pointer = Value*;
    using reference = Value&;
    ForwardListIterator() = default;
    explicit ForwardListIterator(NodePtr p) : ptr(p) {}
    reference operator*() const {
        return ptr->data;
    }
    pointer operator->() const {
        return &ptr->data;
    }
    ForwardListIterator& operator++() {
        if (ptr) ptr = ptr->next;
        return *this;
    }
    ForwardListIterator operator++(int) {
        ForwardListIterator tmp = *this;
        ++(*this);
        return tmp;
    }
    bool operator==(const ForwardListIterator& o) const { return ptr == o.ptr; }
    bool operator!=(const ForwardListIterator& o) const { return ptr != o.ptr; }
    friend class ForwardList<typename std::remove_const<Value>::type>;
    template <class, class> friend class ForwardListIterator;
private:
    NodePtr get_ptr() const { return ptr; }
    friend ForwardListIterator<typename std::remove_const<Value>::type, NodePtr>;
};


