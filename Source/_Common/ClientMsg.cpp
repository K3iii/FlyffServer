#include "stdafx.h"
#include "clientMsg.h"
#include "timer.h"

CClientMsg g_ClientMsg;

CClientMsg::CClientMsg()
{
	m_nLineSpace = 1;
	m_nBeginLine =  0;
	m_timer.Set(5000);
	m_pFont = NULL;
	m_pFontEffect = NULL;
}
CClientMsg::~CClientMsg()
{
	ClearAllMessage();
	for(int i = 0; i < m_textArray.GetSize(); i++)
		SAFE_DELETE_CONST((LPCLIENTMSG)m_textArray.GetAt(i));
}
void CClientMsg::SetClientMessage( int nSize, int nLineSpace )
{
	m_nLineSpace = nLineSpace;
	m_nBeginLine =  0;
	m_timer.Set(3000);
}
void CClientMsg::ClearAllMessage()
{
	LPCLIENTMSG lpClientMsg;
	for( int i = 0; i < m_textArray.GetSize(); i++ )
	{
		lpClientMsg = (LPCLIENTMSG)m_textArray.GetAt(i);
		SAFE_DELETE( lpClientMsg );
	}
	m_textArray.RemoveAll();
}
void CClientMsg::Process()
{
	LPCLIENTMSG lpClientMsg;
	for (int i = m_textArray.GetSize() - 1; i >= 0; i--)
	{
		lpClientMsg = (LPCLIENTMSG)m_textArray.GetAt(i);
		if (lpClientMsg)
		{
			if (lpClientMsg->m_nAlpha != 255)
			{
				lpClientMsg->m_nAlpha -= 10;
				if (lpClientMsg->m_nAlpha < 0)
				{
					SAFE_DELETE_CONST((LPCLIENTMSG)m_textArray.GetAt(0));
					m_textArray.RemoveAt(0);
					i--;
				}
			}
		}
	}

	if (m_timer.Over() && m_textArray.GetSize())
	{
		lpClientMsg = (LPCLIENTMSG)m_textArray.GetAt(0);
		if (lpClientMsg)
		{
			m_timer.Reset();
			if (lpClientMsg->m_nAlpha == 255)
				lpClientMsg->m_nAlpha = 254;
		}
	}
}
void CClientMsg::Render(CPoint point, C2DRender* p2DRender)
{
	CSize size;
	m_nLineSpace = -3;
	LPCLIENTMSG lpClientMsg;
	int nLine = 0;
	for( int i = m_textArray.GetSize() - 1; i >= 0 ; i-- )
	{
		lpClientMsg = (LPCLIENTMSG) m_textArray.GetAt( i );
		if(lpClientMsg)
		{
			LPCTSTR lpStr = lpClientMsg->m_string;
			p2DRender->m_pFont->GetTextExtent((TCHAR*)lpStr,&size);
			nLine -= size.cy + m_nLineSpace;

			CPoint pt = point;
			pt.x = pt.x - size.cx / 2;
			pt.y = pt.y + ( ( m_textArray.GetSize() * ( size.cy + m_nLineSpace ) ) / 2 ) + nLine;
			lpClientMsg->m_string.SetColor( ( lpClientMsg->m_nAlpha << 24 ) | ( lpClientMsg->m_dwRGB & 0x00ffffff ) );


			p2DRender->TextOut_EditString( pt.x, pt.y, lpClientMsg->m_string );
		}
	}

}
void CClientMsg::SetLinePos(C2DRender* p2DRender,int nLinePos)
{

}
void CClientMsg::SetRectPage(C2DRender* p2DRender,CRect rect)
{
	ClearAllMessage();
	m_rectPage = rect;
}
void CClientMsg::SetLineNum(C2DRender* p2DRender,int nLineNum)
{
	ClearAllMessage();
	SetSize(nLineNum);
}
void CClientMsg::Scroll()
{

}
void CClientMsg::AddMessage( LPCTSTR lpszMessage, DWORD RGB, BOOL bEffect )
{
#ifdef __AIO_NEUZAPP
	if (!g_Neuz.IsRenderState())
		return;
#endif

	LPCLIENTMSG lpClientMsg = new CLIENTMSG;
	lpClientMsg->m_dwRGB = RGB;
	lpClientMsg->m_string.SetParsingString( lpszMessage, 0xffffffff );
	lpClientMsg->m_pFont = bEffect ? m_pFontEffect : m_pFont;
	lpClientMsg->m_nAlpha = 255;
	//m_textArray.SetAt( m_nAddLineNum, lpClientMsg );
	m_textArray.Add(lpClientMsg);
	int nMax = 6;
	if( m_textArray.GetSize() > nMax )
	{
		SAFE_DELETE_CONST((LPCLIENTMSG)m_textArray.GetAt(0));
		m_textArray.RemoveAt( 0 );
	}
	//m_nAddLineNum++;
	m_timer.Set( 3000 );
}
