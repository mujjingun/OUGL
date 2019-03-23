#ifndef RAWBUFFERVIEW_H
#define RAWBUFFERVIEW_H

#include <cstdint>
#include <vector>

class RawBufferView {
    const void* m_data;
    std::size_t m_size;

public:
    RawBufferView();

    template <typename T, std::size_t N>
    RawBufferView(T (&arr)[N])
        : m_data(arr)
        , m_size(sizeof(T) * N)
    {
    }

    template <typename T>
    RawBufferView(std::vector<T> const& vec)
        : m_data(vec.data())
        , m_size(sizeof(T) * vec.size())
    {
    }

    template <typename T>
    explicit RawBufferView(T const& t)
        : m_data(&t)
        , m_size(sizeof(T))
    {
    }

    const void* data() const;
    std::size_t size() const;
};

#endif // RAWBUFFERVIEW_H
