#pragma once
#include <atlimage.h>
#include <vector>

class CGarbageItem
{
public:
    CGarbageItem();

    void LoadImageFromMemory(const std::vector<BYTE>& imageData);
    void Draw(CDC* pDC);

    void SetPosition(const CPoint& pos);
    CPoint GetPosition() const; // 获取位置
    CRect GetRect() const;
    void SetType(const CString& type);
    CString GetType() const;
    void Reset(); // 用于重置图片状态

private:
    CImage m_image;
    CString m_type;
    CPoint m_position;
    int m_width;
    int m_height;
};
