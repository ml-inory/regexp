#ifndef __REGEXP_HPP__
#define __REGEXP_HPP__

#include <string>
#include <vector>

// 运算符枚举
enum E_OP
{
    OP_LPAREN = '(',
    OP_RPAREN = ')',
    OP_ALTER = '|',     // alternation
    OP_CONCAT = '.',    // concatenation
    OP_ZERO_OR_ONE = '?',
    OP_ONE_OR_MORE = '+',
    OP_ZERO_OR_MORE = '*'
};

// 获取运算符优先级
int getOpPriority(int op);
// 比较运算符优先级
// > 0: lhs > rhs
// < 0: lhs < rhs
// = 0: lhs = rhs
int compareOpPriority(int lhs, int rhs);

class RegExp
{
public:
    RegExp();
    RegExp(const char *rule);
    RegExp(const std::string& rule);

    ~RegExp();

    // 设置正则表达式
    void setRule(const std::string& rule);
    // 获取正则表达式
    std::string getRule(void);

    // 匹配，返回匹配到的字符串
    // std::vector<std::string> match(const char *expression);
    // std::vector<std::string> match(const std::string& expression);

private:
    // 中缀表达式转为后缀（逆波兰式）
    std::string re2post(const std::string& rule);
    // 判断某个字符是否为操作符
    bool isOp(char s);

private:
    std::string _rule;
};

#endif // __REGEXP_HPP__