#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "CGgame.h"
#endif

#include "CGgameDoc.h"
#include "CGgameView.h"
#include "resource.h"
#include <random>
#include <iostream>

// 包含 Python C API 头文件
#include <Python.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCGgameView, CView)

BEGIN_MESSAGE_MAP(CCGgameView, CView)
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    // --- 自定义消息映射 ---
    ON_COMMAND(ID_GAME_START, &CCGgameView::OnGameStart)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    ON_MESSAGE(WM_IMAGE_LOADED, &CCGgameView::OnImageLoaded)
END_MESSAGE_MAP()

CCGgameView::CCGgameView() noexcept
    : m_score(0), m_round(1), m_turn(1), m_upgradePoints(0),
    m_isGameActive(false), m_isDragging(false),
    m_pApiModule(NULL), m_pApiFunc(NULL)
{
    AttachDebugConsole();
}

CCGgameView::~CCGgameView()
{
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    Py_XDECREF(m_pApiFunc);
    Py_XDECREF(m_pApiModule);

    Py_Finalize();
}

BOOL CCGgameView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

int CCGgameView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_uiFont.CreatePointFont(120, _T("微软雅黑"));

    Py_Initialize();

    // *** 已移除: PyEval_InitThreads() 在 Python 3.9+ 中已弃用且不再需要 ***

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('.')");

    m_pApiModule = PyImport_ImportModule("api_module");
    if (m_pApiModule)
    {
        m_pApiFunc = PyObject_GetAttrString(m_pApiModule, "get_image_data");
        if (!m_pApiFunc || !PyCallable_Check(m_pApiFunc))
        {
            PyErr_Print();
            AfxMessageBox(_T("错误：无法在'api_module'中找到'get_image_data'函数。"));
        }
    }
    else
    {
        PyErr_Print();
        AfxMessageBox(_T("错误：无法导入Python模块 'api_module.py'。"));
    }

    // 主线程完成初始化后，必须释放GIL，以供后台线程使用
    PyEval_SaveThread();

    return 0;
}

void CCGgameView::OnDraw(CDC* pDC)
{
    CCGgameDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    CRect clientRect;
    GetClientRect(&clientRect);

    CDC memDC;
    memDC.CreateCompatibleDC(pDC);
    CBitmap memBitmap;
    memBitmap.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

    memDC.FillSolidRect(&clientRect, RGB(240, 248, 255));
    m_appleTree.Draw(&memDC, CRect(20, 20, 220, 220));

    if (m_isGameActive)
    {
        for (auto& bin : m_bins)
        {
            bin.Draw(&memDC);
        }
        m_currentItem.Draw(&memDC);
    }

    memDC.SelectObject(&m_uiFont);
    memDC.SetBkMode(TRANSPARENT);
    CString text;
    text.Format(_T("分数: %d | 轮次: %d/5 | 回合: %d/10 | 升级点: %d"), m_score, m_round, m_turn, m_upgradePoints);
    memDC.TextOut(250, 20, text);

    if (!m_isGameActive)
    {
        memDC.TextOut(clientRect.CenterPoint().x - 100, clientRect.CenterPoint().y - 50, _T("点击 游戏->开始新游戏"));
    }

    pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBitmap);
}

void CCGgameView::OnGameStart()
{
    InitializeGame();
}

void CCGgameView::InitializeGame()
{
    SetupBins();

    m_score = 3;
    m_round = 1;
    m_turn = 1;
    m_appleTree.SetLevel(0);
    m_upgradePoints = 0;
    m_isGameActive = true;
    m_isDragging = false;

    NextTurn();
}

