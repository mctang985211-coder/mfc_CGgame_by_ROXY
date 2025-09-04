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
    CPoint GetPosition() const; // ��ȡλ��
    CRect GetRect() const;
    void SetType(const CString& type);
    CString GetType() const;
    void Reset(); // ��������ͼƬ״̬

private:
    CImage m_image;
    CString m_type;
    CPoint m_position;
    int m_width;
    int m_height;
};
