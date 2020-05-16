#pragma once
#include "../BigHead.h"
class Cheat
{
public:
	string name;
	string category;
	bool enabled = false;
	bool wasEnabled = false;
	ulong keyBind = 0x0;
	Cheat(string name, string category);
	virtual void onLoop();
	virtual void onTick();
	virtual void onEnable();
	virtual void onDisable();
	virtual void onKey(ulong key);
	virtual void onPreRender();
	virtual void onPostRender();
};
