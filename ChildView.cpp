// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Simple.h"
#include "ChildView.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <math.h>
#include <stdlib.h>
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
	//��Ŭ���� ������ �κп��� ������ ������������ �ʱ�ȭ �����־���.
	check=FALSE;
	checkRES=FALSE;
	Locctr=0;
	programLength=0;
	startaddress=0;
	returnsymval=0;
	normal=0;
	sympass1=0;
	sympass2=0;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_ASS_SOURCE, &CChildView::OnAssSource)
	ON_COMMAND(ID_ASS_PASS1, &CChildView::OnAssPass1)
	ON_COMMAND(ID_ASS_SYMTAB, &CChildView::OnAssSymtab)
	ON_COMMAND(ID_PASS2_OBJECT, &CChildView::OnPass2Object)
	ON_COMMAND(ID_CLEAR_CLICK, &CChildView::OnClearClick)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers


BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this);
}

void CChildView::Pass1(CString filename) //CString���� ������ �̸��� �Է¹޴´�.
{
	ifstream file(filename); //�Է¹��� �̸��� ������ ����.
	ofstream checkfirst("check.txt"); //START�� Ȯ���ϱ� ���� �ӽ÷� ���� �ؽ�Ʈ����
	ofstream checksecond("checkpass2.txt"); //pass1���� �ڷᱸ���� �Ͼ�� ������ �ϵ��� pass2���� �����ϱ����� ������� ���� NULL�̶�� ���ڸ� �Է��Ѵ�.
	//�� ������ ���� pass2�Լ����� listfile��ſ� �ҷ��ͼ� ����Ѵ�.
	ofstream checksymtab("checkpass2sym.txt");
	//�� ������ symtab�� �ּҸ� 10������ �����Ͽ� pass2���� 16������ ������ �����ڵ带 �ۼ��ϰԲ� �Ѵ�.
	ofstream inputlist("listfile.txt"); //pass1������ ���� locctr���� ������ ����
	ofstream inputsymtab("symtab.txt"); //pass1������ ���� symtab�� ������ ����
	Line *m_line=new Line; //�� ���� ������ �ڷḦ �ޱ����� ������ ����ü�� ����Ʈ������ �����Ͽ��ش�.
	SymTab *m_sym=new SymTab; //symtab������ ���� ������ ����ü�� ����Ʈ������ �����Ͽ��ش�.
	

	file>>m_line->label>>m_line->opcode>>m_line->operand; 
	checkfirst<<m_line->label<<m_line->opcode<<m_line->operand;
	
	if(strcmp(m_line->opcode,"START")!=0){
		MessageBox("OPCODE START�� �������� �ʽ��ϴ�.");
		Locctr=0;
	}
	else if(strcmp(m_line->opcode,"START")==0){
		startaddress=atoi(m_line->operand);
		hexdec(startaddress);  //16������ 10������ �ٲپ� �����ּҸ� 10������ ���ϰ� ���� ���Ͽ��� 16���������� �Էµǵ��� �Ѵ�.
		itoa(Locctr,ch_locctr,16);
		inputlist<<ch_locctr<<" "<<m_line->label<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
		checksecond<<Locctr<<" "<<m_line->label<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
	}
	// START�� �����ϴ��� �˻��Ͽ� START�� ���ٸ� locctr���� 0���� �����Ͽ��ش�.


	while(!file.eof()){
		char linebuffer[40];
		CString buffer;
		file.getline(linebuffer,40);
		buffer=linebuffer[40];

		if(strchr(buffer,'\n')==0){
			file>>m_line->label>>m_line->opcode>>m_line->operand;

			if(strcmp(m_line->label,".")==0 && strcmp(m_line->opcode,".")==0 && strcmp(m_line->operand,".")==0){			
				inputlist<<".\n"<<m_line->label<<"\n"<<m_line->opcode<<"\n"<<m_line->operand<<"\n";	
				Locctr=Locctr;
			}
			//"."���� �̷���� �ּ����� ó�����ش�.

			else if(OPTAB(m_line->label)==0xFF && strcmp(m_line->label,"END")!=0) //���� �̸��� opcode�� �ƴϰ� end�� �ƴҶ�(label,opcode,operand)
			{
				Locctr+=3;				

				if(strcmp(m_line->label,"FIRST")==0) //���� �̸��� FIRST�̸� �״�� ��
				{
					Locctr=Locctr-3;
				}
				itoa(Locctr,ch_locctr,16);
				inputlist<<ch_locctr<<" "<<m_line->label<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
				checksecond<<Locctr<<" "<<m_line->label<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
				strcpy(m_sym->name,m_line->label);
				m_sym->value=Locctr;
				inputsymtab<<m_sym->name<<" "<<ch_locctr<<"\n";
				checksymtab<<m_sym->name<<" "<<Locctr<<"\n";
				if(OPTAB(m_line->opcode)!=0xFF) //opcode�� optab�� �����Ҷ�
				{
					Locctr+=3;
				}

				searchSymtab(m_sym->name,1); //sympass1���� �÷��׸� �����Ѵ�. ������ �ִٸ� -1�� sympass1�� �Էµǰ� �ƴϸ� 0�̴�.
				if(sympass1==-1) //symtab�� �ش� ���� �̹� �����ϸ� ����
				{
					sympass1=0;
					inputlist<<"LABEL�� �ߺ��˴ϴ�.(duplicate symbol)"<<"\n";
					inputsymtab<<"LABEL�� �ߺ��˴ϴ�.(duplicate symbol)"<<"\n";
				}
				if(strcmp(m_line->opcode,"WORD")==0){
					Locctr+=3;
				}
				else if(strcmp(m_line->opcode,"RESW")==0){
					int len=atoi(m_line->operand);
					len= len*3;
					Locctr+=len;
				}
				else if(strcmp(m_line->opcode,"RESB")==0){
					int len=atoi(m_line->operand);
					Locctr+=len;
				}
				else if(strcmp(m_line->opcode,"BYTE")==0){
					if(strchr(m_line->operand,'C')){
						int len=strlen(m_line->operand);
						len = len-3;
 						Locctr+=len;
					}
					else if(strchr(m_line->operand,'X')){
						int len=strlen(m_line->operand);
						len=(len-3)/2;
						Locctr+=len;
					}
				}
				else if(strcmp(m_line->opcode,"START")==0){
					Locctr=Locctr;
				}
				
				if(OPTAB(m_line->opcode)==0xFF && strcmp(m_line->opcode,"WORD")!=0 && strcmp(m_line->opcode,"RESW")!=0 &&strcmp(m_line->opcode,"RESB")!=0 && strcmp(m_line->opcode,"BYTE")!=0){
					inputlist<<"�ش� OPCODE�� �������� �ʽ��ϴ�.(invalid operation code)"<<"\n";
				} //opcode�� �������� �ʴ� ��� ����� ���� &&�� �Ͽ� invalid operation code ������ �߻������� �˷��ش�.
				Locctr=Locctr-3;
			}
			//label,opcode,operand�� �� ���忡 ��� ������ ���


			if(OPTAB(m_line->label)==0x4C)//RSUB�� �󺧿� �����Ҷ� (���������� label,opcode,operand�� �����ϰ� ���� ���ο� opcode,operand�� ������ ���)
			{
				Locctr+=3;
 				strcpy(temp,m_line->label);
				file>>m_line->label;
				strcpy(temp4,m_line->label);
				strcpy(m_line->label,"");
				strcpy(temp1,m_line->opcode);
				strcpy(temp2,m_line->operand);
				strcpy(m_line->operand,"");
				strcpy(m_line->opcode,temp);   //���������� ������ temp������ ����Ͽ� �ڷ���� ���� ��ġ�� ���ҽ����ش�.
				itoa(Locctr,ch_locctr,16);
				inputlist<<ch_locctr<<" "<<m_line->opcode<<"\n";
				checksecond<<Locctr<<" NULL "<<m_line->opcode<<" NULL"<<"\n";
				check=TRUE;  //������ �з����� ǥ���Ѵ�.
				Locctr+=3;
			}
			if(check==TRUE) //������ �зȰ� opcode�� BYTE�� ��츦 ���� if��
			{
					check=FALSE; //if���ȿ��� �и� ������ ó�����ֱ� ������ �ٽ� �÷��׸� �������·� �ʱ�ȭ �����ش�.
					int m_loc=0;
					strcpy(m_line->label,temp1);
					strcpy(m_line->opcode,temp2);
					strcpy(m_line->operand,temp4);
					
					if(strcmp(m_line->opcode,"BYTE")==0){
						if(strchr(m_line->operand,'C')){
							int len=strlen(m_line->operand);
							len = len-6;
 							m_loc+=len;
						}
						else if(strchr(m_line->operand,'X')){
							int len=strlen(m_line->operand);
							len=(len-3)/2-3;
							m_loc+=len;
						}
					}
					itoa(Locctr,ch_locctr,16);
					inputlist<<ch_locctr<<" "<<m_line->label<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
					checksecond<<Locctr<<" "<<m_line->label<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
					Locctr+=m_loc;
					strcpy(m_sym->name,m_line->label);
					m_sym->value=Locctr;
					inputsymtab<<m_sym->name<<" "<<ch_locctr<<"\n";
					checksymtab<<m_sym->name<<" "<<Locctr<<"\n";
					searchSymtab(m_sym->name,1);
					if(sympass1==-1) //symtab�� �ش� ���� �̹� �����ϸ� ����
					{
						sympass1=0;
						inputlist<<"LABEL�� �ߺ��˴ϴ�.(duplicate symbol)"<<"\n";
						inputsymtab<<"LABEL�� �ߺ��˴ϴ�.(duplicate symbol)"<<"\n";
					}
			}
			if(OPTAB(m_line->operand)==0x4C) //operand�� RSUB�� ������ ���(���������� opcode,operand�� �����ϰ� ������ο� RSUB(opcode)�� �����ϴ� ���)
			{		
				strcpy(temp,m_line->label);
				strcpy(temp1,m_line->opcode);
				strcpy(temp2,m_line->operand);

				if(OPTAB(m_line->label)!=0xFF){
					Locctr+=3;
					strcpy(m_line->label,"");
					strcpy(m_line->opcode,temp);
					strcpy(m_line->operand,temp1);
					itoa(Locctr,ch_locctr,16);
					inputlist<<ch_locctr<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
					checksecond<<Locctr<<" "<<m_line->opcode<<" "<<m_line->operand<<" NULL"<<"\n";
				}
				Locctr+=3;
				strcpy(m_line->opcode,temp2);
				strcpy(m_line->operand,"");
				itoa(Locctr,ch_locctr,16);
				inputlist<<ch_locctr<<" "<<m_line->opcode<<"\n";
				checksecond<<Locctr<<" NULL "<<m_line->opcode<<" NULL"<<"\n";
			}
			else if(OPTAB(m_line->label)!=0xFF && OPTAB(m_line->operand)!=0x4C) //�󺧿� RSUB�� �ƴ� opcode�̸��� �����Ҷ�
			{
				Locctr+=3;		
				strcpy(temp,m_line->opcode); //operand�� temp�� ����
				strcpy(m_line->opcode,m_line->label); //�󺧿� �ִ� opcode�� opcode�� ����
				file>>m_line->label; //������ �ϳ��� ���� ����
				strcpy(temp2,m_line->label); //�� ������ temp2�� ����
				strcpy(m_line->label,""); //���� �������� �ʾƾ������� �ʱ�ȭ
				strcpy(temp1,m_line->operand); //operand�� ��(���������� opcode) temp1������
				strcpy(m_line->operand,temp); //operand�� temp�� ����
				check=TRUE; // ��ĭ�� �з����� ǥ��
				itoa(Locctr,ch_locctr,16);
				inputlist<<ch_locctr<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
				checksecond<<Locctr<<" "<<m_line->opcode<<" "<<m_line->operand<<" NULL"<<"\n";
			}
			if(check==TRUE && strcmp(m_line->label,"END")!=0) //��ĭ�� �и��� �󺧿� end�� ������� ���� ���
			{
				Locctr+=3;
				strcpy(m_line->opcode,temp1); //temp1�� opcode�� ����
				strcpy(m_line->operand,temp2); 
				check=FALSE;
				
				if(OPTAB(m_line->operand)!=0xFF) //���۷��忡 �����ڵ尡 �� ���� ���
				{
					file>>m_line->label;
					strcpy(temp4,m_line->label);
					strcpy(m_line->label,temp1);
					strcpy(m_line->opcode,temp2);
					strcpy(m_line->operand,temp4);
					itoa(Locctr,ch_locctr,16);
					inputlist<<ch_locctr<<" "<<m_line->label<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
					checksecond<<Locctr<<" "<<m_line->label<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
					strcpy(m_sym->name,m_line->label);
					m_sym->value=Locctr;
					inputsymtab<<m_sym->name<<" "<<ch_locctr<<"\n";
					checksymtab<<m_sym->name<<" "<<Locctr<<"\n";
					searchSymtab(m_sym->name,1);
					if(sympass1==-1) //symtab�� �ش� ���� �̹� �����ϸ� ����
					{
						sympass1=0;
						inputlist<<"LABEL�� �ߺ��˴ϴ�.(duplicate symbol)"<<"\n";
						inputsymtab<<"LABEL�� �ߺ��˴ϴ�.(duplicate symbol)"<<"\n";
					}
				}
				else{
					itoa(Locctr,ch_locctr,16);
					inputlist<<ch_locctr<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
					checksecond<<Locctr<<" "<<m_line->opcode<<" "<<m_line->operand<<" NULL"<<"\n";
				}
			}
			else if(strcmp(m_line->label,"END")==0)
			{
				strcpy(temp,m_line->label);
				strcpy(temp1,m_line->opcode);
				strcpy(m_line->opcode,temp);
				strcpy(m_line->operand,temp1);
				inputlist<<m_line->opcode<<" "<<m_line->operand<<"\n";
				checksecond<<m_line->opcode<<" "<<m_line->operand<<" NULL"<<" NULL"<<"\n";
				
			}
			strcpy(temp3,m_sym->name); //duplicate symbol �˻��
			}

	}
	int m_locctr=Locctr;
	hexdec(startaddress);
	programLength=m_locctr-Locctr+3;  //���α׷� �� ���̴� ������ 10���� Locctr���� 16���� �����ּҸ� 10������ ��ȯ�� Locctr�� ������ �� �� END�� 3�� �����ش�.
	file.close();
	inputlist.close();
	inputsymtab.close();
	checksymtab.close();
	checkfirst.close();
	checksecond.close();
}

