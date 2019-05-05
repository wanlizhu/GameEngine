#pragma once

template<typename T>
class DirtyData
{
public:
    DirtyData() : m_dirty(false)
    {
        m_origData = T();
        m_newData = T();
    }
    virtual ~DirtyData() = default;

    void SetData(const T& data)
    {
        m_newData = data;
        m_dirty = true;
    }

    const T& GetData() const
    {
        return m_origData;
    }

    void ApplyData() const
    {
        if (m_dirty)
        {
            m_origData = m_newData;
            m_dirty = false;
        }
    }

    bool IsDirty() const
    {
        return m_dirty;
    }

private:
    T m_origData;
    T m_newData;

    bool m_dirty;
};