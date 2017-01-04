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
	//뷰클래스 생성자 부분에서 각각의 전역변수들을 초기화 시켜주었다.
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

void CChildView::Pass1(CString filename) //CString으로 파일의 이름을 입력받는다.
{
	ifstream file(filename); //입력받은 이름의 파일을 연다.
	ofstream checkfirst("check.txt"); //START를 확인하기 위해 임시로 만든 텍스트파일
	ofstream checksecond("checkpass2.txt"); //pass1에서 자료구조상 일어나는 불편한 일들을 pass2에서 방지하기위해 빈공간에 직접 NULL이라는 문자를 입력한다.
	//위 파일은 실제 pass2함수에서 listfile대신에 불러와서 사용한다.
	ofstream checksymtab("checkpass2sym.txt");
	//위 파일은 symtab의 주소를 10진수로 저장하여 pass2에서 16진수로 변경해 목적코드를 작성하게끔 한다.
	ofstream inputlist("listfile.txt"); //pass1과정을 거쳐 locctr값을 배정한 파일
	ofstream inputsymtab("symtab.txt"); //pass1과정을 거쳐 symtab을 생성할 파일
	Line *m_line=new Line; //한 라인 단위로 자료를 받기위해 생성한 구조체를 포인트형으로 선언하여준다.
	SymTab *m_sym=new SymTab; //symtab저장을 위해 생성한 구조체를 포인트형으로 선언하여준다.
	

	file>>m_line->label>>m_line->opcode>>m_line->operand; 
	checkfirst<<m_line->label<<m_line->opcode<<m_line->operand;
	
	if(strcmp(m_line->opcode,"START")!=0){
		MessageBox("OPCODE START가 존재하지 않습니다.");
		Locctr=0;
	}
	else if(strcmp(m_line->opcode,"START")==0){
		startaddress=atoi(m_line->operand);
		hexdec(startaddress);  //16진수를 10진수로 바꾸어 시작주소를 10진수로 더하고 실제 파일에는 16진수형으로 입력되도록 한다.
		itoa(Locctr,ch_locctr,16);
		inputlist<<ch_locctr<<" "<<m_line->label<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
		checksecond<<Locctr<<" "<<m_line->label<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
	}
	// START가 존재하는지 검사하여 START가 없다면 locctr값을 0으로 설정하여준다.


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
			//"."으로 이루어진 주석문을 처리해준다.

			else if(OPTAB(m_line->label)==0xFF && strcmp(m_line->label,"END")!=0) //라벨의 이름이 opcode가 아니고 end가 아닐때(label,opcode,operand)
			{
				Locctr+=3;				

				if(strcmp(m_line->label,"FIRST")==0) //라벨의 이름이 FIRST이면 그대로 둠
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
				if(OPTAB(m_line->opcode)!=0xFF) //opcode가 optab에 존재할때
				{
					Locctr+=3;
				}

				searchSymtab(m_sym->name,1); //sympass1변수 플래그를 생성한다. 오류가 있다면 -1이 sympass1에 입력되고 아니면 0이다.
				if(sympass1==-1) //symtab에 해당 라벨이 이미 존재하면 에러
				{
					sympass1=0;
					inputlist<<"LABEL이 중복됩니다.(duplicate symbol)"<<"\n";
					inputsymtab<<"LABEL이 중복됩니다.(duplicate symbol)"<<"\n";
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
					inputlist<<"해당 OPCODE가 존재하지 않습니다.(invalid operation code)"<<"\n";
				} //opcode가 존재하지 않는 모든 경우의 수를 &&로 하여 invalid operation code 오류가 발생했음을 알려준다.
				Locctr=Locctr-3;
			}
			//label,opcode,operand가 한 문장에 모두 존재할 경우


			if(OPTAB(m_line->label)==0x4C)//RSUB가 라벨에 존재할때 (이전라인이 label,opcode,operand가 존재하고 현재 라인에 opcode,operand만 존재할 경우)
			{
				Locctr+=3;
 				strcpy(temp,m_line->label);
				file>>m_line->label;
				strcpy(temp4,m_line->label);
				strcpy(m_line->label,"");
				strcpy(temp1,m_line->opcode);
				strcpy(temp2,m_line->operand);
				strcpy(m_line->operand,"");
				strcpy(m_line->opcode,temp);   //전역변수로 선언한 temp공간을 사용하여 자료들을 본래 위치로 스왑시켜준다.
				itoa(Locctr,ch_locctr,16);
				inputlist<<ch_locctr<<" "<<m_line->opcode<<"\n";
				checksecond<<Locctr<<" NULL "<<m_line->opcode<<" NULL"<<"\n";
				check=TRUE;  //라인이 밀렸음을 표시한다.
				Locctr+=3;
			}
			if(check==TRUE) //라인이 밀렸고 opcode가 BYTE일 경우를 위한 if문
			{
					check=FALSE; //if문안에서 밀린 문제를 처리해주기 때문에 다시 플래그를 원래상태로 초기화 시켜준다.
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
					if(sympass1==-1) //symtab에 해당 라벨이 이미 존재하면 에러
					{
						sympass1=0;
						inputlist<<"LABEL이 중복됩니다.(duplicate symbol)"<<"\n";
						inputsymtab<<"LABEL이 중복됩니다.(duplicate symbol)"<<"\n";
					}
			}
			if(OPTAB(m_line->operand)==0x4C) //operand에 RSUB가 존재할 경우(이전라인이 opcode,operand만 존재하고 현재라인에 RSUB(opcode)만 존재하는 경우)
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
			else if(OPTAB(m_line->label)!=0xFF && OPTAB(m_line->operand)!=0x4C) //라벨에 RSUB가 아닌 opcode이름이 존재할때
			{
				Locctr+=3;		
				strcpy(temp,m_line->opcode); //operand를 temp에 저장
				strcpy(m_line->opcode,m_line->label); //라벨에 있는 opcode를 opcode에 저장
				file>>m_line->label; //못받은 하나의 변수 저장
				strcpy(temp2,m_line->label); //그 변수를 temp2에 저장
				strcpy(m_line->label,""); //라벨은 존재하지 않아야함으로 초기화
				strcpy(temp1,m_line->operand); //operand의 값(다음라인의 opcode) temp1에저장
				strcpy(m_line->operand,temp); //operand에 temp값 저장
				check=TRUE; // 한칸씩 밀렸음을 표시
				itoa(Locctr,ch_locctr,16);
				inputlist<<ch_locctr<<" "<<m_line->opcode<<" "<<m_line->operand<<"\n";
				checksecond<<Locctr<<" "<<m_line->opcode<<" "<<m_line->operand<<" NULL"<<"\n";
			}
			if(check==TRUE && strcmp(m_line->label,"END")!=0) //한칸씩 밀리고 라벨에 end가 들어있지 않을 경우
			{
				Locctr+=3;
				strcpy(m_line->opcode,temp1); //temp1을 opcode에 저장
				strcpy(m_line->operand,temp2); 
				check=FALSE;
				
				if(OPTAB(m_line->operand)!=0xFF) //오퍼런드에 오피코드가 들어가 있을 경우
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
					if(sympass1==-1) //symtab에 해당 라벨이 이미 존재하면 에러
					{
						sympass1=0;
						inputlist<<"LABEL이 중복됩니다.(duplicate symbol)"<<"\n";
						inputsymtab<<"LABEL이 중복됩니다.(duplicate symbol)"<<"\n";
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
			strcpy(temp3,m_sym->name); //duplicate symbol 검사용
			}

	}
	int m_locctr=Locctr;
	hexdec(startaddress);
	programLength=m_locctr-Locctr+3;  //프로그램 총 길이는 누적된 10진수 Locctr값과 16진수 시작주소를 10진수로 변환한 Locctr을 뺄셈을 한 후 END의 3을 더해준다.
	file.close();
	inputlist.close();
	inputsymtab.close();
	checksymtab.close();
	checkfirst.close();
	checksecond.close();
}