void CChildView::Pass2(CString filename) //CString���� �����̸��� �Է¹޴´�.
{
	ifstream file(filename); //�ش� �̸��� ������ ����.
	ifstream symtab("checkpass2sym.txt"); //pass1���� ������ symtab������ ����.
	ofstream object("object.txt"); //���� ���α׷��� �����ϱ� ���� object���� 
	ofstream check("check.txt"); //START�� üũ�ϱ� ���� �ӽ�����
	Line *m_line=new Line; //�� ������ �Է¹ޱ����� ����ü�� ������������ ����
	char header[21]= {' ', }; //������ڵ� ����
	char text[72]= {' ', }; //�ؽ�Ʈ���ڵ� ����
	char end[9] = {' ', }; //���巹�ڵ� ����
	CString loc,opcode,operand,pl; //�迭�� ����� ���ڵ忡 �Է¹��� ���� �ֱ����� CString�� ����

	file>>m_line->loc>>m_line->label>>m_line->opcode>>m_line->operand;
	check<<m_line->loc<<m_line->label<<m_line->opcode<<m_line->operand;

	if(strcmp(m_line->opcode,"START")!=0){
		MessageBox("OPCODE START�� �������� �ʽ��ϴ�.");
	}
	else if(strcmp(m_line->opcode,"START")==0){
		header[0]='H';
		strcpy(&header[1],m_line->label);
		int len;
		len=strlen(m_line->label);
		
		for(;len<6;len++)
			header[len+1]=' ';
		loc.Format("%06d", startaddress);
		pl.Format("%06X", programLength);
		strcpy(&header[7],loc);
		strcpy(&header[13],pl);
		object<<header<<"\n";
	}
	//������ڵ� �ۼ�


	while(!file.eof()){
		file>>m_line->loc>>m_line->label>>m_line->opcode>>m_line->operand;
		
		hexdec(startaddress);
		loc.Format("%d",Locctr);
		if(strcmp(m_line->loc,loc)==0) //�ؽ�Ʈ���ڵ��� ���� ���ۺκ� 
		{
			text[0]='T';
			loc.Format("%06d",startaddress);
			strcpy(&text[1],loc);
			object<<&text[0];
		}
		if(strcmp(m_line->loc,"END")==0){
			object<<"\n";
			end[0]='E';
			loc.Format("%06d",startaddress);
			strcpy(&end[1],loc);
			object<<&end[0];
		}
		if(strcmp(m_line->label,"NULL")!=0 && OPTAB(m_line->opcode)!=0xFF && strcmp(m_line->operand,"NULL")!=0) //(loc,label,opcode,operand ��� ����������)�ش� ������ �����ڵ� ������ ���� opcode�� �˻��Ѵ�.
		{
			opcode.Format("%02X",OPTAB(m_line->opcode)); //optab���� opcode�� ���� �޾ƿ´�
			strcpy(&text[10+normal],opcode);
			object<<&text[10+normal];
			searchSymtab(m_line->operand,2); //symtab���� �ش� operand�� ����Ű�� ���� �����ϴ��� Ȯ��
			if(sympass2==0) //symtab���� ���۷����� ���� ���Ͽ� symbol�� ������ �ּҰ��� �ҷ��ͼ� �Է���
			{
				operand.Format("%04X",returnsymval);
				strcpy(&text[12+normal],operand);
				object<<&text[12+normal];
			}
			else if(sympass2==-1) //�ش� operand�� symtab�� �������� ������ undefined symbol������ ���
			{
				sympass2=0;
				CString str;
				str.Format("%s",m_line->operand);
				str+=" undefined symbol���� �Դϴ�.";
				MessageBox(str);
				return;
			}			
			normal+=6; //���� �Է��� ���� 6��Ʈ�� ����
		}
		else if(OPTAB(m_line->label)!=0xFF && strcmp(m_line->operand,"NULL")==0) //label�� opcode�� ����ִ� ��� (opcode,operan�� �ִ°��)
		{
			if(strchr(m_line->opcode,',')) //,X�� �ε��� �������͸� ����� ��츦 ����
			{
				CString indexcheck;
				strcpy(temp,m_line->label);
				strcpy(m_line->label,"");
				strcpy(temp1,m_line->opcode);
				strcpy(m_line->opcode,temp);
				strcpy(m_line->operand,temp1);
					
				indexcheck=strtok(m_line->operand,","); //","���������� ���� �� operand�� �ִ� symbol�� �Է¹޴´�.

				opcode.Format("%02X",OPTAB(m_line->opcode));
				strcpy(&text[10+normal],opcode);
				object<<&text[10+normal];

				strcpy(m_line->operand,indexcheck);
				searchSymtab(m_line->operand,2);
				if(sympass2==0) //symtab���� ���۷����� ���� ���Ͽ� symbol�� ������ �ּҰ��� �ҷ��ͼ� �Է���
				{
					int index=returnsymval;
					index=index+32768;  //X�� �����ڵ�󿡼� 16���� 8000���� ���۷����� ���� 10���� 32768�� �����ش�.
					operand.Format("%04X",index);
					strcpy(&text[12+normal],operand);
					object<<&text[12+normal];
				}
				else if(sympass2==-1){
					sympass2=0;
					CString str;
					str.Format("%s",m_line->operand);
					str+=" undefined symbol���� �Դϴ�.";
					MessageBox(str);
					return;
				}
				
			}
			else //","���� opcode,operand�� ���� ���
			{
				strcpy(temp,m_line->label);
				strcpy(m_line->label,"");
				strcpy(temp1,m_line->opcode);
				strcpy(m_line->opcode,temp);
				strcpy(m_line->operand,temp1);

				opcode.Format("%02X",OPTAB(m_line->opcode));
				strcpy(&text[10+normal],opcode);
				object<<&text[10+normal];	
				searchSymtab(m_line->operand,2);
				if(sympass2==0) //symtab���� ���۷����� ���� ���Ͽ� symbol�� ������ �ּҰ��� �ҷ��ͼ� �Է���
				{
					operand.Format("%04X",returnsymval);
					strcpy(&text[12+normal],operand);
					object<<&text[12+normal];
				}
				else if(sympass2==-1){
					sympass2=0;
					CString str;
					str.Format("%s",m_line->operand);
					str+=" undefined symbol���� �Դϴ�.";
					MessageBox(str);
					return;
				}
				
			}
			normal+=6;
		}
		else if(strcmp(m_line->label,"NULL")==0 && OPTAB(m_line->opcode)==0x4C && strcmp(m_line->operand,"NULL")==0) //RSUB�� opcode�� ������ ���
		{
			opcode.Format("%02X",OPTAB(m_line->opcode));
			int zero=0;
			operand.Format("%04d",zero); //RSUB�� ��� �����ڵ忡 operand���� ��� 0�̴�.
			strcpy(&text[10+normal],opcode);
			object<<&text[10+normal];
			strcpy(&text[12+normal],operand);
			object<<&text[12+normal];

			normal+=6;
		}
		else if(strcmp(m_line->opcode,"WORD")==0) //opcode�� WORD�� ���� ���
		{
			int zero=0;
			opcode.Format("%02X",zero); //WORD�� �����ڵ忡 operand���� �Է¹��� ���ڸ� �״�� �Է��Ѵ�.
			strcpy(&text[10+normal],opcode);
			int m_operand=atoi(m_line->operand);
			operand.Format("%04X",m_operand);
			object<<&text[10+normal];
			strcpy(&text[12+normal],operand);
			object<<&text[12+normal];
			normal+=6;
		}
		else if(strcmp(m_line->opcode,"BYTE")==0) //opcode�� BYTE�� ���� ���
		{
			if(strchr(m_line->operand,'C')) // C�� ���� ��� �Է¹��� ���ڸ� 16���� ascii�ڵ�� ��ȯ�Ͽ� �����ڵ忡 �Է��Ѵ�.
			{
				int len = strlen(m_line->operand);
				len= (len-3)*2;
				int num[10]={' ', };
				CString hex;
				strcpy(temp,"");
				strcpy(temp,m_line->operand);
				for(int i=0;i<10;i++){
					temp[i]=temp[i+2];
					if(temp[i]=='C' || temp[i]=='\''){
						temp[i]='\0';
					}
				}
				for(int i=0;i<len;i++){
					opcode.Format("%d",temp[i]);
					num[i]=atoi(opcode);
					if(num[i]!=NULL){
						char temp[3];
						itoa(num[i],temp,16);
						hex=hex+temp;
					}
					else
						break;
				}
				strcpy(&text[10+normal],hex);
				object<<&text[10+normal];
				normal+=len;
			}
			else if(strchr(m_line->operand,'X')) //X�� ���� ��� �Է¹��� ���ڸ� ���� 2���� 1����Ʈ�� �Ѵ�. �����ڵ忡�� ���ڰ� �״�� �Էµȴ�.
			{
				int len = strlen(m_line->operand);
				len= (len-3)/2;
				strcpy(temp,"");
				strcpy(temp,m_line->operand);
				for(int i=0;i<10;i++){
					temp[i]=temp[i+2];
					if(temp[i]=='X'|| temp[i]=='\''){
						temp[i]='\0';
					}
				}
				operand.Format("%s",temp);
				strcpy(&text[10+normal],operand);
				object<<&text[10+normal];
				normal+=len;
			}
		}

		if(normal==60 || (normal<=60 && normal>54)) //�ؽ�Ʈ���ڵ��� ũ�Ⱑ �� ���ų� ���ο� �����ڵ带 �Է¹������� ������ ���� ��� ���ο� �ؽ�Ʈ���ڵ带 ����
		{
			pl.Format("%02X",normal/2);
			strcpy(&text[7],pl);
			object<<&text[7];

			object<<"\n";
			for(int i=0; i<72; i++){
				text[i]='\0';
			}
			text[0]='T';
			int len = atoi(m_line->loc)+3;
			loc.Format("%06X",len);
			strcpy(&text[1],loc);
			object<<&text[0];
			normal=0;
		}
			
		else if(strcmp(m_line->opcode,"RESW")==0 || strcmp(m_line->opcode,"RESB")==0) //opcode�� RESW�� RESB�� ���� ��� �����ڵ�� �������� �ʴ´�./
		{
			//�������α׷��� ���� �ؽ�Ʈ���ڵ带 �����ϰԵȴ�.
			while(!file.eof())
			{
				file>>m_line->loc>>m_line->label>>m_line->opcode>>m_line->operand;
				if(strcmp(m_line->opcode,"RESW")!=0 && strcmp(m_line->opcode,"RESB")!=0)
				{
					break;
				}
			}
			pl.Format("%02X",normal/2);
			strcpy(&text[7],pl);
			object<<&text[7];
			//RESW�� RESB�� ������ ���� �ٽ� ���ο� �ؽ�Ʈ ���ڵ尡 ���۵ǵ��� �Ѵ�.
			for(int i=0;i<71;i++){
				text[i]='\0';
			}
				
			object<<"\n";
			text[0]='T';
			int len = atoi(m_line->loc);
			loc.Format("%06X",len);
			strcpy(&text[1],loc);
			object<<&text[0];
			normal=0; //�ؽ�Ʈ ���ڵ尡 ���� ���۵Ǳ⶧���� �������̸� �ʱ�ȭ �����ش�.

			if(strcmp(m_line->label,"NULL")!=0 && OPTAB(m_line->opcode)!=0xFF && strcmp(m_line->operand,"NULL")!=0) //(loc,label,opcode,operand ��� ����������)�ش� ������ �����ڵ� ������ ���� opcode�� �˻��Ѵ�.
			{
				opcode.Format("%02X",OPTAB(m_line->opcode)); //optab���� opcode�� ���� �޾ƿ´�
				strcpy(&text[10+normal],opcode);
				object<<&text[10+normal];
				searchSymtab(m_line->operand,2); //symtab���� �ش� operand�� ����Ű�� ���� �����ϴ��� Ȯ��
				if(sympass2==0) //symtab���� ���۷����� ���� ���Ͽ� symbol�� ������ �ּҰ��� �ҷ��ͼ� �Է���
				{
					int len=returnsymval;
					operand.Format("%04X",len);
					strcpy(&text[12+normal],operand);
					object<<&text[12+normal];
				}
				else if(sympass2==-1) //�ش� operand�� symtab�� �������� ������ undefined symbol������ ���
				{
					sympass2=0;
					CString str;
					str.Format("%s",m_line->operand);
					str+=" undefined symbol���� �Դϴ�.";
					MessageBox(str);
					return;
				}			
				normal+=6; //���� �Է��� ���� 6��Ʈ�� ����
			}
			else if(OPTAB(m_line->label)!=0xFF && strcmp(m_line->operand,"NULL")==0) //label�� opcode�� ����ִ� ��� (opcode,operan�� �ִ°��)
			{
				if(strchr(m_line->opcode,',')) //,X�� �ε��� �������͸� ����� ��츦 ����
				{
					CString indexcheck;
					strcpy(temp,m_line->label);
					strcpy(m_line->label,"");
					strcpy(temp1,m_line->opcode);
					strcpy(m_line->opcode,temp);
					strcpy(m_line->operand,temp1);
					
					indexcheck=strtok(m_line->operand,","); //","���������� ���� �� operand�� �ִ� symbol�� �Է¹޴´�.

					opcode.Format("%02X",OPTAB(m_line->opcode));
					strcpy(&text[10+normal],opcode);
					object<<&text[10+normal];

					strcpy(m_line->operand,indexcheck);
					searchSymtab(m_line->operand,2);
					if(sympass2==0) //symtab���� ���۷����� ���� ���Ͽ� symbol�� ������ �ּҰ��� �ҷ��ͼ� �Է���
					{
						int index=returnsymval;
						index=index+32768;  //X�� �����ڵ�󿡼� 16���� 8000���� ���۷����� ���� 10���� 32768�� �����ش�.
						operand.Format("%04X",index);
						strcpy(&text[12+normal],operand);
						object<<&text[12+normal];
					}
					else if(sympass2==-1){
						sympass2=0;
						CString str;
						str.Format("%s",m_line->operand);
						str+=" undefined symbol���� �Դϴ�.";
						MessageBox(str);
						return;
					}
				
				}
				else //","���� opcode,operand�� ���� ���
				{
					strcpy(temp,m_line->label);
					strcpy(m_line->label,"");
					strcpy(temp1,m_line->opcode);
					strcpy(m_line->opcode,temp);
					strcpy(m_line->operand,temp1);

					opcode.Format("%02x",OPTAB(m_line->opcode));
					strcpy(&text[10+normal],opcode);
					object<<&text[10+normal];	
					searchSymtab(m_line->operand,2);
					if(sympass2==0) //symtab���� ���۷����� ���� ���Ͽ� symbol�� ������ �ּҰ��� �ҷ��ͼ� �Է���
					{
						operand.Format("%04x",returnsymval);
						strcpy(&text[12+normal],operand);
						object<<&text[12+normal];
					}
					else if(sympass2==-1){
						sympass2=0;
						CString str;
						str.Format("%s",m_line->operand);
						str+=" undefined symbol���� �Դϴ�.";
						MessageBox(str);
						return;
					}
				
				}
				normal+=6;
			}
			else if(strcmp(m_line->label,"NULL")==0 && OPTAB(m_line->opcode)==0x4C && strcmp(m_line->operand,"NULL")==0) //RSUB�� opcode�� ������ ���
			{
				opcode.Format("%02x",OPTAB(m_line->opcode));
				int zero=0;
				operand.Format("%04d",zero); //RSUB�� ��� �����ڵ忡 operand���� ��� 0�̴�.
				strcpy(&text[10+normal],opcode);
				object<<&text[10+normal];
				strcpy(&text[12+normal],operand);
				object<<&text[12+normal];

				normal+=6;
			}
			else if(strcmp(m_line->opcode,"WORD")==0) //opcode�� WORD�� ���� ���
			{
				int zero=0;
				opcode.Format("%02x",zero); //WORD�� �����ڵ忡 operand���� �Է¹��� ���ڸ� �״�� �Է��Ѵ�.
				strcpy(&text[10+normal],opcode);
				int m_operand=atoi(m_line->operand);
				operand.Format("%04d",m_operand);
				object<<&text[10+normal];
				strcpy(&text[12+normal],operand);
				object<<&text[12+normal];
				normal+=6;
			}
			else if(strcmp(m_line->opcode,"BYTE")==0) //opcode�� BYTE�� ���� ���
			{
				if(strchr(m_line->operand,'C')) // C�� ���� ��� �Է¹��� ���ڸ� 16���� ascii�ڵ�� ��ȯ�Ͽ� �����ڵ忡 �Է��Ѵ�.
				{
					int len = strlen(m_line->operand);
					len= (len-3)*2;
					int num[10]={' ', };
					CString hex;
					strcpy(temp,"");
					strcpy(temp,m_line->operand);
					for(int i=0;i<10;i++){
						temp[i]=temp[i+2];
						if(temp[i]=='C' || temp[i]=='\''){
							temp[i]='\0';
						}
					}
					for(int i=0;i<len;i++){
						opcode.Format("%d",temp[i]);
						num[i]=atoi(opcode);
						if(num[i]!=NULL){
							char temp[3];
							itoa(num[i],temp,16);
							hex=hex+temp;
						}
						else
							break;
					}
					strcpy(&text[10+normal],hex);
					object<<&text[10+normal];
					normal+=len;
				}
				else if(strchr(m_line->operand,'X')) //X�� ���� ��� �Է¹��� ���ڸ� ���� 2���� 1����Ʈ�� �Ѵ�. �����ڵ忡�� ���ڰ� �״�� �Էµȴ�.
				{
					int len = strlen(m_line->operand);
					len= (len-3)/2;
					strcpy(temp,"");
					strcpy(temp,m_line->operand);
					for(int i=0;i<10;i++){
						temp[i]=temp[i+2];
						if(temp[i]=='X'|| temp[i]=='\''){
							temp[i]='\0';
						}
					}
					operand.Format("%s",temp);
					strcpy(&text[10+normal],operand);
					object<<&text[10+normal];
					normal+=len;
				}
			}
				
		}
			
	}
	file.close();
	symtab.close();
	check.close();
	object.close();
}

