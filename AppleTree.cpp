#include "pch.h"
#include "AppleTree.h"
#include "resource.h" // ������ԴID��ͷ�ļ�

CAppleTree::CAppleTree() : m_level(0)
{
    // �ڹ��캯���м�������ͼƬ��Դ
    // ȷ����ЩID�Ѿ��� resource.h �ж��壬����ͼƬ����ΪBitmap��Դ����
    m_treeImages[0].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_0);
    m_treeImages[1].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_1);
    m_treeImages[2].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_2);
    m_treeImages[3].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_3);
    m_treeImages[4].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_4);
    m_treeImages[5].LoadFromResource(AfxGetInstanceHandle(), IDB_TREE_5);
}

void CAppleTree::SetLevel(int level)
{
    // ���õȼ�����ȷ���ȼ���0��5֮��
    if (level >= 0 && level <= 5)
    {
        m_level = level;
    }
}

void CAppleTree::Draw(CDC* pDC, const CRect& rect)
{
    // ���ݵ�ǰ�ȼ������ƶ�Ӧ��ͼƬ
    if (!m_treeImages[m_level].IsNull())
    {
        // CImage::Draw ���Զ�����PNG��͸��ͨ��
        m_treeImages[m_level].Draw(pDC->GetSafeHdc(), rect);
    }
}