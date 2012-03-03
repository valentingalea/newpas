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
#include "DlgSettings.h"


IMPLEMENT_DYNAMIC(CDlgSettings, CDialog)


CDlgSettings::CDlgSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSettings::IDD, pParent)
{
}


CDlgSettings::~CDlgSettings()
{
}


void CDlgSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSettings, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSettings::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CDlgSettings::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	GetDlgItem( IDC_PLACEHOLDER )->GetClientRect( rect );
	GetDlgItem( IDC_PLACEHOLDER )->MapWindowPoints( this, &rect );

	m_wndPropList.Create( WS_CHILD | WS_BORDER | WS_VISIBLE | WS_TABSTOP, rect, this, 1234 );
	m_wndPropList.EnableHeaderCtrl( FALSE );
	m_wndPropList.SetVSDotNetLook( TRUE );
	m_wndPropList.SetAlphabeticMode( FALSE );
	m_wndPropList.EnableDescriptionArea();
	
	CMFCPropertyGridProperty *pGroupGeneral = new CMFCPropertyGridProperty( _T( "General" ) );
	m_wndPropList.AddProperty( pGroupGeneral );

	m_pPropCompilerApp = new CMFCPropertyGridFileProperty(
		_T( "Compiler Location" ),
		TRUE,
		theApp.m_strCompilerApp,
		_T("exe"),
		4 | 2,
		0,
		_T("Path to the NewPas compiler. Can be relative to the executable of this editor.")
	);
	pGroupGeneral->AddSubItem( m_pPropCompilerApp );

	m_pPropSDKOutput = new CMFCPropertyGridFileProperty(
		_T( "Output Folder" ),
		theApp.m_strSDKOutput,
		0,
		_T("Output path for VM object code. Can be relative to the executable of this editor.")
	);
	pGroupGeneral->AddSubItem( m_pPropSDKOutput );

	m_pPropSDKApp = new CMFCPropertyGridFileProperty(
		_T( "Game Application" ),
		TRUE,
		theApp.m_strSDKApp,
		_T("exe"),
		4 | 2,
		0,
		_T("Path to the game companion application. Can be relative to the executable of this editor.")
	);
	pGroupGeneral->AddSubItem( m_pPropSDKApp );
	
	return TRUE;
}


void CDlgSettings::OnBnClickedOk()
{
	theApp.m_strCompilerApp = m_pPropCompilerApp->GetValue();
	theApp.m_strSDKApp = m_pPropSDKApp->GetValue();
	theApp.m_strSDKOutput = m_pPropSDKOutput->GetValue();

	OnOK();
}
