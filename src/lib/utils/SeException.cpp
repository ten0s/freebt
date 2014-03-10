// Copyright (c) 2004, Antony C. Roberts

// Use of this file is subject to the terms
// described in the LICENSE.TXT file that
// accompanies this file.
//
// Your use of this file indicates your
// acceptance of the terms described in
// LICENSE.TXT.
//
// http://www.freebt.net

#include <eh.h>
#include "fbtSeXcpt.h"

void fbtXcptSETranslator(unsigned int nSeCode, _EXCEPTION_POINTERS* pExcPointers)
{
	throw new fbtSeException(nSeCode,pExcPointers);
}

void fbtXcptEnableSEHandling()
{
	_set_se_translator(fbtXcptSETranslator);
}

fbtSeException::fbtSeException(unsigned int nSeCode, _EXCEPTION_POINTERS* pExcPointers)
{
	m_nSeCode = nSeCode;
	m_pExcPointers = pExcPointers;
}

fbtSeException::fbtSeException(fbtSeException & CseExc)
{
	m_nSeCode = CseExc.m_nSeCode;
	m_pExcPointers = CseExc.m_pExcPointers;
}

unsigned int fbtSeException::GetSeCode()
{
	return m_nSeCode;
}
