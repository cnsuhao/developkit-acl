#include "stdafx.h"
#include "client/ClientManager.h"
#include "server/ServerManager.h"
#include "client/ClientConnection.h"
#include "server/ServerConnection.h"
#include "status/StatusTimer.h"

void StatusTimer::destroy()
{
	delete this;
}

void StatusTimer::timer_callback(unsigned int)
{
	logger("total client: %d, total server: %d",
		(int) ClientManager::get_instance().length(),
		(int) ServerManager::get_instance().length());
}
