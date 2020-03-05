// util.cpp
//
// Utils
//
// Marco Bibuli 04/03/2020


#include "util.h"

void parse_cmd(std::vector<std::string>& cmd_msg , std::string cmd_string)
{
	int i = 0;
	char s[BUF_SIZE];
	std::string str;

	int k = 0;
	do
	{
		if (cmd_string[i] == ' ' || cmd_string[i] == NULL)
		{
			if (k > 0)
			{
				s[k] = NULL;
				str = s;
				cmd_msg.push_back(str);
				k = 0;
			}
		}
		else
		{
			s[k] = cmd_string[i];
			k++;
		}
	} while (cmd_string[i++] != NULL);
	
}
 




