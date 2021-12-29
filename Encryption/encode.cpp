#include <sstream>
#include <vector>

using namespace std;

static wstring c1(L"Õ·±»");
static wstring c2(L"∞Õ≤∑");
static wchar_t cd(L'∑ä');

__forceinline wstring _encode(const wstring& str)
{
	wstringstream ws;
	for (auto c : str)
	{
		switch (c)
		{
		case L'a':
			ws << c1 << c2;
			break;
		case L'b':
			ws << c2 << c1 << c1 << c1;
			break;
		case L'c':
			ws << c2 << c1 << c2 << c1;
			break;
		case L'd':
			ws << c2 << c1 << c1;
			break;
		case L'e':
			ws << c1;
			break;
		case L'f':
			ws << c1 << c1 << c2 << c1;
			break;
		case L'0':
			ws << c2 << c2 << c1;
			break;
		case L'1':
			ws << c1 << c1 << c1 << c1;
			break;
		case L'2':
			ws << c1 << c1;
			break;
		case L'3':
			ws << c1 << c2 << c2 << c2;
			break;
		case L'4':
			ws << c2 << c1 << c2;
			break;
		case L'5':
			ws << c1 << c2 << c1 << c1;
			break;
		case L'6':
			ws << c2 << c2;
			break;
		case L'7':
			ws << c2 << c1;
			break;
		case L'8':
			ws << c2 << c2 << c2;
			break;
		case L'9':
			ws << c1 << c2 << c2 << c1;
			break;
		case L',':
			ws << c2;
			break;
		default:
			throw exception("undefind char");
		}
		ws << cd;
	}
	return ws.str();
}

wstring encode(const wstring& str)
{
	wstringstream ws;
	for (UINT16 c : str)
	{
		ws << hex << c;
		ws << ',';
	}
	return _encode(ws.str());
}

__forceinline char _decode(const vector<char>& v)
{
	stringstream ss;
	for (auto c : v)ss.put(c);
	auto&& s = ss.str();
	if (s._Equal("01"))
	{
		return 'a';
	}
	if (s._Equal("1000"))
	{
		return 'b';
	}
	if (s._Equal("1010"))
	{
		return 'c';
	}
	if (s._Equal("100"))
	{
		return 'd';
	}
	if (s._Equal("0"))
	{
		return 'e';
	}
	if (s._Equal("0010"))
	{
		return 'f';
	}
	if (s._Equal("110"))
	{
		return '0';
	}
	if (s._Equal("0000"))
	{
		return '1';
	}
	if (s._Equal("00"))
	{
		return '2';
	}
	if (s._Equal("0111"))
	{
		return '3';
	}
	if (s._Equal("101"))
	{
		return '4';
	}
	if (s._Equal("0100"))
	{
		return '5';
	}
	if (s._Equal("11"))
	{
		return '6';
	}
	if (s._Equal("10"))
	{
		return '7';
	}
	if (s._Equal("111"))
	{
		return '8';
	}
	if (s._Equal("0110"))
	{
		return '9';
	}
	if (s._Equal("1"))
	{
		return ',';
	}
	throw exception("undefind char");
}

__forceinline wchar_t _decode2(const vector<char>& v)
{
	stringstream ss;
	for (auto c : v)ss.put(c);
	UINT32 i;
	ss >> hex >> i;
	return (wchar_t)i;
}

wstring decode(const wstring& str)
{
	wstringstream ws;
	vector<char> v, v2;
	wchar_t buff[3]{};
	int id = 0;
	for (wchar_t c : str)
	{
		if (c == cd)
		{
			id = 0;
			auto dc = _decode(v);
			v.clear();
			if (dc == ',')
			{
				ws.put(_decode2(v2));
				v2.clear();
				continue;
			}
			v2.push_back(dc);
			continue;
		}
		buff[id] = c;
		if (id == 1)
		{
			if (wstring(buff)._Equal(c1))
			{
				v.push_back('0');
			}
			else if (wstring(buff)._Equal(c2))
			{
				v.push_back('1');
			}
			else
			{
				throw exception("unknow exception");
			}
		}
		++id %= 2;
	}
	return ws.str();
}