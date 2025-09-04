#include "pch.h"
#include "GarbageItem.h"
#include <shlwapi.h> // Ϊ��ʹ�� SHCreateMemStream
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

    // ʹ�� IStream ���ڴ�������ݣ����Ǵ������ͼƬ��ʽ��ɿ��ķ�ʽ
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
        // ͼƬ���������� m_position ָ����λ�ã���СΪ m_width x m_height
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
    // ���㲢����ͼƬ��ǰλ�õľ�������
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