void CCGgameView::SetupBins()
{
    m_bins.clear();
    CRect clientRect;
    GetClientRect(&clientRect);

    if (clientRect.Width() <= 0) return;

    int binWidth = 150;
    int binHeight = 200;
    int spacing = (clientRect.Width() - 4 * binWidth) / 5;
    int top = clientRect.bottom - binHeight - 50;

    m_bins.emplace_back(_T("recyclable"), CRect(CPoint(spacing, top), CSize(binWidth, binHeight)));
    m_bins.emplace_back(_T("hazardous"), CRect(CPoint(spacing * 2 + binWidth, top), CSize(binWidth, binHeight)));
    m_bins.emplace_back(_T("kitchen"), CRect(CPoint(spacing * 3 + binWidth * 2, top), CSize(binWidth, binHeight)));
    m_bins.emplace_back(_T("other"), CRect(CPoint(spacing * 4 + binWidth * 3, top), CSize(binWidth, binHeight)));

    m_bins[0].LoadImages(IDB_BIN_RECYCLABLE_C, IDB_BIN_RECYCLABLE_O);
    m_bins[1].LoadImages(IDB_BIN_HAZARDOUS_C, IDB_BIN_HAZARDOUS_O);
    m_bins[2].LoadImages(IDB_BIN_KITCHEN_C, IDB_BIN_KITCHEN_O);
    m_bins[3].LoadImages(IDB_BIN_OTHER_C, IDB_BIN_OTHER_O);
}


void CCGgameView::NextTurn()
{
    if (!m_isGameActive) return;

    m_currentItem.Reset();
    FetchNewGarbageImage();

    CRect clientRect;
    GetClientRect(&clientRect);
    m_initialItemPos.SetPoint(clientRect.CenterPoint().x - 50, 150);
    m_currentItem.SetPosition(m_initialItemPos);

    Invalidate();
}

void CCGgameView::FetchNewGarbageImage()
{
    std::vector<std::pair<CString, CString>> garbageList = {
        {_T("recyclable"), _T("踩扁的塑料矿泉水瓶")},
        {_T("hazardous"), _T("废旧的红色油漆桶")},
        {_T("kitchen"), _T("吃剩的苹果核")},
        {_T("other"), _T("用过的脏卫生纸")}
    };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, static_cast<int>(garbageList.size() - 1));
    int index = distrib(gen);

    m_currentItem.SetType(garbageList[index].first);
    CString prompt = garbageList[index].second;

    ThreadParams* pParams = new ThreadParams;
    pParams->pView = this;
    pParams->prompt = prompt;

    AfxBeginThread(WorkerThreadFunction, pParams, THREAD_PRIORITY_NORMAL);
}

UINT CCGgameView::WorkerThreadFunction(LPVOID pParam)
{
    ThreadParams* pParams = static_cast<ThreadParams*>(pParam);
    if (!pParams || !pParams->pView)
    {
        delete pParams;
        return 1;
    }

    CCGgameView* pView = pParams->pView;
    CString prompt = pParams->prompt;

    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    auto pImageData = new std::vector<BYTE>();
    if (pView->CallPythonToFetchImage(prompt, *pImageData))
    {
        pView->PostMessage(WM_IMAGE_LOADED, (WPARAM)TRUE, (LPARAM)pImageData);
    }
    else
    {
        delete pImageData;
        pView->PostMessage(WM_IMAGE_LOADED, (WPARAM)FALSE, (LPARAM)NULL);
    }

    PyGILState_Release(gstate);

    delete pParams;
    return 0;
}


LRESULT CCGgameView::OnImageLoaded(WPARAM wParam, LPARAM lParam)
{
    BOOL bSuccess = (BOOL)wParam;
    if (bSuccess)
    {
        auto pImageData = (std::vector<BYTE>*)lParam;
        m_currentItem.LoadImageFromMemory(*pImageData);
        delete pImageData;
        Invalidate();
    }
    else
    {
        AfxMessageBox(_T("通过 Python 获取图片失败！请检查控制台输出获取详细错误。"));
    }
    return 0;
}


void CCGgameView::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_isGameActive && m_currentItem.GetRect().PtInRect(point))
    {
        m_isDragging = true;
        m_dragOffset = point - m_currentItem.GetPosition();
        SetCapture();
    }
    CView::OnLButtonDown(nFlags, point);
}

void CCGgameView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_isDragging)
    {
        m_currentItem.SetPosition(point - m_dragOffset);
        for (auto& bin : m_bins)
        {
            bin.SetLidOpen(bin.GetRect().PtInRect(point));
        }
        Invalidate();
    }
    CView::OnMouseMove(nFlags, point);
}

