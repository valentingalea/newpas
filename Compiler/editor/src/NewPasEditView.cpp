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
#include "NewPasEditDoc.h"
#include "NewPasEditView.h"


const TCHAR szPascalKeyWords[] = 
	_T("absolute abstract and array as asm assembler automated begin case ")
	_T("cdecl class const constructor deprecated destructor dispid dispinterface div do downto ")
	_T("dynamic else end except export exports external far file final finalization finally for ")
	_T("forward function goto if implementation in inherited initialization inline interface is ")
	_T("label library message mod near nil not object of on or out overload override packed ")
	_T("pascal platform private procedure program property protected public published raise ")
	_T("record register reintroduce repeat resourcestring safecall sealed set shl shr static ")
	_T("stdcall strict string then threadvar to try type unit unsafe until uses var varargs ")
	_T("virtual while with xor ");


IMPLEMENT_DYNCREATE(CNewPasEditView, CScintillaView)


BEGIN_MESSAGE_MAP(CNewPasEditView, CScintillaView)
END_MESSAGE_MAP()


CNewPasEditView::CNewPasEditView()
{
}


CNewPasEditView::~CNewPasEditView()
{
}


BOOL CNewPasEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CScintillaView::PreCreateWindow(cs);
}


#ifdef _DEBUG
void CNewPasEditView::AssertValid() const
{
	CScintillaView::AssertValid();
}


void CNewPasEditView::Dump(CDumpContext& dc) const
{
	CScintillaView::Dump(dc);
}


CNewPasEditDoc* CNewPasEditView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNewPasEditDoc)));
	return (CNewPasEditDoc*)m_pDocument;
}
#endif //_DEBUG


void CNewPasEditView::OnInitialUpdate()
{
	CScintillaView::OnInitialUpdate();

	CScintillaCtrl& rCtrl = GetCtrl();

	//Setup the Lexer
	rCtrl.SetLexer(SCLEX_PASCAL);
	rCtrl.SetKeyWords(0, szPascalKeyWords);

	//Setup styles
	SetAStyle(STYLE_DEFAULT, RGB(0, 0, 0), RGB(0xff, 0xff, 0xff), 10, "Courier New");
	rCtrl.StyleClearAll();
	SetAStyle(SCE_PAS_DEFAULT, RGB(0, 0, 0));
	SetAStyle(SCE_PAS_COMMENT, RGB(0, 0x80, 0));
	SetAStyle(SCE_PAS_COMMENT2, RGB(0, 0x80, 0));
	SetAStyle(SCE_PAS_COMMENTLINE, RGB(0, 0x80, 0));
	SetAStyle(SCE_PAS_NUMBER, RGB(0, 0x80, 0x80));
	SetAStyle(SCE_PAS_WORD, RGB(0, 0, 0x80));
	rCtrl.StyleSetBold(SCE_PAS_WORD, 1);
	SetAStyle(SCE_PAS_STRING, RGB(0x80, 0, 0x80));
	SetAStyle(SCE_PAS_IDENTIFIER, RGB(0, 0, 0));
	SetAStyle(SCE_PAS_PREPROCESSOR, RGB(0x80, 0, 0));
	SetAStyle(SCE_PAS_OPERATOR, RGB(0x80, 0x80, 0));

	//Line margins
	rCtrl.SetMarginWidthN(0, 32);

	//Tabs
	rCtrl.SetTabIndents( TRUE );
	rCtrl.SetTabWidth( 4 );
	rCtrl.SetIndentationGuides( SC_IV_LOOKBOTH );

	//Setup folding
	rCtrl.SetMarginWidthN(2, 16);
	rCtrl.SetMarginSensitiveN(2, TRUE);
	rCtrl.SetMarginTypeN(2, SC_MARGIN_SYMBOL);
	rCtrl.SetMarginMaskN(2, SC_MASK_FOLDERS);
	rCtrl.SetProperty(_T("fold"), _T("1"));

	//Setup markers
	DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0xFF));
	DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
	DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
	DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
	DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
	DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
	DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
}


void CNewPasEditView::SetAStyle(int style, COLORREF fore, COLORREF back, int size, const char* face) 
{
	CScintillaCtrl& rCtrl = GetCtrl();

	rCtrl.StyleSetFore(style, fore);
	rCtrl.StyleSetBack(style, back);
	if (size >= 1)
		rCtrl.StyleSetSize(style, size);
	if (face) 
		rCtrl.StyleSetFont(style, face);
}


void CNewPasEditView::DefineMarker(int marker, int markerType, COLORREF fore, COLORREF back) 
{
	CScintillaCtrl& rCtrl = GetCtrl();

	rCtrl.MarkerDefine(marker, markerType);
	rCtrl.MarkerSetFore(marker, fore);
	rCtrl.MarkerSetBack(marker, back);
}
