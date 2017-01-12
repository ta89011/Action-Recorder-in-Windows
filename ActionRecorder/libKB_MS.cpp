#pragma once

#define _WIN32_WINNT	0x0500
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>
#include <windows.h>
#include <WINUSER.h>

int recordKey = 0x77;	// F8 to start/stop record key
int playKey = 0x78;	// F9 to start/stop player

MSG p_msg;
HHOOK g_mus_hook = 0, g_kb_hook = 0;
bool recording, playing, had_Cmd_data;
int waitingcmd;
double play_speeder = 1;
long long int PrevTime;
std::vector<InputKeyMouse> Record_ev_queue, vi_empty;
EventTaskTree* ett_Listening;

void toLowerStr(char* a)
{
	int i = 0;
	while(a[i] != '\0')
	{
		a[i] = ( (a[i] >= 'A' && a[i] <= 'Z') ? a[i]+32 : a[i] );
		i++;
	}
}

bool isbtn_status(){return had_Cmd_data;}

void hookCtrlCmd()
{
	SetCtrlCmd();
  g_kb_hook = SetWindowsHookEx (WH_KEYBOARD_LL, &kb_proc, GetModuleHandle(NULL), 0);
  if(g_kb_hook == NULL)
  {
  	//fprintf (stderr,"SetWindowsHookEx failed with error %d\n",::GetLastError());
  	exit(-1);
  }
}

void SetCtrlCmd()
{
	waitingcmd = 1;
	playing = recording = true;
	if(Record_ev_queue.size())had_Cmd_data = true;
	else had_Cmd_data = false;
}

void peek_an_msg()
{
	if(waitingcmd && PeekMessage(&p_msg, NULL,  0, 0, PM_REMOVE))
	{
		TranslateMessage(&p_msg);
		DispatchMessage(&p_msg);
	}
}

bool ExcuCMD()
{
	if(waitingcmd == playKey)toggle_playing();//RCplayer();
	else if(waitingcmd == recordKey)
	{
		StartRecord();
		return true;
	}
	return false;
}

void contact_eventQ(EventTaskTree& main_Q,double set_speed)
{
	ett_Listening = &main_Q;
	play_speeder = set_speed;
}

LRESULT CALLBACK mus_proc (int code, WPARAM w, LPARAM l)
{
	InputKeyMouse inpkm;
	inpkm.type = u_Mouse;

  PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)l;

  //see WinUser.h
  // the mouse action can express as 2^n
  if(w == WM_MOUSEMOVE)inpkm.act = 0;
  else if(w == WM_LBUTTONDOWN)inpkm.act = 1;
  else if(w == WM_LBUTTONUP)inpkm.act = 2;
  else if(w == WM_RBUTTONDOWN)inpkm.act = 3;
  else if(w == WM_RBUTTONUP)inpkm.act = 4;
  else if(w == WM_MBUTTONDOWN)inpkm.act = 5;
  else if(w == WM_MBUTTONUP)inpkm.act = 6;
  else if(w == WM_MOUSEWHEEL)inpkm.act = 11;

	inpkm.x = p->pt.x;
	inpkm.y = p->pt.y;
	if(GET_WHEEL_DELTA_WPARAM(p->mouseData) == ScrollUp)inpkm.msData = 'u';
	else if(GET_WHEEL_DELTA_WPARAM(p->mouseData) == ScrollDown)inpkm.msData = 'd';
	inpkm.dtime = (PrevTime ? p->time - PrevTime : 0);
	if(inpkm.dtime < 0)inpkm.dtime *= -1;
	Record_ev_queue.push_back(inpkm);

  PrevTime = p->time;
  return CallNextHookEx (g_mus_hook, code, w, l);
}

