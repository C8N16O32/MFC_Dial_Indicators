/*
没有用到其他资源文件
*/

/*stdafx.h的内容：
#pragma once

#include "targetver.h"

#include "afxwin.h"
*/


//mfc程序架构/绘图架构

#include "stdafx.h"

#define MSPC 10

//程序框架
class CMyFrameWnd : public CFrameWnd {
	DECLARE_MESSAGE_MAP()
public:
	int OnCreate(LPCREATESTRUCT p);
	void OnMouseMove(UINT nKey, CPoint pt);
	void OnLButtonUp(UINT nKey, CPoint pt);
	void OnLButtonDown(UINT nKey, CPoint pt);
	void OnTimer(UINT_PTR ptr);
	void OnSize(UINT uint, int x, int y);
	void OnPaint();
	void doublebuffer(CDC *pDC);
	UINT nKey; CPoint pt;
	CSize screensize = { 1280,720 };
	time_t timems, timemsold;
};

//消息反射宏表
BEGIN_MESSAGE_MAP(CMyFrameWnd, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

//窗口类
class CMyWinApp : public CWinApp {
public:
	CMyWinApp() {};
	virtual BOOL InitInstance() {
		CMyFrameWnd* pFrame = new CMyFrameWnd;
		pFrame->Create(NULL, (LPCTSTR)_T("百分表"));
		m_pMainWnd = pFrame;
		pFrame->ShowWindow(SW_SHOW);
		pFrame->UpdateWindow();
		return TRUE;
	}
};
template<class T2>void H2(T2 x) { return; }

//界面类
class BFB {
	//外部变量快照
	CSize screensize = { 1280,720 };
	void getdataformmyframe(CMyFrameWnd* that) {
		screensize = that->screensize;
		return;
	}

	//变量
	double pi = acos(-1);
	//物体变化范围（-2mm在800*600时对应1像素
	double ysmin = -0.182, ysmax = 10.2;
	//杆行程范围：
	double ypmin = -0.182, ypmax = 10.2;
	//被测物 表针1 表针2 探测杆
	double ysliderdefault = 0, yslider = ysliderdefault, arc1default = 0, arc1 = 0, arc2 = 0, ypol = 0;

#define RS 11
	int rsize = RS;
	char rtitle[RS][16] = { "背景", "探头", "百分表", "被测物初始", "加号", "减号", "下方文本区", "文本样例1", "文本放大", "文本读数", "文本读数显示" };
	enum rtitle_enum { 背景, 探头, 百分表, 被测物, 加号, 减号, 下方文本区, 文本样例1, 文本放大, 文本读数, 文本读数显示 };
	CRect rtemplate[RS] = { { 0,0,800,600 },{ 352,355,380,415 },{ 248,27,479,355 },{ 295,415,431,466 },{ 611,85,711,185 },{ 611,309,711,409 },{ 0,476,800,600 },{ 21,481,202,509 },{ 571,481,752,509 },{ 158,554,251,592 },{ 264,518,535,592 }
	}, rrect[RS] = {};
	CPoint poltemplate[16] = { { 352,30 },{ 349,34 },{ 349,336 },{ 355,356 },{ 376,356 },{ 382,336 },{ 382,34 },{ 379,30 },{ 356,355 },{ 356,406 },{ 375,406 },{ 375,355 },{ 356,397 },{ 356,415 },{ 375,415 },{ 375,397 }
	}, pol[16] = {};


#define CS 7
	int csize = CS;
	enum ctitle_enum { NC = 0, 白, 蓝, 浅蓝, 黑, 米绿, 灰 };
	COLORREF clist[CS] = { RGB(1,1,1),RGB(255,255,255),RGB(54,154,254),RGB(146,161,235),RGB(0,0,0),RGB(228,239,220),RGB(128,128,128) };
	int crecti[RS] = { 白 ,NC, NC, 米绿 ,灰 ,灰 ,蓝 ,NC ,NC ,NC ,浅蓝 };
	int frect[RS] = { };

#define ARC2 10
	//算法
	void coremain() {
		double y = yslider;
		auto limit = [&y](double min, double max)->void {
			if (y < min)y = min; if (y>max)y = max; return; };
		limit(ysmin, ysmax);
		yslider = y;
		limit(ypmin, ypmax);
		ypol = y;
		arc1 = -2 * pi / 1 * y;//1毫米转一圈
		arc2 = 2 * pi / ARC2 * y;//6毫米转一圈
		return;
	}

	//绘图
	void pscale() {
		CRect ss = { 0,0,screensize.cx,screensize.cy };
		int mul, div, cx0 = 800, cy0 = 600;
		if (ss.Width() < 400 || ss.Height() < 300)ss = { 0,0,400,300 }, mul = 1, div = 2;
		else {
			mul = ss.Width(); div = cx0;
			if (mul*1.0 / div > ss.Height()*1.0 / cy0)mul = ss.Height(), div = cy0;
		}
		int i;
		for (i = 0; i < rsize; i++)
			rrect[i] = rtemplate[i].MulDiv(mul, div);
		for (i = 0; i < 16; i++)
			pol[i] = { poltemplate[i].x * mul / div, poltemplate[i].y*mul / div};

		int offx = (screensize.cx - rrect[0].Width()) / 2;
		for (i = 0; i < rsize; i++)
			rrect[i].OffsetRect(offx, 0);
		for (i = 0; i < 16; i++)
			pol[i].Offset(offx, 0);
		return;
	};

	void ptext(CDC*pDC, char*text, CRect rect, COLORREF textrgb = RGB(0, 0, 0)) {
		CFont cfdb; cfdb.CreatePointFont(rect.Height() * 7, (LPCTSTR)_T("黑体"));
		pDC->SelectObject(cfdb); pDC->SetTextColor(textrgb);
		pDC->TextOut(rect.CenterPoint().x - pDC->GetTextExtent(text).cx / 2
			, rect.CenterPoint().y - pDC->GetTextExtent(text).cy / 2, text);
		cfdb.DeleteObject();
		return;
	}

	void pclock(CDC*pDC, CPoint pc, CPoint pc2, double r, double r2, COLORREF color = RGB(0, 0, 0)) {
		//画笔
		double arc0 = -arc1default + pi / 2;
		int wd = 1 + r / 250, wm = wd * 3;//线宽
		CPen penmark, pen; penmark.CreatePen(PS_SOLID, wm, RGB(0, 0, 0)); pen.CreatePen(PS_SOLID, wd, RGB(0, 0, 0));
		//圆形
		auto setrect = [](CRect &rect, CPoint pc, double r) {
			rect.SetRect((int)(pc.x - r + 0.5), (int)(pc.y - r + 0.5), (int)(pc.x + r + 0.5), (int)(pc.y + r + 0.5));
		};
		CRect rect, rect2; setrect(rect, pc, r); setrect(rect2, pc2, r2);
		auto dcircle = [=](CPen *pen, CRect r0, double shrinkrate = 1) {
			CPen *oldPen = pDC->SelectObject(pen);
			int dl = r0.Width() / 2 * (1 - shrinkrate) + 0.5;
			r0.DeflateRect(dl, dl, dl, dl);
			pDC->Ellipse(&r0);
			pDC->SelectObject(oldPen);
		};
		//线
		double notplot = -1; int rnum = 6;
		enum title_seq { wmax, wmin3, wmin2, wmin1, text, innercircle }; double sl[] = { 1,0.9,0.83,0.8,0.75,0.6 }; double ss[] = { 1,-1,-1,0.75,0.6,-1 };//坐标ρ
		char textl[][16] = { "0","10", "20", "30", "40", "50", "60", "70", "80", "90" }, texts[ARC2][16] = { "0","1","2","3","4","5","6","7","8","9" };
		auto calp = [=](CPoint pc, double phi, double r) ->CPoint {
			phi += arc0;
			double x = pc.x + cos(phi)*r;
			double y = pc.y - sin(phi)*r;
			return{ (int)(x + 0.5),(int)(y + 0.5) };
		};
		auto dline_one = [=](CPen *pen, CPoint p1, CPoint p2) {
			CPen *oldPen = pDC->SelectObject(pen);
			pDC->MoveTo(p1); pDC->LineTo(p2);
			pDC->SelectObject(oldPen);
		};

		//绘制
		auto polmain = [&]() {
			int i;
			dline_one(&penmark, pol[0], pol[7]);
			for (i = 1; i < 12; i++) {
				if (i == 8)continue;
				dline_one(&penmark, pol[i], pol[i - 1]);
			}
			dcircle(&penmark, { pol[12],pol[14] });
		};
		auto mainclock = [&](CPoint pc, double r) {
			dline_one(&penmark, pc, calp(pc, arc1, r*sl[wmin3]));//指针

			int lc = 0; double phi = 0, tl; CRect tr; CPoint tc1, tc2;
			for (int i = 0; i < 100; i++) {
				phi = -pi * 2 / 100 * i;
				lc = 1; if (i % 5 == 0)lc = 2; if (i % 10 == 0)lc = 3;
				dline_one(&pen, calp(pc, phi, r*sl[0]), calp(pc, phi, r*sl[lc]));//刻度
				if (lc == 3) {//文本
					tl = r*(sl[5] - sl[3]);
					tc1 = calp(pc, phi, r*sl[text]); tc2 = tc1;
					tc1.Offset(-tl / 2, -tl / 2); tc2.Offset(tl / 2, tl / 2);
					tr.SetRect(tc1, tc2);
					ptext(pDC, textl[i / 10], tr);
				}
			}
		};
		auto altclock = [&](CPoint pc, double r) {
			dline_one(&penmark, pc, calp(pc, arc2, r*sl[wmin3]));//指针

			int lc = 0; double phi = 0, tl; CRect tr; CPoint tc1, tc2;
			for (int i = 0; i < ARC2; i++) {
				phi = pi * 2 / ARC2 * i;
				lc = 3;
				dline_one(&pen, calp(pc, phi, r*ss[0]), calp(pc, phi, r*ss[lc]));//刻度
				if (lc == 3) {//文本
					tl = r*0.5;
					tc1 = calp(pc, phi, r*ss[text]); tc2 = tc1;
					tc1.Offset(-tl / 2, -tl / 2); tc2.Offset(tl / 2, tl / 2);
					tr.SetRect(tc1, tc2);
					ptext(pDC, texts[i], tr);
				}
			}
		};
		polmain();
		dcircle(&penmark, rect);//盘面
		dcircle(&pen, rect, sl[innercircle]);//盘面内边缘
		dcircle(&penmark, rect2);//盘面
		mainclock(pc, r);
		altclock(pc2, r2);

		return;
	}

	void pmain(CDC *pDC) {
		//数据运算
		auto core = [&] {
			double factor = -1.0 / 600 * rrect[背景].Height();
			//探头：整体移动
			int offy = ypol*factor + 0.5;
			rrect[探头].OffsetRect(0, offy);
			for (int i = 9; i < 16; i++)if (i != 11)pol[i].Offset(0, offy);
			//被测物：上边缘移动
			rrect[被测物].top += (yslider - ysliderdefault)*factor + 0.5;
		};
		//纯色背景
		auto printbg = [&](CDC *pDC) {
			for (int i = 0; i < RS; i++)if (!frect[i] && crecti[i]>0)
				pDC->FillSolidRect(rrect[i], clist[crecti[i]]);
		};
		//文本
		auto text_main = [=](CDC*pDC) {
			static char text[RS][16] = {
				"","","","","+","-"
				,"","百分表","视图缩放","读数：",""
			};
			for (int i = 0; i < rsize; i++)if (strlen(text[i])>0)
				ptext(pDC, text[i], rrect[i]);
			if (!frect[文本读数显示])return;
			double v = ypol + 1 * arc1default*(2 * pi); if (v<0 && v>-0.0005)v = 0;
			char temp[16] = ""; sprintf(temp, "%.3lfmm", v);
			ptext(pDC, temp, rrect[文本读数显示], RGB(255, 255, 255));
			return;
		};
		//表盘
		auto clock_main = [=](CDC*pDC) {
			auto calp = [](CPoint pc, double phi, double r) ->CPoint {
				phi += acos(-1) / 2;
				double x = pc.x + cos(phi)*r;
				double y = pc.y - sin(phi)*r;
				return{ (int)(x + 0.5),(int)(y + 0.5) };
			};
			CPoint c1, c2; double r1, r2;
			c1 = rrect[百分表].CenterPoint(); r1 = rrect[百分表].Width() / 2;
			if (frect[文本放大]) {
				int x = rrect[下方文本区].top / 2;
				c1.SetPoint(rrect[背景].CenterPoint().x, x); r1 = x;
			}
			c2 = calp(c1, -200 * pi / 180, r1*0.4); r2 = r1*0.2;
			pclock(pDC, c1, c2, r1, r2);
		};

		core();
		printbg(pDC);
		clock_main(pDC);
		text_main(pDC);
		return;
	}

public:

	//交互 可更改变量列表:yslider frect;
	void fmain(CPoint pt, int isclick = 0) {
		//按钮:加减
		double mmps = 0.5, mspc = MSPC;int rlen = 11;
		auto fr = [=](int dif)->int {int r = dif / (rrect[加号].Height() / rlen); if (r >= rlen)r = rlen - 1; return r; };
		auto mmpc = [=](int k, int r)->double {double rlist[] = { 20,10,5,2,1,5e-1,2e-1,1e-1,5e-2,2e-2,5e-3 };
		if (k == 1)return mmps *mspc / 1000 * rlist[r]; return -mmps *mspc / 1000 * rlist[rlen - 1 - r]; };
		auto fyslider = [&](int k, int dif) {//加号1减号-1
			double y;
			auto limit = [&y](double min, double max)->void {
				if (y < min)y = min; if (y>max)y = max; return; };
			y = yslider;
			y += mmpc(k, fr(dif));
			limit(ysmin, ysmax);
			yslider = y;
		};

		//遍历
		int k = -1;
		for (int i = rsize - 1; i >= 0; i--)
			if (rrect[i].PtInRect(pt)) { k = i; break; }

		//作动
		if(isclick)
			switch (k) {
			case -1:break;
			case 文本读数显示:frect[文本读数显示] = !frect[文本读数显示]; break;
			case 文本放大:frect[文本放大] = !frect[文本放大]; break;
			default:break;
			}
		else
			switch (k) {
			case -1:break;
			case 加号:fyslider(1, pt.y - rrect[加号].top); break;
			case 减号:fyslider(-1, pt.y - rrect[减号].top); break;
			default:break;
			}
		return;
	}



	//主函数
	void main(CMyFrameWnd* that, CDC *pDC) {
		getdataformmyframe(that);
		pscale();
		coremain();
		pmain(pDC);
		return;
	}

#undef RS
#undef CS
#undef ARC2
};

//窗口全局对象创建
CMyWinApp theApp;

//界面全局对象创建
BFB bfb;




//消息反射函数
int CMyFrameWnd::OnCreate(LPCREATESTRUCT p) {
	//TODO: 设置定时器
	SetTimer(1, MSPC, NULL);
	time_t timems = clock();
	timemsold = timems;
	return CFrameWnd::OnCreate(p);
}
void CMyFrameWnd::OnMouseMove(UINT nKey, CPoint ptinput) {
	pt = ptinput;
	return CFrameWnd::OnMouseMove(nKey, pt);
}
void CMyFrameWnd::OnLButtonUp(UINT nKey, CPoint pt) {
	this->nKey = nKey;
	return CFrameWnd::OnLButtonUp(nKey, pt);
}
void CMyFrameWnd::OnLButtonDown(UINT nKey, CPoint pt) {
	this->nKey = nKey;
	bfb.fmain(pt, 1);
	return CFrameWnd::OnLButtonDown(nKey, pt);
}
void CMyFrameWnd::OnTimer(UINT_PTR ptr) {
	//TODO: 在其他需要重绘的反射中添加重绘
	CRect temp; temp.SetRect(0, 0, screensize.cx, screensize.cy);
	if ((nKey&MK_LBUTTON) == MK_LBUTTON)
		bfb.fmain(pt, 0);
	::InvalidateRect(this->m_hWnd, &temp, FALSE);
	return CFrameWnd::OnTimer(ptr);
}
void CMyFrameWnd::OnSize(UINT uint, int x, int y) {
	screensize.cx = x; screensize.cy = y;
	return CFrameWnd::OnSize(uint, x, y);
}

//双缓冲刷新
void CMyFrameWnd::doublebuffer(CDC *pDC) {
	
	CDC MemDC; CBitmap MemBitmap;
	
	MemDC.CreateCompatibleDC(NULL);
	MemBitmap.CreateCompatibleBitmap(pDC, screensize.cx, screensize.cy);
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);

	MemDC.FillSolidRect(0, 0, screensize.cx, screensize.cy, RGB(196, 196, 240));
	MemDC.SetBkMode(TRANSPARENT);

	//绘图
	timems = clock();
	bfb.main(this, &MemDC);
	//TODO: 绘图部分
	timemsold = timems;

	pDC->BitBlt(0, 0, screensize.cx, screensize.cy, &MemDC, 0, 0, SRCCOPY);

	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
	return;
}

//绘图消息反射
void CMyFrameWnd::OnPaint() {
	CDC *pDC = GetDC();
	doublebuffer(pDC);
	this->ReleaseDC(pDC);
	return CFrameWnd::OnPaint();
}

