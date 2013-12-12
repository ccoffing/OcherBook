/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include <ctype.h>

#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "ocher/fmt/Layout.h"


Layout::Layout() :
    m_dataLen(0),
    nl(0),
    ws(0),
    pre(0),
    m_text(new clc::Buffer),
    m_textLen(0)
{
    m_text->lockBuffer(chunk);
    m_data.lockBuffer(chunk);
}

Layout::~Layout()
{
    // Walk the bytecode and delete embedded strings
    const unsigned int N = m_data.size();
    const char* raw = m_data.data();
    for (unsigned int i = 0; i < N; ) {
        uint16_t code = *(uint16_t*)(raw+i);
        i += 2;
        unsigned int opType = (code>>12)&0xf;
        unsigned int op = (code>>8)&0xf;
        if (opType == OpCmd && op == CmdOutputStr) {
            delete *(clc::Buffer**)(raw+i);
            i += sizeof(clc::Buffer*);
        }
    }

    delete m_text;
}

clc::Buffer Layout::unlock()
{
    flushText();
    delete m_text;
    m_text = 0;

    m_data.unlockBuffer(m_dataLen);
    return m_data;
}

char* Layout::checkAlloc(unsigned int n)
{
    if (m_dataLen + n > m_data.size()) {
        m_data.unlockBuffer(m_data.size());
        m_data.lockBuffer(m_dataLen + n + chunk);
    }
    char* p = m_data.c_str() + m_dataLen;
    m_dataLen += n;
    return p;
}

void Layout::push(unsigned int opType, unsigned int op, unsigned int arg)
{
    char* p = checkAlloc(2);
    uint16_t i = (opType<<12) | (op<<8) | arg;
    *(uint16_t*)p = i;
}

void Layout::pushPtr(void* ptr)
{
    int n = sizeof(ptr);
    char* p = checkAlloc(n);
    *((char**)p) = (char*)ptr;
}

void Layout::pushTextAttr(TextAttr attr, uint8_t arg)
{
    push(OpPushTextAttr, attr, arg);
}

void Layout::popTextAttr(unsigned int n)
{
    push(OpCmd, CmdPopAttr, n);
}

void Layout::pushLineAttr(LineAttr attr, uint8_t arg)
{
    push(OpPushLineAttr, attr, arg);
}

void Layout::popLineAttr(unsigned int n)
{
    push(OpCmd, CmdPopAttr, n);
}

inline void Layout::_outputChar(char c)
{
    if (m_textLen == chunk) {
        flushText();
    }
    (*m_text)[m_textLen++] = c;
}

void Layout::outputChar(char c)
{
    if (isspace(c)) {
        if (! ws) {
            ws = 1;
            _outputChar(' ');
        }
    } else {
        nl = 0;
        ws = 0;
        _outputChar(c);
    }
}

void Layout::outputNl()
{
    if (! nl) {
        _outputChar('\n');
        nl = 1;
    }
}

void Layout::outputBr()
{
    _outputChar('\n');
    nl = 1;
}

void Layout::flushText()
{
    if (m_textLen) {
        push(OpCmd, CmdOutputStr, 0);
        m_text->unlockBuffer(m_textLen);
        pushPtr(m_text);
        // m_text pointer is now owned by the layout bytecode.
        m_text = new clc::Buffer;
        m_text->lockBuffer(chunk);
        m_textLen = 0;
    }
}

void Layout::outputPageBreak()
{
    push(OpCmd, CmdForcePage, 0);
}
