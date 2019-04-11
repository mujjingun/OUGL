#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <vector>

namespace ou {

template <typename T>
class CircularBuffer : public std::vector<T> {

public:
    std::size_t m_head = 0, m_tail = 0;
    using std::vector<T>::vector;

    std::size_t count() const {
        int cnt = int(m_tail) - int(m_head);
        return cnt >= 0? cnt : cnt + this->size();
    }

    bool available() const {
        return (m_tail + 1) % this->size() != m_head;
    }

    T& push() {
        T& ref = (*this)[m_tail];
        m_tail = (m_tail + 1) % this->size();
        return ref;
    }

    T& pop() {
        T& ref = (*this)[m_head];
        m_head = (m_head + 1) % this->size();
        return ref;
    }
};
}

#endif // CIRCULARBUFFER_H
