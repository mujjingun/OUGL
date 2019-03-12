#include "rawbufferview.h"

const void* RawBufferView::data() const
{
    return m_data;
}

std::size_t RawBufferView::size() const
{
    return m_size;
}

RawBufferView::RawBufferView()
    : m_data(nullptr)
    , m_size(0)
{
}
