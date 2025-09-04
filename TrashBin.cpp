#include "pch.h"
#include "TrashBin.h"

CTrashBin::CTrashBin(const CString& type, const CRect& rect)
    : m_type(type), m_rect(rect), m_isLidOpen(false)
{
}

void CTrashBin::LoadImages(UINT closedImageID, UINT openImageID)
{
    // 从资源加载图片
    m_imageClosed.LoadFromResource(AfxGetInstanceHandle(), closedImageID);
    m_imageOpen.LoadFromResource(AfxGetInstanceHandle(), openImageID);
}

void CTrashBin::Draw(CDC* pDC)
{
    // 根据盖子状态选择要绘制的图片
    CImage* pImageToDraw = m_isLidOpen ? &m_imageOpen : &m_imageClosed;

    if (pImageToDraw && !pImageToDraw->IsNull())
    {
        // *** 已修正: 对于没有Alpha通道的BMP图片, 使用 TransparentBlt ***
        // *** 它可以将图片中的某一种颜色(这里是纯白色)处理为透明 ***
        // *** 注意: 如果您的BMP背景色不是纯白(255,255,255), 请修改下面的 RGB 值 ***
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