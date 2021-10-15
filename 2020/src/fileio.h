#pragma once


class Fileio
{
public:
	Fileio(const char* input, size_t bytes, bool CRLF=false);
		//Read only positive numbers
	template <class T>
	T ReadPositive();
		//Advances the current position until an invalid symbol is reached.
		//Returns the number of chars skipped.
	size_t AdvanceString();
	bool eof();
	void ignore(unsigned idx=1);
	void ignorenl();

public:
		//input
	const char* text;
		//size of input
	const size_t size;
		//current get position
	size_t pos;
		//size of line terminator - 2 for CRLF, 1 for LF
	const unsigned nlSize;
		//first symbol for line terminator - /r for CRLF, /n for LF
	const char nl;
};


inline Fileio::Fileio(const char* input, size_t bytes, bool _CRLF)
	:
	text(input)
	,size(bytes)
	,nlSize(_CRLF? 2 : 1)
	,nl(_CRLF? '\r' : '\n')
{
	pos = 0;
}

template <class T>
inline T Fileio::ReadPositive()
{
	T res = 0;
	for (unsigned idx=1; pos<size; ++idx)
	{
		if ('0'<=text[pos] && text[pos]<='9')
			res = res*10 + (text[pos++]-'0');
		else
			return res;
	}
	return res;
}

inline size_t Fileio::AdvanceString()
{
	size_t beg = pos;
	while (pos<size && text[pos]>32)
		++pos;
	return pos-beg;
}

inline bool Fileio::eof()
{
	return pos >= size;
}

inline void Fileio::ignore(unsigned idx)
{
	pos += idx;
}

inline void Fileio::ignorenl()
{
	pos += nlSize;
}
