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


IMPLEMENT_DYNCREATE(CNewPasEditDoc, CScintillaDoc)


BEGIN_MESSAGE_MAP(CNewPasEditDoc, CScintillaDoc)
END_MESSAGE_MAP()


CNewPasEditDoc::CNewPasEditDoc()
{
}


CNewPasEditDoc::~CNewPasEditDoc()
{
}


#ifdef _DEBUG
void CNewPasEditDoc::AssertValid() const
{
	CScintillaDoc::AssertValid();
}


void CNewPasEditDoc::Dump(CDumpContext& dc) const
{
	CScintillaDoc::Dump(dc);
}
#endif //_DEBUG
