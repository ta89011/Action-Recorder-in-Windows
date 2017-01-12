#pragma once

#define _WIN32_WINNT	0x0500
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <windows.h>
#include <WINUSER.h>

#define MsStepX 65535.0f/(::GetSystemMetrics( SM_CXSCREEN )-1)
#define MsStepY 65535.0f/(::GetSystemMetrics( SM_CYSCREEN )-1)
#define ScrollUp  GET_WHEEL_DELTA_WPARAM(0x780000)  //this is WHEEL_DELTA
#define ScrollDown GET_WHEEL_DELTA_WPARAM(0xff880000)
#define u_Keyboard 0
#define u_Mouse 1

#define act_RandJob 0
#define act_OneJob 1

using namespace std;

//mouse: act: 
// 0: move
// 1: Left button down
// 2: Left button up
// 3: Right button down
// 4: Right button up
// 5: Middle button down
// 6: Middle button up
// 11: wheel scroll
struct InputKeyMouse {
	int act;	// the Event action
	int dtime; //wait time before this input (millisecond)
	
	//for key board
	int keyCode;
	
	//for mouse
	LONG x;
	LONG y;
	char msData;

	// u_Keyboard or u_Mouse
	bool type;
};

class EventTaskTree{
public:
	EventTaskTree():p_nxt(NULL){}
	EventTaskTree(int i,int t,int pa,int p,int p_num, int re,string na,string pth,std::vector<InputKeyMouse> quQ,EventTaskTree *p_n):
		id(i), type(t), parent(pa), weight(p), rand_size(p_num), replaytype(re), idName(na), Nam_path(pth), ev_Q(quQ), p_nxt(p_n) {}
	~EventTaskTree(){}

	int id;
	int type;
	int parent;
	int weight;
	int rand_size;
	int replaytype;	//play to end or play in this
	string idName;
	string Nam_path;
	std::vector<InputKeyMouse> ev_Q;
	EventTaskTree *p_nxt;

	void add(int,int,int,int,int,int,string,string,EventTaskTree*);
	void Delete(int);
	std::vector<int> find_children(int);
	EventTaskTree* find(int);
	EventTaskTree* find(string);
};

LRESULT CALLBACK mus_proc (int code, WPARAM w, LPARAM l);
LRESULT CALLBACK kb_proc (int code, WPARAM w, LPARAM l);

void toLowerStr(char* a);
void hookCtrlCmd();
void SetCtrlCmd();
void peek_an_msg();
bool ExcuCMD();
bool isbtn_status();
void contact_eventQ(EventTaskTree&,double);
void StartRecord();
void SetPlayer(const std::vector<InputKeyMouse>&);
void toSendAction(InputKeyMouse *d_act);
void RCplayer();
bool toggle_recording();
void toggle_playing();
bool FileExistCheck(string fna);
void WriteDataQ(string fna);
bool ReadDataQ(string fna);
std::vector<int> random_child(std::vector<int>,int);
void CombineEvents();
