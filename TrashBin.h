#pragma once
#include <atlimage.h>

class CTrashBin
{
public:
    CTrashBin(const CString& type, const CRect& rect);

    // 从资源ID加载图片
    void LoadImages(UINT closedImageID, UINT openImageID);
    void Draw(CDC* pDC);

    CRect GetRect() const;
    CString GetType() const;
    void SetLidOpen(bool isOpen);
    bool IsLidOpen() const;

private:
    CRect m_rect;
    CString m_type;
    bool m_isLidOpen;
    CImage m_imageClosed;
    CImage m_imageOpen;
};
