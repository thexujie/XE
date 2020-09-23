#include "PCH.h"
#include "Regex.h"
#include <regex>

namespace XE
{
	bool FRegex::Match(FStringView String)
	{
		std::regex_constants::syntax_option_type StdFlags = {};
		SetFlag(StdFlags, std::regex_constants::icase, GetFlag(RegexFlags, ERegexFlags::IgnoreCase));
		std::basic_regex<char, std::regex_traits<char>> StdRegex((const char *)Expression.Data, Expression.Size, StdFlags);
		return std::regex_match((const char *)String.Begin(), (const char *)String.End(), StdRegex);
	}
	
	bool FRegex::Search(FStringView String)
	{
		Result.Clear();
		std::regex_constants::syntax_option_type StdFlags = {};
		SetFlag(StdFlags, std::regex_constants::icase, GetFlag(RegexFlags, ERegexFlags::IgnoreCase));
		std::basic_regex<char, std::regex_traits<char>> StdRegex((const char *)Expression.Data, Expression.Size, StdFlags);
		std::match_results<const char *> StdResults;
		bool StdIsMatched = std::regex_search((const char *)String.Begin(), (const char *)String.End(), StdResults, StdRegex);
		if (!StdIsMatched)
			return false;
		for (auto & StdResult : StdResults)
		{
			if (StdResult.matched)
				Result.Add(FStringView((const Char *)StdResult.first, StdResult.second - StdResult.first));
			else
				Result.Add(FStringView());
		}
		return true;
	}
}
