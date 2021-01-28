#include "regexp.hpp"
#include <stack>
#include <stdio.h>

using namespace std;

// 获取运算符优先级
int getOpPriority(int op)
{
    if (op == OP_LPAREN || op == OP_RPAREN)
        return 4;
    else if (op == OP_ALTER)
        return 1;
    else if (op == OP_CONCAT)
        return 2;
    else if (op == OP_ZERO_OR_ONE || op == OP_ONE_OR_MORE || op == OP_ZERO_OR_MORE)
        return 3;
    else
        return -1;
}

// 比较运算符优先级
// > 0: lhs > rhs
// < 0: lhs < rhs
// = 0: lhs = rhs
int compareOpPriority(int lhs, int rhs)
{
    return getOpPriority(lhs) - getOpPriority(rhs);
}

RegExp::RegExp()
{
    setRule("");
}

RegExp::RegExp(const char *rule):
    RegExp(std::string(rule))
{

}

RegExp::RegExp(const std::string& rule)
{
    setRule(rule);
    
}

RegExp::~RegExp()
{

}

// 设置正则表达式
void RegExp::setRule(const std::string& rule)
{
    _rule = rule;
    std::string postfix = re2post(rule);
    printf("origin: %s\n", rule.c_str());
    printf("postfix: %s\n", postfix.c_str());
}

// 获取正则表达式
std::string RegExp::getRule(void)
{
    return _rule;
}

// 匹配，返回匹配到的字符串
// std::vector<std::string> RegExp::match(const char *expression)
// {
//     return match(expression);
// }

// 中缀表达式转为后缀（逆波兰式）
std::string RegExp::re2post(const std::string& rule)
{
    std::string ret;
    // 运算符栈
    stack<char> ops;
    // 操作数栈
    stack<char> operands;

    // 连续concat的次数
    int concat_num = 0;
    for (const auto& c : rule)
    {
        // printf("%c\n", c);
        // 是运算符
        if (isOp(c))
        {
            // ops空
            if (ops.empty())
            {
                concat_num = 0;
                ops.push(c);
            }
            else
            {
                // 遇到)
                if (c == OP_RPAREN)
                {
                    // 弹出栈顶，把运算符推到operands，直到栈顶为(
                    while (!ops.empty() && ops.top() != OP_LPAREN)
                    {
                        operands.push(ops.top());
                        ops.pop();
                    }
                    if (!ops.empty() && ops.top() == OP_LPAREN)
                        ops.pop();
                }
                else
                {
                    concat_num = 0;
                    // c的优先级比栈顶高
                    if (compareOpPriority(c, ops.top()) >= 0)
                    {
                        ops.push(c);
                    }
                    else
                    {
                        // 弹出栈顶，放入operands
                        operands.push(ops.top());
                        ops.pop();
                        ops.push(c);
                    }
                }
            }
        }
        else
        {
            concat_num++;
            operands.push(c);
            if (concat_num >= 2)
                ops.push(OP_CONCAT);
        }
    }
    while (!ops.empty())
    {
        operands.push(ops.top());
        ops.pop();
    }

    printf("size: %d\n", operands.size());
    ret.resize(operands.size());
    for (size_t i = ret.size() - 1; i > 0; i--)
    {
        ret[i] = operands.top();
        // printf("ret[%d] = %c\n", i, ret[i]);
        operands.pop();
    }
    ret[0] = operands.top();

    return ret;
}

// 判断某个字符是否为操作符
bool RegExp::isOp(char s)
{
    return (s == OP_LPAREN || s == OP_RPAREN || s == OP_ALTER || s == OP_CONCAT || s == OP_ONE_OR_MORE || s == OP_ZERO_OR_MORE || s == OP_ZERO_OR_ONE);
}