void CChildView::OnAssSource() //Source Code �޴��� Ŭ���ϸ� �ش� �ؽ�Ʈ ������ ������ ����ϴ� �Լ�
{
	CClientDC dc(this);
	ifstream file("src.txt");
	char buffer[80];
	CString str,strline;
	CRect rectangle;
	GetClientRect(&rectangle);

	

	while(!file.eof()){
		file.getline(buffer,80);
		str=buffer;
		m_list.AddTail(str);
	}

	POSITION pos=m_list.GetHeadPosition();
	while(pos){
		CString strone=m_list.GetNext(pos);
		strline+=strone+'\n';
	}
	
	dc.DrawText(strline,rectangle,DT_LEFT);

	m_list.RemoveAll();
	file.close();
	
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}

void CChildView::OnAssPass1() //Pass1 �޴��� Ŭ���ϸ� �ش� �ؽ�Ʈ ������ ������ ����ϴ� �Լ�
{
	Pass1("src.txt");
	MessageBox("SYMTAB�� Listfile�� �����Ǿ����ϴ�.");
	CClientDC dc(this);
	ifstream file("listfile.txt");
	char buffer[80];
	CString str,strline;
	CRect rectangle;
	GetClientRect(&rectangle);

	while(!file.eof()){
		file.getline(buffer,80);
		str=buffer;
		m_list.AddTail(str);
	}

	POSITION pos=m_list.GetHeadPosition();
	while(pos){
		CString strone=m_list.GetNext(pos);
		strline+=strone+'\n';
	}
	
	dc.DrawText(strline,rectangle,DT_LEFT);

	m_list.RemoveAll();
	file.close();
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}

