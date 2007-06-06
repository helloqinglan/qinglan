
#include "GuildWarPch.h"
#include "Util/Util.h"

using namespace std;

// ·Ö¸ô×Ö·û´®
vector<string> strSplit(const string& src, const string& sep)
{
	vector<string> r;
	string s;
	for (string::const_iterator i = src.begin(); i != src.end(); i++)
	{
		if (sep.find(*i) != string::npos)
		{
			if (s.length()) r.push_back(s);
			s = "";
		}
		else
		{
			s += *i;
		}
	}
	if (s.length()) 
		r.push_back(s);
	return r;
}
