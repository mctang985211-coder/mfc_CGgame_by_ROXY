#include "pch.h"
#include "TrashBin.h"

CTrashBin::CTrashBin(const CString& type, const CRect& rect)
    : m_type(type), m_rect(rect), m_isLidOpen(false)
{
}

void CTrashBin::LoadImages(UINT closedImageID, UINT openImageID)
{
    // ����Դ����ͼƬ
    m_imageClosed.LoadFromResource(AfxGetInstanceHandle(), closedImageID);
    m_imageOpen.LoadFromResource(AfxGetInstanceHandle(), openImageID);
}

void CTrashBin::Draw(CDC* pDC)
{
    // ���ݸ���״̬ѡ��Ҫ���Ƶ�ͼƬ
    CImage* pImageToDraw = m_isLidOpen ? &m_imageOpen : &m_imageClosed;

    if (pImageToDraw && !pImageToDraw->IsNull())
    {
        // *** ������: ����û��Alphaͨ����BMPͼƬ, ʹ�� TransparentBlt ***
        // *** �����Խ�ͼƬ�е�ĳһ����ɫ(�����Ǵ���ɫ)����Ϊ͸�� ***
        // *** ע��: �������BMP����ɫ���Ǵ���(255,255,255), ���޸������ RGB ֵ ***
        pImageToDraw->TransparentBlt(pDC->GetSafeHdc(), m_rect, RGB(255, 255, 255));
    }
}

CRect CTrashBin::GetRect() const
{
    return m_rect;
}

CString CTrashBin::GetType() const
{
    return m_type;
}

void CTrashBin::SetLidOpen(bool isOpen)
{
    m_isLidOpen = isOpen;
}

bool CTrashBin::IsLidOpen() const
{
    return m_isLidOpen;
}