void CChildView::OnAssSymtab() //SYMTAB �޴��� Ŭ���ϸ� �ش� �ؽ�Ʈ ������ ������ ����ϴ� �Լ�
{
	CClientDC dc(this);
	ifstream file("symtab.txt");
	char buffer[80];
	CString str,strline;
	CRect rectangle;
	GetClientRect(&rectangle);

	

	while(!file.eof()){
		file.getline(buffer,80);
		str=buffer;
		m_list.AddTail(str);
	}

	POSITION pos=m_list.GetHeadPosition();
	while(pos){
		CString strone=m_list.GetNext(pos);
		strline+=strone+'\n';
	}
	
	dc.DrawText(strline,rectangle,DT_LEFT);

	m_list.RemoveAll();
	file.close();
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}

void CChildView::OnPass2Object() //�������α׷� �޴��� Ŭ���ϸ� �ش� �ؽ�Ʈ ������ ������ ����ϴ� �Լ�
{
	CClientDC dc(this);
	Pass2("checkpass2.txt");
	ifstream file("object.txt");
	char buffer[80];
	CString str,strline;
	CRect rectangle;
	GetClientRect(&rectangle);

	

	while(!file.eof()){
		file.getline(buffer,80);
		str=buffer;
		m_list.AddTail(str);
	}

	POSITION pos=m_list.GetHeadPosition();
	while(pos){
		CString strone=m_list.GetNext(pos);
		strline+=strone+'\n';
	}
	
	dc.DrawText(strline,rectangle,DT_LEFT);

	m_list.RemoveAll();
	file.close();
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}

