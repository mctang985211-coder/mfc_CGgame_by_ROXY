#pragma once
#include <atlimage.h> // ���� CImage ���ͷ�ļ�

class CAppleTree
{
public:
    CAppleTree();

    void SetLevel(int level);
    void Draw(CDC* pDC, const CRect& rect);

private:
    int m_level;
    CImage m_treeImages[6]; // ��һ������洢0��5��������ƻ����ͼƬ
};