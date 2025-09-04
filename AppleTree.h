#pragma once
#include <atlimage.h> // 包含 CImage 类的头文件

class CAppleTree
{
public:
    CAppleTree();

    void SetLevel(int level);
    void Draw(CDC* pDC, const CRect& rect);

private:
    int m_level;
    CImage m_treeImages[6]; // 用一个数组存储0到5级的所有苹果树图片
};