int CChildView::OPTAB(char* opcode)  //OPTAB�Լ��� char*������ opcode�� �Է¹޾Ƽ� int���� 16������ �ش� opcode�� ���� �˷��ش�.
{
	int int_opcode;
	if  (!strcmp(opcode,"ADD")) int_opcode=0x18;
	else if(!strcmp(opcode,"AND")) int_opcode=0x40;
	else if(!strcmp(opcode,"COMP")) int_opcode=0x28;
	else if(!strcmp(opcode,"DIV")) int_opcode=0x24;
	else if(!strcmp(opcode,"J")) int_opcode=0x3C;
	else if(!strcmp(opcode,"JEQ")) int_opcode=0x30;
	else if(!strcmp(opcode,"JGT")) int_opcode=0x34;
	else if(!strcmp(opcode,"JLT"))	int_opcode=0x38;
	else if(!strcmp(opcode,"JSUB")) int_opcode=0x48;
	else if(!strcmp(opcode,"LDA")) int_opcode=0x00;
	else if(!strcmp(opcode,"LDL")) int_opcode=0x08;
	else if(!strcmp(opcode,"MUL")) int_opcode=0x20;
	else if(!strcmp(opcode,"OR")) int_opcode=0x44;
	else if(!strcmp(opcode,"RSUB")) int_opcode=0x4C;
	else if(!strcmp(opcode,"STA")) int_opcode=0x0C;
	else if(!strcmp(opcode,"STL")) int_opcode=0x14;
	else if(!strcmp(opcode,"STX")) int_opcode=0x10;
	else if(!strcmp(opcode,"TD")) int_opcode=0xE0;
	else if(!strcmp(opcode,"TIX")) int_opcode=0x2C;
	else if(!strcmp(opcode,"LDX")) int_opcode=0x04;
	else if(!strcmp(opcode,"STCH")) int_opcode=0x54;
	else if(!strcmp(opcode,"WD")) int_opcode=0xDC;
	else if(!strcmp(opcode,"RD")) int_opcode=0xD8;
	else if(!strcmp(opcode,"LDCH")) int_opcode=0x50;
	else int_opcode=0xFF;
	return int_opcode;
}

