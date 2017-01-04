// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__F930E632_47A0_45A1_9A76_82D0D39293B4__INCLUDED_)
#define AFX_CHILDVIEW_H__F930E632_47A0_45A1_9A76_82D0D39293B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CChildView window

typedef struct oneline{
	char loc[10];  //주소
	char label[10]; //라벨
	char opcode[10]; //opcode
	char operand[10]; //operand
}Line;

typedef struct SymTab {
	char name[10];	//라벨 이름
	int value; //10진수 주소값
	char m_value[10]; //16진수 주소값
}Symtab;
class CChildView : public CWnd
{
// Construction
public:
	CChildView();
	CStringList m_list;
	int Locctr; //10진수로 주소값을 추가하기위해 누적시키는 주소값
	char ch_locctr[5]; //16진수로 변환시켜 입력받을 변수
	int programLength; //10진수로 프로그램 총 길이를 입력받을 변수
	char ch_programLength[5]; //16진수로 프로그램 총 길이를 입력받을 변수
	int startaddress; //시작주소
	int returnsymval; //symtab에 operand를 검사하여 입력받을 라벨의 주소값
	int normal; //목적프로그램에서 텍스트 테이블 한 라인의 길이를 측정할 값
	int sympass1; //pass1에서 symtab과 비교하여 오류검사(duplicate symbol) 플래그
	int sympass2; //pass2에서 symtab과 비교하여 오류검사(undefined symbol) 플래그
// Attributes
public:
	bool check; //자료구조 특성상 자료를 밀려서 입력받았을 경우를 표시하는 플래그
	bool checkRES; //opcode에 RESW나 RESB가 있어서 해당라인은 목적코드를 생성하지 않기위해 표시하는 플래그
	char temp[10]; //자료구조 특성상 자료를 밀려서 입력받았을 경우 잘못 입력받은 값을 원래위치로 스왑하기위해 임시로 저장하기위한 공간
	char temp1[10]; //자료구조 특성상 자료를 밀려서 입력받았을 경우 잘못 입력받은 값을 원래위치로 스왑하기위해 임시로 저장하기위한 공간
	char temp2[10]; //자료구조 특성상 자료를 밀려서 입력받았을 경우 잘못 입력받은 값을 원래위치로 스왑하기위해 임시로 저장하기위한 공간
	char temp3[10]; //pass1 symbol 중복검사를 하기위해 임시로 해당 label을 저장하기위한 공간(duplicate symbol)
	char temp4[10]; ////자료구조 특성상 자료를 밀려서 입력받았을 경우 잘못 입력받은 값을 원래위치로 스왑하기위해 임시로 저장하기위한 공간
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void Pass1(CString filename);
	afx_msg void OnAssSource();
	afx_msg void OnAssPass1();
	afx_msg void OnAssSymtab();
	afx_msg void OnPass2Object();
	int OPTAB(char* opcode);
	void searchSymtab(char* symname,int pass);
	void Pass2(CString filename);
	void hexdec(int loc);
	afx_msg void OnClearClick();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__F930E632_47A0_45A1_9A76_82D0D39293B4__INCLUDED_)