LRESULT CALLBACK kb_proc (int code, WPARAM w, LPARAM l)
{
  PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)l;
  if(waitingcmd && (w == WM_KEYUP || w == WM_SYSKEYUP))
  {
  	if(p->vkCode == playKey)waitingcmd = playKey;
		else if(p->vkCode == recordKey)waitingcmd = recordKey;
  }
	else if(!recording)
	{
		InputKeyMouse inpkm;
		inpkm.type = u_Keyboard;
		if (w == WM_KEYDOWN || w == WM_SYSKEYDOWN)inpkm.act = 0;
		else if (w == WM_KEYUP || w == WM_SYSKEYUP)inpkm.act = 1;
		inpkm.keyCode = p->vkCode;
		inpkm.dtime = (PrevTime ? p->time - PrevTime : 0);
		if(inpkm.dtime < 0)inpkm.dtime *= -1;
		Record_ev_queue.push_back(inpkm);
		PrevTime = p->time;
		if(p->vkCode == recordKey && 
			(w == WM_KEYUP || w == WM_SYSKEYUP))
			recording = true;
	}
	else if(!playing)
	{
		if(p->vkCode == playKey && 
			(w == WM_KEYUP || w == WM_SYSKEYUP))
			playing = true;
	}
  return CallNextHookEx (g_kb_hook, code, w, l);
}

void StartRecord()
{
	waitingcmd = 0;
	//need to change button enable/disable

  PrevTime = 0;
  recording = false;
  Record_ev_queue.clear();


  g_mus_hook = SetWindowsHookEx (WH_MOUSE_LL, &mus_proc, GetModuleHandle(NULL), 0);
  if (g_mus_hook == NULL)
  {
    //fprintf (stderr,"SetWindowsHookEx failed with error %d\n",::GetLastError());
    exit(-1);
  }

  while(!recording) {
  	if (PeekMessage(&p_msg, NULL,  0, 0, PM_REMOVE))
  	{
  		if (p_msg.message == WM_QUIT)break;
  		else {
  			TranslateMessage(&p_msg);
  			DispatchMessage(&p_msg);
  		}
  	}
  	Sleep(1);	//avoid busy waiting
  }

  UnhookWindowsHookEx (g_mus_hook);

  SetCtrlCmd();
  //need to change button enable/disable
}


void SetPlayer(const std::vector<InputKeyMouse>& Que_to_run)
{
	Record_ev_queue.clear();
	Record_ev_queue = Que_to_run;
}

void toSendAction(InputKeyMouse *d_act)
{
	INPUT inpkey = {0};
	if(d_act->type == u_Keyboard)
	{
		inpkey.type = INPUT_KEYBOARD;
		inpkey.ki.wVk = d_act->keyCode;
		inpkey.ki.dwFlags = d_act->act ? KEYEVENTF_KEYUP : 0;	//0 for press key , 1 for release key
	}
	else
	{
		inpkey.type = INPUT_MOUSE;
		inpkey.mi.dx = MsStepX*(d_act->x);
		inpkey.mi.dy = MsStepY*(d_act->y);
		inpkey.mi.mouseData = (d_act->msData == 'u' ? ScrollUp : ScrollDown);
		inpkey.mi.dwFlags = (1 << d_act->act) | MOUSEEVENTF_ABSOLUTE;
	}

  SendInput(1, &inpkey, sizeof(INPUT));
}

void RCplayer()
{
	if(Record_ev_queue.size() == 0)return;

	for(
		std::vector<InputKeyMouse>::iterator i = Record_ev_queue.begin();
		i != Record_ev_queue.end() && !playing;
		i++
	){
		Sleep( (int)((double)(i->dtime)/play_speeder) );
		toSendAction(&(*i));
		if(PeekMessage(&p_msg, NULL,  0, 0, PM_REMOVE))
		{
			TranslateMessage(&p_msg);
			DispatchMessage(&p_msg);
		}
	}
}

