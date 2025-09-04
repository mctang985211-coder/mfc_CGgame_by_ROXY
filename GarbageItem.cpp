#include "pch.h"
#include "GarbageItem.h"
#include <shlwapi.h> // 为了使用 SHCreateMemStream
#pragma comment(lib, "shlwapi.lib")

CGarbageItem::CGarbageItem()
    : m_type(_T("")), m_position(0, 0), m_width(100), m_height(100)
{
}

void CGarbageItem::LoadImageFromMemory(const std::vector<BYTE>& imageData)
{
    if (imageData.empty())
    {
        return;
    }

    // 使用 IStream 从内存加载数据，这是处理各种图片格式最可靠的方式
    IStream* pStream = SHCreateMemStream(imageData.data(), static_cast<UINT>(imageData.size()));
    if (pStream)
    {
        if (!m_image.IsNull())
        {
            m_image.Destroy();
        }
        m_image.Load(pStream);
        pStream->Release();
    }
}

void CGarbageItem::Draw(CDC* pDC)
{
    if (!m_image.IsNull())
    {
        // 图片将被绘制在 m_position 指定的位置，大小为 m_width x m_height
        m_image.Draw(pDC->GetSafeHdc(), GetRect());
    }
}

void CGarbageItem::SetPosition(const CPoint& pos)
{
    m_position = pos;
}

CPoint CGarbageItem::GetPosition() const
{
    return m_position;
}

CRect CGarbageItem::GetRect() const
{
    // 计算并返回图片当前位置的矩形区域
    return CRect(m_position, CSize(m_width, m_height));
}

void CGarbageItem::SetType(const CString& type)
{
    m_type = type;
}

CString CGarbageItem::GetType() const
{
    return m_type;
}

void CGarbageItem::Reset()
{
    if (!m_image.IsNull())
    {
        m_image.Destroy();
    }
    m_type = _T("");
}
