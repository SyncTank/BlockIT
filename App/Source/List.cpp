#include "List.h"

namespace App 
{
	void getListIteams()
	{
		//Core::PrintHelloWorld();
		
		std::unordered_map<std::wstring, std::vector<DWORD>> totalProcessList;
		//std::vector<std::wstring> blackList;
		
		Core::ProcessList(totalProcessList);

		//if (totalProcessList.find(test) != totalProcessList.end())
		//{
		//	Core::KillProcess(totalProcessList[test]);
		//	totalProcessList.erase(test);
		//}
		
		//Core::IterProcess_CPP(Processes);

	}

	void setText(int switchbase, std::string str)
	{
		switch (switchbase)
		{
		default:
			break;

		case 0:
			str = " ";
		}
	}


}