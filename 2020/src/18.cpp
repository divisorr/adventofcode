#include "common.h"
#include "fileio.h"

/*Shunting-yard algorithm processes numbers bottom to top, which brakes part1.
The following algorithm processes top to bottom and evaluates immediately whatever it can.*/

class Stack
{
public:
	bool empty() const { return cur==0; }
	void pop() { --cur; }
	long long top() { return arr[cur]; }
	void push(long long val) { arr[++cur]=val; }
private:
	long long arr[32]{};
	int cur=0;
};

template <bool Part2>
class Parser
{
public:
	void Evaluate(long long token)
	{
		if (token == cb)
		{
			/*Evaluation of closing braces should always happen. In case
			the closing brace is the last symbol of the line there could be
			numbers and operators left in the stack, so we have to carry the whole evaluation.*/
			token = Result();
			stack.pop();  //Pop the cb operator
		}
		if (token >= 0)
		{
			if (stack.empty())
				stack.push(token);
			else
			{
				long long c = stack.top();
				if (c == plus)
				{
					stack.pop();
					long long lhs = stack.top();
					stack.pop();
					stack.push(lhs+token);
				}
				else if (c == mult)
				{
					if (!Part2)
					{
						stack.pop();
						long long lhs = stack.top();
						stack.pop();
						stack.push(lhs*token);
					}
					else
						stack.push(token);
				}
				else
					stack.push(token);
			}
		}
		else
			stack.push(token);
	}

	long long Result()
	{
		long long top = stack.top();
		stack.pop();
		if (Part2)
		{
			while (!stack.empty() && stack.top()!=ob)
			{
				long long temp = stack.top();
				if (temp >= 0)
					top *= temp;
				stack.pop();  //This is either a number or multiplication
			}
		}
		return top;
	}

private:
	Stack stack;
	const static long long plus = -5; //Plus sign:       '+'-'0' == -5
	const static long long mult = -6; //Multiplication:  '*'-'0' == -6
	const static long long cb = -7;   //Closing bracket: ')'-'0' == -7
	const static long long ob = -8;   //Opening bracket: '('-'0' == -8
};


output day18(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	Parser<false> p1;
	Parser<true> p2;
	long long part1=0, part2=0;
	size_t& pos = file.pos;
	const char nl = file.nl;
	//"<=" becase the last line of the input doesn't have line terminating symbol
	while (pos <= bytes)
	{
		if (input[pos] == ' ')
			++pos;
		else
		{
			if (input[pos] > nl)
			{
				p1.Evaluate(input[pos]-'0');
				p2.Evaluate(input[pos]-'0');
				++pos;
			}
			else
			{
				part1 += p1.Result();
				part2 += p2.Result();
				file.ignorenl();
			}
		}
	}
	return {part1, part2};
}
