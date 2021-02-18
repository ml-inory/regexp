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

// 状态类型
enum E_STATE_TYPE
{
    STATE_SPLIT = 256,
    STATE_MATCH = 257
};

// 获取运算符优先级
int getOpPriority(int op);
// 比较运算符优先级
// > 0: lhs > rhs
// < 0: lhs < rhs
// = 0: lhs = rhs
int compareOpPriority(int lhs, int rhs);

// 状态
struct State
{
    int c;  // < 256: ASCII =256: Split 257: matchState
    State* out;
    State* out1;
    int lastlist;

    State(int _c, State* _out, State* _out1):
        c(_c), out(_out), out1(_out1)
    { }
};

// 片段
struct Frag
{
    State* start;
    std::vector<State**> out;

    Frag(State* _start = NULL):
        start(_start)
    { }

    Frag(State* _start, State** _outp):
        start(_start)
    {
        out.push_back(_outp);
    }

    void patch(Frag* other)
    {
        for (auto& pp : out)
            *pp = other->start;
    }

    void patch(State* other)
    {
        for (auto& pp : out)
            *pp = other;
    }

    void append(Frag* other)
    {
        for (auto& i : other->out)
            out.push_back(i);
    }

    void append(State** other)
    {
        out.push_back(other);
    }
};

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
    bool match(const char *expression);
    bool match(const std::string& expression);

private:
    // 中缀表达式转为后缀（逆波兰式）
    std::string re2post(const std::string& rule);
    // 判断某个字符是否为操作符
    bool isOp(char s);
    // 生成NFA
    Frag* post2nfa(const std::string& post);
    // 用NFA匹配
    bool match_nfa(const std::string& expression);
    

private:
    std::string _rule;
    std::string _post;
    Frag* _nfa;
};

#endif // __REGEXP_HPP__