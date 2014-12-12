#include "stdafx.h"
#include "server/ServerManager.h"
#include "server/ServerConnection.h"
#include "server/ServerTimer.h"

void ServerTimer::destroy()
{
	delete this;
}

void ServerTimer::timer_callback(unsigned int)
{
}