void CChildView::searchSymtab(char* symname, int pass) //symtab�� ���� �������������� pass1�� pass2���� ����Ѵ�. ������ ����� int pass������ �����Ѵ�.
{
	ifstream symtab("symtab.txt");
	ifstream pass2symtab("checkpass2sym.txt");
	SymTab *m_sym=new SymTab;

	if(pass==1){
		if(strcmp(symname,temp3)==0){
			sympass1=-1; //�ߺ��ɶ� -1�̴�.
		}
		else{
			sympass1=0; //�ߺ����� ������ 0;
		} //Pass1���� duplicate symbolȮ���ϴ� if��
	}
	else if(pass==2){
		while(!pass2symtab.eof()){
			pass2symtab>>m_sym->name>>m_sym->m_value;
			
			if(strcmp(symname,m_sym->name)==0){
				returnsymval=atoi(m_sym->m_value);
				sympass2=0; //operand�� symtab�� �����Ҷ�
				break;
			}
			else if(strcmp(symname,m_sym->name)!=0){
				sympass2=-1;
			}
		}
	}
	symtab.close();
	pass2symtab.close();
}

void CChildView::hexdec(int loc) //4�ڸ��� 16������ 10������ ��ȯ�Ҷ� ����ϴ� �Լ��� int�� 16������ �Է¹޾Ƽ� ���������� Locctr�� ��ȯ�� 10������ ��������.
{
	int in_loc0,in_loc1,in_loc2,in_loc3;
	int sum=0;
	char ch_loc[5];
	char ch_lloc[2];
	CString str;
	itoa(loc,ch_loc,10);
	ch_lloc[0]=ch_loc[3];
	in_loc0=atoi(ch_lloc);
	ch_lloc[0]=ch_loc[2];
	in_loc1=atoi(ch_lloc);
	ch_lloc[0]=ch_loc[1];
	in_loc2=atoi(ch_lloc);
	ch_lloc[0]=ch_loc[0];
	in_loc3=atoi(ch_lloc);

	sum=in_loc0*pow((double)16,0)+in_loc1*pow((double)16,1)+in_loc2*pow((double)16,2)+in_loc3*pow((double)16,3);
	Locctr=sum;
}


void CChildView::OnClearClick()
{
	Invalidate();
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}
