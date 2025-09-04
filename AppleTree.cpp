#include "pch.h"
#include "AppleTree.h"
#include "resource.h" // 包含资源ID的头文件

CAppleTree::CAppleTree() : m_level(0)
{
    // 在构造函数中加载所有图片资源
    // 确保这些ID已经在 resource.h 中定义，并且图片已作为Bitmap资源导入
    m_treeImages[0].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_0);
    m_treeImages[1].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_1);
    m_treeImages[2].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_2);
    m_treeImages[3].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_3);
    m_treeImages[4].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_4);
    m_treeImages[5].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_5);
}

void CAppleTree::SetLevel(int level)
{
    // 设置等级，并确保等级在0到5之间
    if (level >= 0 && level <= 5)
    {
        m_level = level;
    }
}

void CAppleTree::Draw(CDC* pDC, const CRect& rect)
{
    // 根据当前等级，绘制对应的图片
    if (!m_treeImages[m_level].IsNull())
    {
        // CImage::Draw 会自动处理PNG的透明通道
        m_treeImages[m_level].Draw(pDC->GetSafeHdc(), rect);
    }
}