void CCGgameView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_isDragging)
    {
        m_isDragging = false;
        ReleaseCapture();

        CheckDrop(point);

        for (auto& bin : m_bins)
        {
            bin.SetLidOpen(false);
        }

        if (m_isGameActive)
        {
            m_currentItem.SetPosition(m_initialItemPos);
        }

        Invalidate();
    }
    CView::OnLButtonUp(nFlags, point);
}

void CCGgameView::CheckDrop(const CPoint& point)
{
    bool droppedInBin = false;
    for (const auto& bin : m_bins)
    {
        if (bin.GetRect().PtInRect(point))
        {
            droppedInBin = true;
            UpdateGameState(bin.GetType() == m_currentItem.GetType());
            break;
        }
    }
}

void CCGgameView::UpdateGameState(bool correct)
{
    if (correct)
    {
        m_score++;
        AfxMessageBox(_T("正确!"));
    }
    else
    {
        m_score--;
        AfxMessageBox(_T("错误!"));
    }

    if (m_score <= 0)
    {
        EndGame(_T("分数耗尽"));
        return;
    }

    if (m_score >= 10 && m_upgradePoints < 5)
    {
        m_score -= 10;
        m_upgradePoints++;
        m_appleTree.SetLevel(m_upgradePoints);
        AfxMessageBox(_T("恭喜！苹果树升级了！"));
    }

    m_turn++;
    if (m_turn > 10)
    {
        m_turn = 1;
        m_round++;
        if (m_round > 5)
        {
            EndGame(_T("所有轮次完成"));
            return;
        }
    }
    NextTurn();
}

void CCGgameView::EndGame(const CString& reason)
{
    m_isGameActive = false;
    CString finalMessage;
    finalMessage.Format(_T("游戏结束！原因：%s\n您的最终分数是: %d"), reason, m_score);
    AfxMessageBox(finalMessage);
    Invalidate();
}

bool CCGgameView::CallPythonToFetchImage(const CString& prompt, std::vector<BYTE>& imageData)
{
    if (!m_pApiFunc)
    {
        return false;
    }

    bool success = false;
    PyObject* pArgs = PyTuple_New(1);
    PyObject* pValue = PyUnicode_FromWideChar(prompt, prompt.GetLength());
    if (!pValue) {
        Py_DECREF(pArgs);
        PyErr_Print();
        return false;
    }
    PyTuple_SetItem(pArgs, 0, pValue);

    PyObject* pResult = PyObject_CallObject(m_pApiFunc, pArgs);
    Py_DECREF(pArgs);

    if (pResult != NULL && PyBytes_Check(pResult))
    {
        char* buffer = PyBytes_AsString(pResult);
        size_t size = PyBytes_Size(pResult);
        imageData.assign(buffer, buffer + size);
        success = true;
    }
    else
    {
        PyErr_Print();
    }

    Py_XDECREF(pResult);
    return success;
}

void CCGgameView::AttachDebugConsole()
{
#ifdef _DEBUG
    if (GetConsoleWindow() == NULL)
    {
        if (AllocConsole())
        {
            FILE* pCout;
            freopen_s(&pCout, "CONOUT$", "w", stdout);
            freopen_s(&pCout, "CONIN$", "r", stdin);
            freopen_s(&pCout, "CONOUT$", "w", stderr);
            SetConsoleTitle(_T("调试输出窗口"));
            std::cout.clear();
            std::cerr.clear();
            std::cin.clear();
            printf("调试控制台已附加。\n");
        }
    }
#endif
}

BOOL CCGgameView::OnPreparePrinting(CPrintInfo* pInfo) { return DoPreparePrinting(pInfo); }
void CCGgameView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {}
void CCGgameView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {}

#ifdef _DEBUG
void CCGgameView::AssertValid() const { CView::AssertValid(); }
void CCGgameView::Dump(CDumpContext& dc) const { CView::Dump(dc); }
CCGgameDoc* CCGgameView::GetDocument() const
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGgameDoc)));
    return (CCGgameDoc*)m_pDocument;
}
#endif