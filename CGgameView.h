#pragma once

#include "TrashBin.h"
#include "GarbageItem.h"
#include "AppleTree.h"
#include <vector>
#include <string>

// 向前声明 PyObject 结构体，避免在头文件中包含 Python.h
struct _object;
typedef _object PyObject;

// 定义一个用于线程通信的自定义Windows消息
#define WM_IMAGE_LOADED (WM_APP + 1)

class CCGgameView : public CView
{
protected: // 仅从序列化创建
    CCGgameView() noexcept;
    DECLARE_DYNCREATE(CCGgameView)

    // 特性
public:
    CCGgameDoc* GetDocument() const;

    // 操作
public:

    // 重写
public:
    virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs); 
protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    // *** 已修正: 将 CInfo 改为 CPrintInfo ***
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

    // 实现
public:
    virtual ~CCGgameView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    // 消息映射函数
    afx_msg void OnGameStart();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg LRESULT OnImageLoaded(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

private:
    // 用于向后台线程传递参数的结构体
    struct ThreadParams
    {
        CCGgameView* pView;
        CString prompt;
    };
    HBITMAP CreateBitmapFromCImage(ATL::CImage& image);
    HBITMAP CreateMaskFromBitmap(HBITMAP hbmImage);
    // 游戏状态
    int m_score;
    int m_round;
    int m_turn;
    int m_upgradePoints;
    bool m_isGameActive;

    // 游戏对象
    std::vector<CTrashBin> m_bins;
    CGarbageItem m_currentItem;
    CAppleTree m_appleTree;
    CPoint m_initialItemPos;

    // 拖拽控制
    bool m_isDragging;
    CPoint m_dragOffset;

    // UI 资源
    CFont m_uiFont;

    // Python C API 对象指针
    PyObject* m_pApiModule; // 指向 api_module 模块
    PyObject* m_pApiFunc;   // 指向 get_image_data 函数

    // 私有方法
    void InitializeGame();
    void SetupBins();
    void NextTurn();
    void CheckDrop(const CPoint& point);
    void EndGame(const CString& reason);
    void UpdateGameState(bool correct);

    // API 调用
    void FetchNewGarbageImage();
    bool CallPythonToFetchImage(const CString& prompt, std::vector<BYTE>& imageData);

    // 调试控制台
    void AttachDebugConsole();

    // 静态的线程工作函数
    static UINT WorkerThreadFunction(LPVOID pParam);
};

#ifndef _DEBUG
inline CCGgameDoc* CCGgameView::GetDocument() const
{
    return reinterpret_cast<CCGgameDoc*>(m_pDocument);
}
#endif
