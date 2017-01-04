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
	char loc[10];  //�ּ�
	char label[10]; //��
	char opcode[10]; //opcode
	char operand[10]; //operand
}Line;

typedef struct SymTab {
	char name[10];	//�� �̸�
	int value; //10���� �ּҰ�
	char m_value[10]; //16���� �ּҰ�
}Symtab;
class CChildView : public CWnd
{
// Construction
public:
	CChildView();
	CStringList m_list;
	int Locctr; //10������ �ּҰ��� �߰��ϱ����� ������Ű�� �ּҰ�
	char ch_locctr[5]; //16������ ��ȯ���� �Է¹��� ����
	int programLength; //10������ ���α׷� �� ���̸� �Է¹��� ����
	char ch_programLength[5]; //16������ ���α׷� �� ���̸� �Է¹��� ����
	int startaddress; //�����ּ�
	int returnsymval; //symtab�� operand�� �˻��Ͽ� �Է¹��� ���� �ּҰ�
	int normal; //�������α׷����� �ؽ�Ʈ ���̺� �� ������ ���̸� ������ ��
	int sympass1; //pass1���� symtab�� ���Ͽ� �����˻�(duplicate symbol) �÷���
	int sympass2; //pass2���� symtab�� ���Ͽ� �����˻�(undefined symbol) �÷���
// Attributes
public:
	bool check; //�ڷᱸ�� Ư���� �ڷḦ �з��� �Է¹޾��� ��츦 ǥ���ϴ� �÷���
	bool checkRES; //opcode�� RESW�� RESB�� �־ �ش������ �����ڵ带 �������� �ʱ����� ǥ���ϴ� �÷���
	char temp[10]; //�ڷᱸ�� Ư���� �ڷḦ �з��� �Է¹޾��� ��� �߸� �Է¹��� ���� ������ġ�� �����ϱ����� �ӽ÷� �����ϱ����� ����
	char temp1[10]; //�ڷᱸ�� Ư���� �ڷḦ �з��� �Է¹޾��� ��� �߸� �Է¹��� ���� ������ġ�� �����ϱ����� �ӽ÷� �����ϱ����� ����
	char temp2[10]; //�ڷᱸ�� Ư���� �ڷḦ �з��� �Է¹޾��� ��� �߸� �Է¹��� ���� ������ġ�� �����ϱ����� �ӽ÷� �����ϱ����� ����
	char temp3[10]; //pass1 symbol �ߺ��˻縦 �ϱ����� �ӽ÷� �ش� label�� �����ϱ����� ����(duplicate symbol)
	char temp4[10]; ////�ڷᱸ�� Ư���� �ڷḦ �з��� �Է¹޾��� ��� �߸� �Է¹��� ���� ������ġ�� �����ϱ����� �ӽ÷� �����ϱ����� ����
	
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
