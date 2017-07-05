#include <algorithm>
#include <iostream>
#include <regex>

int parse_line(std::string const& line)
{
	std::string tmp;
	if(std::regex_search(line, std::regex("(\\s)+(-)?(\\d)+\\/(-)?(\\d)+(\\s)+"))) {
		tmp = std::regex_replace(line, std::regex("(-)?(\\d)+//(-)?(\\d)+"), std::string("V"));
	} else if(std::regex_search(line, std::regex("(\\s)+(-)?(\\d)+/(-)?(\\d)+(\\s)+"))) {
		tmp = std::regex_replace(line, std::regex("(-)?(\\d)+/(-)?(\\d)+"), std::string("V"));
	} else if(std::regex_search(line, std::regex("(\\s)+(-)?(\\d)+/(-)?(\\d)+/(-)?(\\d)+(\\s)+"))) {
		tmp = std::regex_replace(line, std::regex("(-)?(\\d)+/(-)?(\\d)+/(-)?(\\d)+"), std::string("V"));
	} else {
		tmp = std::regex_replace(line, std::regex("(-)?(\\d)+"), std::string("V"));
	}
	return static_cast<int>(std::count(tmp.begin(), tmp.end(), 'V'));
}

int main()
{
	try
        {
		bool test = (parse_line("f 7/7/7 -3/3/-3 2/-2/2") == 3) &&
				(parse_line("f 7//7 3//-3 -2//2") == 3) &&
				(parse_line("f 7/7 3/-3 -2/2") == 3) &&
				(parse_line("f 7 3 -2") == 3);
                return test ? 0 : 1;
        }
	catch (std::regex_error& e)
	{
		std::string error_str;
		switch (e.code())
		{
		case std::regex_constants::error_collate:
			error_str = "collate"; break;
		case std::regex_constants::error_ctype:
			error_str = "ctype"; break;
		case std::regex_constants::error_escape:
			error_str = "escape"; break;
		case std::regex_constants::error_backref:
			error_str = "backref"; break;
		case std::regex_constants::error_brack:
			error_str = "brack"; break;
		case std::regex_constants::error_paren:
			error_str = "paren"; break;
		case std::regex_constants::error_brace:
			error_str = "brace"; break;
		case std::regex_constants::error_badbrace:
			error_str = "badbrace"; break;
		case std::regex_constants::error_range:
			error_str = "range"; break;
		case std::regex_constants::error_space:
			error_str = "space"; break;
		case std::regex_constants::error_badrepeat:
			error_str = "badrepeat"; break;
		case std::regex_constants::error_complexity:
			error_str = "complexity"; break;
		case std::regex_constants::error_stack:
			error_str = "stack"; break;
		default:
			error_str = "unknown";
		}
		std::cout << e.what() << ": " << e.code() << " (" << error_str << ")" << std::endl;
		return 1;		
	}
}