void WriteDataQ(string fna)
{
	ofstream fo(fna.c_str());
	for(
		std::vector<InputKeyMouse>::iterator i = Record_ev_queue.begin();
		i != Record_ev_queue.end();
		i++
	){
		fo<< i->dtime << ' ';
		if(i->type == u_Keyboard)
			fo << "keyboard" << ' ' << i->keyCode << ' ' << (i->act?"up":"down") << endl;
		else
		{
			fo << "mouse" << ' ' << i->x << ' ' << i->y << ' ';
			switch(i->act){
				case 0:	fo << "move";	break;
				case 1:	fo << "left_down";	break;
				case 2:	fo << "left_up";	break;
				case 3:	fo << "right_down";	break;
				case 4:	fo << "right_up";	break;
				case 5:	fo << "middle_down";	break;
				case 6:	fo << "middle_up";	break;
				case 11:
					fo << "scroll" << ' ';
					if(i->msData == 'u')fo << "scroll_up";
					else if(i->msData == 'd')fo << "scroll_down";
				break;
			}
			fo << endl;
		}
	}
	fo.close();
}

bool ReadDataQ(string fna)
{
	char tmpcmd[20];
	InputKeyMouse inpkm;
	Record_ev_queue.clear();
	ifstream fi(fna.c_str());
	while(fi >> inpkm.dtime >> tmpcmd)
	{
		toLowerStr(tmpcmd);
		if(!strcmp(tmpcmd,"keyboard"))
		{
			inpkm.type = u_Keyboard;
			fi >> inpkm.keyCode >> tmpcmd;
			toLowerStr(tmpcmd);
			if(!strcmp(tmpcmd,"up"))inpkm.act = 1;
			else inpkm.act = 0;
		}
		else
		{
			inpkm.msData = 0;
			inpkm.type = u_Mouse;
			fi >> inpkm.x >> inpkm.y >> tmpcmd;
			toLowerStr(tmpcmd);
			if(!strcmp(tmpcmd,"move"))inpkm.act = 0;
			else if(!strcmp(tmpcmd,"left_down"))inpkm.act = 1;
			else if(!strcmp(tmpcmd,"left_up"))inpkm.act = 2;
			else if(!strcmp(tmpcmd,"right_down"))inpkm.act = 3;
			else if(!strcmp(tmpcmd,"right_up"))inpkm.act = 4;
			else if(!strcmp(tmpcmd,"middle_down"))inpkm.act = 5;
			else if(!strcmp(tmpcmd,"middle_up"))inpkm.act = 6;
			else if(!strcmp(tmpcmd,"scroll"))
			{
				inpkm.act = 11;
				fi >> tmpcmd;
				toLowerStr(tmpcmd);
				if(!strcmp(tmpcmd,"scroll_up"))inpkm.msData = 'u';
				else if(!strcmp(tmpcmd,"scroll_down"))inpkm.msData = 'd';
			}
		}
		Record_ev_queue.push_back(inpkm);
	}
	fi.close();
	return true;
}

bool FileExistCheck(string fna)
{
	ifstream fi(fna);
	return fi.good();
}

std::vector<int> random_child(std::vector<int> chlds,int require_size)
{
	int sumofRate = 0, comparer, tmpnum, chNumber = chlds.size(),i;
	std::vector<int> weight_v(chNumber), rand_rlt(require_size);
	
	for(i=0;i<chNumber;i++)
	{
		tmpnum = (ett_Listening->find(chlds[i]))->weight;
		weight_v[i] = tmpnum;
		sumofRate += tmpnum;
	}

	while(require_size--)
	{
		comparer = 0;
		tmpnum = rand()%sumofRate;
		for(i=0;i<chNumber;i++)
		{
			comparer += weight_v[i];
			if(comparer > tmpnum)break;
		}
		rand_rlt[require_size] = chlds[i];

		sumofRate -= weight_v[i];
		weight_v.erase(weight_v.begin() + i );
		chlds.erase(chlds.begin() + i );
		chNumber--;
	}

	return rand_rlt;
}

bool toggle_recording()
{
	if(!recording)recording = true;
	else if(playing){
		StartRecord();
		return true;
	}
	return false;
}

