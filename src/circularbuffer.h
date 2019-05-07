#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <vector>

namespace ou {

template <typename T>
class CircularBuffer : public std::vector<T> {

public:
    std::size_t m_head = 0, m_tail = 0;
    using std::vector<T>::vector;

    // returns number of elements in the buffer
    std::size_t count() const {
        int cnt = int(m_tail) - int(m_head);
        return cnt >= 0? cnt : cnt + this->size();
    }

    // return if buffer has empty space available
    bool available() const {
        return (m_tail + 1) % this->size() != m_head;
    }

    // push to buffer and return a reference to the element
    // that has been pushed
    T& push() {
        T& ref = (*this)[m_tail];
        m_tail = (m_tail + 1) % this->size();
        return ref;
    }

    // peek the oldest element
    T& top() {
        return (*this)[m_head];
    }

    // pop the oldest element
    void pop() {
        m_head = (m_head + 1) % this->size();
    }
};
}

#endif // CIRCULARBUFFER_H
