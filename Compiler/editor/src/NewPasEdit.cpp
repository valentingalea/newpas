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
#include "afxwinappex.h"
#include "NewPasEdit.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "NewPasEditDoc.h"
#include "NewPasEditView.h"


BEGIN_MESSAGE_MAP(CNewPasEditApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CNewPasEditApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


CNewPasEditApp::CNewPasEditApp()
{
	m_bHiColorIcons = TRUE;
	m_hScintilla = NULL;

	m_strCompilerApp = _T(".\\newpas.exe");
	m_strSDKOutput = _T("..\\..\\CarPhysics\\bin\\data");
	m_strSDKApp = _T("..\\..\\CarPhysics\\bin\\car.exe");
}


CNewPasEditApp theApp;

BOOL CNewPasEditApp::InitInstance()
{
	m_hScintilla = ::LoadLibrary( _T("SciLexer.dll") );
	if( !m_hScintilla )
	{
		AfxMessageBox( IDS_ERR_NODLL, MB_ICONERROR );
		return FALSE;
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	SetRegistryKey(_T("NewPas"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_NewPasEditTYPE,
		RUNTIME_CLASS(CNewPasEditDoc),
		RUNTIME_CLASS(CChildFrame),       // main MDI frame window
		RUNTIME_CLASS(CNewPasEditView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	m_pMainWnd->DragAcceptFiles( TRUE );

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}


class CAboutDlg : public CDialog
{
public:
	/* constructor */	CAboutDlg();

	enum {				IDD = IDD_ABOUTBOX };

protected:
	virtual void		DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
};


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


void CNewPasEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


int CNewPasEditApp::ExitInstance()
{
	if( m_hScintilla )
		::FreeLibrary( m_hScintilla );

	return CWinAppEx::ExitInstance();
}


CString CNewPasEditApp::GetPath()
{
	TCHAR szFilePath[MAX_PATH];
	GetModuleFileName( theApp.m_hInstance, szFilePath, MAX_PATH );

	PathRemoveFileSpec( szFilePath );
	PathAddBackslash( szFilePath );

	return szFilePath;
}


CString CNewPasEditApp::CombinePath(CString &path1, CString path2)
{
	TCHAR szFilePath[MAX_PATH];

	if( PathIsRelative( path2 ) )
	{
		_tcscpy_s( szFilePath, MAX_PATH, path1 );
		PathAppend( szFilePath, path2 );

		return CString( szFilePath );
	}
	else
		return path2;
}