void play_to_end(EventTaskTree* ett_ptr)
{
	while(!playing && ett_ptr->p_nxt!=NULL)
	{
		ett_ptr = ett_ptr->p_nxt;
		if(ett_ptr->parent != -1)continue;

		if(ett_ptr->type == act_OneJob)
		{
			Record_ev_queue.clear();
			Record_ev_queue = ett_ptr->ev_Q;
			RCplayer();
		}
		else
		{
			std::vector<int> vi_chdID = ett_Listening->find_children(ett_ptr->id);
			if(vi_chdID.size() == 0)continue;
			std::vector<int> rand_results = random_child(vi_chdID,ett_ptr->rand_size);

			for(int i = 0 ; i<ett_ptr->rand_size && !playing ; i++)
			{
				Record_ev_queue.clear();
				Record_ev_queue = ett_Listening->find(rand_results[i])->ev_Q;
				RCplayer();
				if(ett_ptr->replaytype && i<ett_ptr->rand_size-1) //the tpye is play to end and replay at this node
				{
					EventTaskTree* sub_ev_ptr = ett_ptr;
					play_to_end(sub_ev_ptr);
				}
			}
		}
	}

}

void toggle_playing()
{
	if(!playing)playing = true;
	else if(recording){
		if(ett_Listening->p_nxt == NULL)return;
		EventTaskTree* ett_ptr = ett_Listening;
		
		srand(time(NULL));

		waitingcmd = 0;
		playing = false;
		while(!playing)
		{
			if(ett_ptr->p_nxt == NULL)ett_ptr = ett_Listening;
			play_to_end(ett_ptr);
		}
		SetCtrlCmd();
	}
}

void CombineEvents()
{
	Record_ev_queue.clear();
	EventTaskTree* ett_ptr = ett_Listening->p_nxt;
	while(ett_ptr != NULL)
	{
		if(ett_ptr->type == act_OneJob)
			Record_ev_queue.insert(Record_ev_queue.end(), ett_ptr->ev_Q.begin(), ett_ptr->ev_Q.end());
		ett_ptr = ett_ptr->p_nxt;
	}
}

void EventTaskTree::add(int i, int t, int pa, int p,int p_num,int re, string na, \
	string pth,EventTaskTree *p_n)
{
	EventTaskTree *p_toNext = p_n;
	if(t == act_OneJob)
		this->p_nxt = new EventTaskTree(i, t, pa, p, p_num, re, na, pth, Record_ev_queue, p_toNext);
	else
		this->p_nxt = new EventTaskTree(i, t, pa, p, p_num, re, na, pth, vi_empty, p_toNext);
}

void EventTaskTree::Delete(int rmID)
{
	EventTaskTree *ee_ptr = this;
	while(ee_ptr->p_nxt != NULL && ee_ptr->p_nxt->id != rmID)
		ee_ptr = ee_ptr->p_nxt;
	EventTaskTree *rmder = ee_ptr->p_nxt;
	if(rmder == NULL)return;	//invalid id
	if(rmder->type == act_RandJob)
	{
		std::vector<int> ch_s = this->find_children(rmder->id);
		for(int i = ch_s.size()-1;i>=0;i--)
			this->Delete(ch_s[i]);
	}
	ee_ptr->p_nxt = rmder->p_nxt;
	delete rmder;
}

std::vector<int> EventTaskTree::find_children(int par_id)
{
	std::vector<int> chs;
	EventTaskTree *ee_ptr = this->p_nxt;
	while(ee_ptr != NULL)
	{
		if(ee_ptr->parent == par_id)chs.push_back(ee_ptr->id);
		ee_ptr = ee_ptr->p_nxt;
	}
	return chs;
}

EventTaskTree* EventTaskTree::find(int fid)
{
	EventTaskTree *ee_ptr = this->p_nxt;
	while(ee_ptr->id != fid)ee_ptr = ee_ptr->p_nxt;
	return ee_ptr;
}

EventTaskTree* EventTaskTree::find(string na)
{
	EventTaskTree *ee_ptr = this->p_nxt;
	while(ee_ptr->idName.compare(na))ee_ptr = ee_ptr->p_nxt;
	return ee_ptr;
}