void CChildView::Pass2(CString filename) //CString형의 파일이름을 입력받는다.
{
	ifstream file(filename); //해당 이름의 파일을 연다.
	ifstream symtab("checkpass2sym.txt"); //pass1에서 생성한 symtab파일은 연다.
	ofstream object("object.txt"); //목적 프로그램을 저장하기 위한 object파일 
	ofstream check("check.txt"); //START를 체크하기 위한 임시파일
	Line *m_line=new Line; //한 라인을 입력받기위한 구조체를 포인터형으로 선언
	char header[21]= {' ', }; //헤더레코드 선언
	char text[72]= {' ', }; //텍스트레코드 선언
	char end[9] = {' ', }; //엔드레코드 선언
	CString loc,opcode,operand,pl; //배열로 선언된 레코드에 입력받은 값을 넣기위해 CString을 선언

	file>>m_line->loc>>m_line->label>>m_line->opcode>>m_line->operand;
	check<<m_line->loc<<m_line->label<<m_line->opcode<<m_line->operand;

	if(strcmp(m_line->opcode,"START")!=0){
		MessageBox("OPCODE START가 존재하지 않습니다.");
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
	//헤더레코드 작성


	while(!file.eof()){
		file>>m_line->loc>>m_line->label>>m_line->opcode>>m_line->operand;
		
		hexdec(startaddress);
		loc.Format("%d",Locctr);
		if(strcmp(m_line->loc,loc)==0) //텍스트레코드의 최초 시작부분 
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
		if(strcmp(m_line->label,"NULL")!=0 && OPTAB(m_line->opcode)!=0xFF && strcmp(m_line->operand,"NULL")!=0) //(loc,label,opcode,operand 모두 값이있을때)해당 라인의 목적코드 생성을 위해 opcode를 검사한다.
		{
			opcode.Format("%02X",OPTAB(m_line->opcode)); //optab에서 opcode의 값을 받아온다
			strcpy(&text[10+normal],opcode);
			object<<&text[10+normal];
			searchSymtab(m_line->operand,2); //symtab에서 해당 operand가 가리키는 라벨이 존재하는지 확인
			if(sympass2==0) //symtab에서 오퍼런드의 값과 비교하여 symbol이 있으면 주소값을 불러와서 입력함
			{
				operand.Format("%04X",returnsymval);
				strcpy(&text[12+normal],operand);
				object<<&text[12+normal];
			}
			else if(sympass2==-1) //해당 operand가 symtab에 존재하지 않으면 undefined symbol에러를 출력
			{
				sympass2=0;
				CString str;
				str.Format("%s",m_line->operand);
				str+=" undefined symbol에러 입니다.";
				MessageBox(str);
				return;
			}			
			normal+=6; //다음 입력을 위해 6비트를 증가
		}
		else if(OPTAB(m_line->label)!=0xFF && strcmp(m_line->operand,"NULL")==0) //label에 opcode가 들어있는 경우 (opcode,operan만 있는경우)
		{
			if(strchr(m_line->opcode,',')) //,X로 인덱스 레지스터를 사용할 경우를 위해
			{
				CString indexcheck;
				strcpy(temp,m_line->label);
				strcpy(m_line->label,"");
				strcpy(temp1,m_line->opcode);
				strcpy(m_line->opcode,temp);
				strcpy(m_line->operand,temp1);
					
				indexcheck=strtok(m_line->operand,","); //","이전까지의 문자 즉 operand에 있는 symbol을 입력받는다.

				opcode.Format("%02X",OPTAB(m_line->opcode));
				strcpy(&text[10+normal],opcode);
				object<<&text[10+normal];

				strcpy(m_line->operand,indexcheck);
				searchSymtab(m_line->operand,2);
				if(sympass2==0) //symtab에서 오퍼런드의 값과 비교하여 symbol이 있으면 주소값을 불러와서 입력함
				{
					int index=returnsymval;
					index=index+32768;  //X는 목적코드상에서 16진수 8000으로 오퍼런드의 값에 10진수 32768을 더해준다.
					operand.Format("%04X",index);
					strcpy(&text[12+normal],operand);
					object<<&text[12+normal];
				}
				else if(sympass2==-1){
					sympass2=0;
					CString str;
					str.Format("%s",m_line->operand);
					str+=" undefined symbol에러 입니다.";
					MessageBox(str);
					return;
				}
				
			}
			else //","없이 opcode,operand만 있을 경우
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
				if(sympass2==0) //symtab에서 오퍼런드의 값과 비교하여 symbol이 있으면 주소값을 불러와서 입력함
				{
					operand.Format("%04X",returnsymval);
					strcpy(&text[12+normal],operand);
					object<<&text[12+normal];
				}
				else if(sympass2==-1){
					sympass2=0;
					CString str;
					str.Format("%s",m_line->operand);
					str+=" undefined symbol에러 입니다.";
					MessageBox(str);
					return;
				}
				
			}
			normal+=6;
		}
		else if(strcmp(m_line->label,"NULL")==0 && OPTAB(m_line->opcode)==0x4C && strcmp(m_line->operand,"NULL")==0) //RSUB가 opcode에 존재할 경우
		{
			opcode.Format("%02X",OPTAB(m_line->opcode));
			int zero=0;
			operand.Format("%04d",zero); //RSUB의 경우 목적코드에 operand값이 모두 0이다.
			strcpy(&text[10+normal],opcode);
			object<<&text[10+normal];
			strcpy(&text[12+normal],operand);
			object<<&text[12+normal];

			normal+=6;
		}
		else if(strcmp(m_line->opcode,"WORD")==0) //opcode에 WORD가 있을 경우
		{
			int zero=0;
			opcode.Format("%02X",zero); //WORD는 목적코드에 operand에서 입력받은 숫자를 그대로 입력한다.
			strcpy(&text[10+normal],opcode);
			int m_operand=atoi(m_line->operand);
			operand.Format("%04X",m_operand);
			object<<&text[10+normal];
			strcpy(&text[12+normal],operand);
			object<<&text[12+normal];
			normal+=6;
		}
		else if(strcmp(m_line->opcode,"BYTE")==0) //opcode에 BYTE가 있을 경우
		{
			if(strchr(m_line->operand,'C')) // C가 있을 경우 입력받은 문자를 16진수 ascii코드로 변환하여 목적코드에 입력한다.
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
			else if(strchr(m_line->operand,'X')) //X가 있을 경우 입력받은 문자를 문자 2개에 1바이트로 한다. 목적코드에는 문자가 그대로 입력된다.
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

		if(normal==60 || (normal<=60 && normal>54)) //텍스트레코드의 크기가 다 차거나 새로운 목적코드를 입력받을만한 공간이 없는 경우 새로운 텍스트레코드를 생성
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
			
		else if(strcmp(m_line->opcode,"RESW")==0 || strcmp(m_line->opcode,"RESB")==0) //opcode에 RESW나 RESB가 있을 경우 목적코드는 생성하지 않는다./
		{
			//목적프로그램은 새로 텍스트레코드를 생성하게된다.
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
			//RESW나 RESB가 끝나고 난뒤 다시 새로운 텍스트 레코드가 시작되도록 한다.
			for(int i=0;i<71;i++){
				text[i]='\0';
			}
				
			object<<"\n";
			text[0]='T';
			int len = atoi(m_line->loc);
			loc.Format("%06X",len);
			strcpy(&text[1],loc);
			object<<&text[0];
			normal=0; //텍스트 레코드가 새로 시작되기때문에 누적길이를 초기화 시켜준다.

			if(strcmp(m_line->label,"NULL")!=0 && OPTAB(m_line->opcode)!=0xFF && strcmp(m_line->operand,"NULL")!=0) //(loc,label,opcode,operand 모두 값이있을때)해당 라인의 목적코드 생성을 위해 opcode를 검사한다.
			{
				opcode.Format("%02X",OPTAB(m_line->opcode)); //optab에서 opcode의 값을 받아온다
				strcpy(&text[10+normal],opcode);
				object<<&text[10+normal];
				searchSymtab(m_line->operand,2); //symtab에서 해당 operand가 가리키는 라벨이 존재하는지 확인
				if(sympass2==0) //symtab에서 오퍼런드의 값과 비교하여 symbol이 있으면 주소값을 불러와서 입력함
				{
					int len=returnsymval;
					operand.Format("%04X",len);
					strcpy(&text[12+normal],operand);
					object<<&text[12+normal];
				}
				else if(sympass2==-1) //해당 operand가 symtab에 존재하지 않으면 undefined symbol에러를 출력
				{
					sympass2=0;
					CString str;
					str.Format("%s",m_line->operand);
					str+=" undefined symbol에러 입니다.";
					MessageBox(str);
					return;
				}			
				normal+=6; //다음 입력을 위해 6비트를 증가
			}
			else if(OPTAB(m_line->label)!=0xFF && strcmp(m_line->operand,"NULL")==0) //label에 opcode가 들어있는 경우 (opcode,operan만 있는경우)
			{
				if(strchr(m_line->opcode,',')) //,X로 인덱스 레지스터를 사용할 경우를 위해
				{
					CString indexcheck;
					strcpy(temp,m_line->label);
					strcpy(m_line->label,"");
					strcpy(temp1,m_line->opcode);
					strcpy(m_line->opcode,temp);
					strcpy(m_line->operand,temp1);
					
					indexcheck=strtok(m_line->operand,","); //","이전까지의 문자 즉 operand에 있는 symbol을 입력받는다.

					opcode.Format("%02X",OPTAB(m_line->opcode));
					strcpy(&text[10+normal],opcode);
					object<<&text[10+normal];

					strcpy(m_line->operand,indexcheck);
					searchSymtab(m_line->operand,2);
					if(sympass2==0) //symtab에서 오퍼런드의 값과 비교하여 symbol이 있으면 주소값을 불러와서 입력함
					{
						int index=returnsymval;
						index=index+32768;  //X는 목적코드상에서 16진수 8000으로 오퍼런드의 값에 10진수 32768을 더해준다.
						operand.Format("%04X",index);
						strcpy(&text[12+normal],operand);
						object<<&text[12+normal];
					}
					else if(sympass2==-1){
						sympass2=0;
						CString str;
						str.Format("%s",m_line->operand);
						str+=" undefined symbol에러 입니다.";
						MessageBox(str);
						return;
					}
				
				}
				else //","없이 opcode,operand만 있을 경우
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
					if(sympass2==0) //symtab에서 오퍼런드의 값과 비교하여 symbol이 있으면 주소값을 불러와서 입력함
					{
						operand.Format("%04x",returnsymval);
						strcpy(&text[12+normal],operand);
						object<<&text[12+normal];
					}
					else if(sympass2==-1){
						sympass2=0;
						CString str;
						str.Format("%s",m_line->operand);
						str+=" undefined symbol에러 입니다.";
						MessageBox(str);
						return;
					}
				
				}
				normal+=6;
			}
			else if(strcmp(m_line->label,"NULL")==0 && OPTAB(m_line->opcode)==0x4C && strcmp(m_line->operand,"NULL")==0) //RSUB가 opcode에 존재할 경우
			{
				opcode.Format("%02x",OPTAB(m_line->opcode));
				int zero=0;
				operand.Format("%04d",zero); //RSUB의 경우 목적코드에 operand값이 모두 0이다.
				strcpy(&text[10+normal],opcode);
				object<<&text[10+normal];
				strcpy(&text[12+normal],operand);
				object<<&text[12+normal];

				normal+=6;
			}
			else if(strcmp(m_line->opcode,"WORD")==0) //opcode에 WORD가 있을 경우
			{
				int zero=0;
				opcode.Format("%02x",zero); //WORD는 목적코드에 operand에서 입력받은 숫자를 그대로 입력한다.
				strcpy(&text[10+normal],opcode);
				int m_operand=atoi(m_line->operand);
				operand.Format("%04d",m_operand);
				object<<&text[10+normal];
				strcpy(&text[12+normal],operand);
				object<<&text[12+normal];
				normal+=6;
			}
			else if(strcmp(m_line->opcode,"BYTE")==0) //opcode에 BYTE가 있을 경우
			{
				if(strchr(m_line->operand,'C')) // C가 있을 경우 입력받은 문자를 16진수 ascii코드로 변환하여 목적코드에 입력한다.
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
				else if(strchr(m_line->operand,'X')) //X가 있을 경우 입력받은 문자를 문자 2개에 1바이트로 한다. 목적코드에는 문자가 그대로 입력된다.
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

void CChildView::OnAssSource() //Source Code 메뉴를 클릭하면 해당 텍스트 파일의 내용을 출력하는 함수
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
	
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CChildView::OnAssPass1() //Pass1 메뉴를 클릭하면 해당 텍스트 파일의 내용을 출력하는 함수
{
	Pass1("src.txt");
	MessageBox("SYMTAB과 Listfile이 생성되었습니다.");
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
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CChildView::OnAssSymtab() //SYMTAB 메뉴를 클릭하면 해당 텍스트 파일의 내용을 출력하는 함수
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
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CChildView::OnPass2Object() //목적프로그램 메뉴를 클릭하면 해당 텍스트 파일의 내용을 출력하는 함수
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
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

int CChildView::OPTAB(char* opcode)  //OPTAB함수로 char*형으로 opcode를 입력받아서 int형의 16진수로 해당 opcode의 값을 알려준다.
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

void CChildView::searchSymtab(char* symname, int pass) //symtab을 통한 오류검출을위해 pass1과 pass2에서 사용한다. 각각의 사용은 int pass변수로 구별한다.
{
	ifstream symtab("symtab.txt");
	ifstream pass2symtab("checkpass2sym.txt");
	SymTab *m_sym=new SymTab;

	if(pass==1){
		if(strcmp(symname,temp3)==0){
			sympass1=-1; //중복될때 -1이다.
		}
		else{
			sympass1=0; //중복되지 않을때 0;
		} //Pass1에서 duplicate symbol확인하는 if문
	}
	else if(pass==2){
		while(!pass2symtab.eof()){
			pass2symtab>>m_sym->name>>m_sym->m_value;
			
			if(strcmp(symname,m_sym->name)==0){
				returnsymval=atoi(m_sym->m_value);
				sympass2=0; //operand가 symtab에 존재할때
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

void CChildView::hexdec(int loc) //4자리의 16진수를 10진수로 변환할때 사용하는 함수로 int형 16진수를 입력받아서 전역변수인 Locctr에 변환된 10진수를 내보낸다.
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
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}
