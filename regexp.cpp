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
    _post = re2post(rule);
    printf("origin: %s\n", _rule.c_str());
    printf("postfix: %s\n", _post.c_str());
    _nfa = post2nfa(_post);
}

// 获取正则表达式
std::string RegExp::getRule(void)
{
    return _rule;
}

// 匹配，返回匹配到的字符串
bool RegExp::match(const char *expression)
{
    return match(std::string(expression));
}

bool RegExp::match(const std::string& expression)
{
    return match_nfa(expression);
}

// 用NFA匹配
bool RegExp::match_nfa(const std::string& expression)
{   
    printf("match nfa\n");
    State* s = _nfa->start;
    int i = 0;

    if (expression.empty()) return false;

    while (i < expression.size())
    {
        char exp_s = expression[i];
        bool found = false;
        if (s->c == STATE_SPLIT)
        {
            State* temp_s = s->out;
            State* temp_s1 = s->out1;
            printf("split\n");
            printf("matching: %c and %c\n", exp_s, temp_s->c);
            if (temp_s->c == exp_s)
            {
                s = temp_s;
                found = true;
            }

            printf("matching: %c and %c\n", exp_s, temp_s1->c);
            if (temp_s1 && temp_s1->c == exp_s)
            {
                s = temp_s1;
                found = true;
            }
        }
        else if (s->c == STATE_MATCH)
        {
            return false;
        }
        else
        {
            printf("matching: %c and %c\n", exp_s, s->c);
            if (s->c == exp_s)
            {
                found = true;
                s = s->out;
            }
        }

        if (!found)
        {
            return false;
        }
        else
        {
            i++;
        }
    }

    State* temp_s = s->out;
    State* temp_s1 = s->out1;
    if (s->c == STATE_MATCH || (temp_s && temp_s->c == STATE_MATCH) || (temp_s1 && temp_s1->c == STATE_MATCH))
    {
        return true;
    }

    return false;
}

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

    // printf("size: %d\n", operands.size());
    ret.resize(operands.size());
    size_t i = ret.size() - 1;
    while (!operands.empty())
    {
        ret[i--] = operands.top();
        operands.pop();
    }

    return ret;
}

// 判断某个字符是否为操作符
bool RegExp::isOp(char s)
{
    return (s == OP_LPAREN || s == OP_RPAREN || s == OP_ALTER || s == OP_CONCAT || s == OP_ONE_OR_MORE || s == OP_ZERO_OR_MORE || s == OP_ZERO_OR_ONE);
}

// 生成NFA
Frag* RegExp::post2nfa(const std::string& post)
{
    Frag* ret = new Frag;
    stack<Frag*> frag_stack;

    for (const char& c : post)
    {

        switch(c)
        {
            case OP_CONCAT:
            {
                Frag* e2 = frag_stack.top();
                frag_stack.pop();
                Frag* e1 = frag_stack.top();
                frag_stack.pop();
                e1->patch(e2);
                frag_stack.push(e1);
                break;
            }

            case OP_ALTER:
            {
                Frag* e2 = frag_stack.top();
                frag_stack.pop();
                Frag* e1 = frag_stack.top();
                frag_stack.pop();

                State* s = new State(STATE_SPLIT, e1->start, e2->start);
                Frag* f = new Frag(s);
                f->append(e1);
                f->append(e2);
                frag_stack.push(f);
                break;
            }
                
            case OP_ZERO_OR_ONE:
            {
                Frag* e1 = frag_stack.top();
                frag_stack.pop();
                State* s = new State(STATE_SPLIT, e1->start, NULL);
                Frag* f = new Frag(s);
                f->append(e1);
                f->append(&s->out1);
                break;
            }

            case OP_ZERO_OR_MORE:
            {
                Frag* e1 = frag_stack.top();
                frag_stack.pop();
                State* s = new State(STATE_SPLIT, e1->start, NULL);
                e1->patch(s);
                Frag* f = new Frag(s, &s->out1);
                frag_stack.push(f);
                break;
            }
                
            case OP_ONE_OR_MORE:
            {
                Frag* e1 = frag_stack.top();
                frag_stack.pop();
                State* s = new State(STATE_SPLIT, e1->start, NULL);
                e1->patch(s);
                Frag* f = new Frag(e1->start, &s->out1);
                frag_stack.push(f);
                break;
            }

            default:
            {
                State* s = new State(c, NULL, NULL);
                Frag* f = new Frag(s, &s->out);
                frag_stack.push(f);
                break;
            }
        }

        // printf("%c  %d\n", c, frag_stack.size());
    }

    if (!frag_stack.empty())
    {
        // printf("get ret\n");
        ret->start = frag_stack.top()->start;
        ret->out = frag_stack.top()->out;
        State* s = new State(STATE_MATCH, NULL, NULL);
        ret->patch(s);
    }
        
    return ret;
}