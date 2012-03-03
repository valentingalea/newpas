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

#pragma once


class CMainFrame : public CMDIFrameWndEx
{
protected:
	DECLARE_DYNAMIC(CMainFrame)
	DECLARE_MESSAGE_MAP()

public:
	/* constructor */	CMainFrame();
	virtual				~CMainFrame();

	virtual BOOL		PreCreateWindow(CREATESTRUCT& cs);

#ifdef _DEBUG
	virtual void		AssertValid() const;
	virtual void		Dump(CDumpContext& dc) const;
#endif

protected:
	CMFCMenuBar			m_wndMenuBar;
	CMFCToolBar			m_wndToolBar;
	CMFCStatusBar		m_wndStatusBar;

	afx_msg	int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void		OnViewCustomize();
	afx_msg LRESULT		OnToolbarCreateNew(WPARAM wp, LPARAM lp);

	afx_msg void		OnCmdBuild();
	afx_msg void		OnCmdRun();
	afx_msg void		OnCmdRunGame();
	afx_msg void		OnBuildSettings();
	afx_msg void		OnUpdateCmd(CCmdUI *pCmdUI);
	afx_msg void		OnWindowCascade();
	afx_msg void		OnWindowTileHorz();
	afx_msg void		OnWindowTileVert();
};

