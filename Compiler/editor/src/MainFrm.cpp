/*
 * Copyright (c) 2012 Valentin Galea
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "stdafx.h"
#include "NewPasEdit.h"
#include "MainFrm.h"
#include "DlgSettings.h"


IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)


BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND(ID_CMD_BUILD, &CMainFrame::OnCmdBuild)
	ON_COMMAND(ID_CMD_RUN, &CMainFrame::OnCmdRun)
	ON_COMMAND(ID_CMD_RUN_GAME, &CMainFrame::OnCmdRunGame)
	ON_COMMAND(ID_BUILD_SETTINGS, &CMainFrame::OnBuildSettings)
	ON_UPDATE_COMMAND_UI(ID_CMD_BUILD, &CMainFrame::OnUpdateCmd)
	ON_UPDATE_COMMAND_UI(ID_CMD_RUN, &CMainFrame::OnUpdateCmd)
	ON_COMMAND(ID_WINDOW_CASCADE, &CMainFrame::OnWindowCascade)
	ON_COMMAND(ID_WINDOW_TILE_HORZ, &CMainFrame::OnWindowTileHorz)
	ON_COMMAND(ID_WINDOW_TILE_VERT, &CMainFrame::OnWindowTileVert)
END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


CMainFrame::CMainFrame()
{
}


CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
	
	//EnableMDITabs();
	//CMDITabInfo mdiTabParams;
	//mdiTabParams.m_style = CMFCTabCtrl::STYLE_FLAT; // other styles available...
	//mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
	//mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
	//mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
	//mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	//EnableMDITabbedGroups(TRUE, mdiTabParams);

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}


#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->Create();
}


LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}


void CMainFrame::OnCmdBuild()
{
	CDocument *pDoc = GetActiveFrame()->GetActiveDocument();
	pDoc->DoFileSave();

	CString strFolder = theApp.CombinePath( theApp.GetPath(), theApp.m_strSDKOutput );
	CString strApp = theApp.CombinePath( theApp.GetPath(), theApp.m_strCompilerApp );
	SetEnvironmentVariable( _T("NEWPAS"), '"' + strApp + '"' );

	CString strArgs;
	// using an environ. var. because cmd /C doesn't support more than 1 quoted path
	strArgs.Format( _T("/C %%NEWPAS%% \"%s\" outvm || pause"), (LPCTSTR)pDoc->GetPathName() );

	::ShellExecute( 0, _T("open"), _T("cmd"), strArgs, strFolder, SW_SHOW );
}


void CMainFrame::OnCmdRun()
{
	CDocument *pDoc = GetActiveFrame()->GetActiveDocument();
	pDoc->DoFileSave();

	CString strApp = theApp.CombinePath( theApp.GetPath(), theApp.m_strCompilerApp );
	SetEnvironmentVariable( _T("NEWPAS"), '"' + strApp + '"' );

	CString strArgs;
	// using an environ. var. because cmd /C doesn't support more than 1 quoted path
	strArgs.Format( _T("/C %%NEWPAS%% \"%s\" execvm & pause"), (LPCTSTR)pDoc->GetPathName() );

	TCHAR szFolder[MAX_PATH];
	_tcscpy_s( szFolder, MAX_PATH, pDoc->GetPathName() );
	PathRemoveFileSpec( szFolder );

	::ShellExecute( 0, _T("open"), _T("cmd"), strArgs, szFolder, SW_SHOW );
}

void CMainFrame::OnCmdRunGame()
{
	::ShellExecute( 0, _T("open"), theApp.CombinePath( theApp.GetPath(), theApp.m_strSDKApp ), 0, 0, SW_SHOW );
}

void CMainFrame::OnBuildSettings()
{
	CDlgSettings dlg;
	dlg.DoModal();
}


void CMainFrame::OnUpdateCmd(CCmdUI *pCmdUI)
{
	if( !GetActiveFrame() )
		return;

	pCmdUI->Enable( GetActiveFrame()->GetActiveDocument() != NULL );
}


void CMainFrame::OnWindowCascade()
{
	MDICascade();
}


void CMainFrame::OnWindowTileHorz()
{
	MDITile(MDITILE_HORIZONTAL);
}


void CMainFrame::OnWindowTileVert()
{
	MDITile(MDITILE_VERTICAL);
}
