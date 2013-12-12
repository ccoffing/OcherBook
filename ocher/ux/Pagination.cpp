/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "ocher/ux/Pagination.h"

#define LOG_NAME "ocher.pagination"


Pagination::Pagination() :
    m_numPages(0)
{
}

Pagination::~Pagination()
{
    flush();
}

void Pagination::flush()
{
    m_numPages = 0;
    while (m_pages.size())
        delete[] (PageMapping*)m_pages.remove();
}

void Pagination::set(unsigned int pageNum, unsigned int layoutOffset, unsigned int strOffset /* TODO attrs */)
{
    ASSERT(pageNum <= m_numPages);
    unsigned int chunk = pageNum / pagesPerChunk;
    if (pageNum == m_numPages) {
        if (chunk == m_pages.countItems()) {
            m_pages.add(new PageMapping[pagesPerChunk]);
        }
    }
    struct PageMapping* mapping = (struct PageMapping*)m_pages.itemAtFast(chunk);
    mapping += pageNum % pagesPerChunk;
    if (mapping->layoutOffset != layoutOffset || mapping->strOffset != strOffset) {
        mapping->layoutOffset = layoutOffset;
        mapping->strOffset = strOffset;
        if (m_numPages <= pageNum)
            m_numPages = pageNum + 1;
        clc::Log::debug(LOG_NAME, "set page %u breaks at layoutOffset %u strOffset %u", pageNum, layoutOffset, strOffset);
    }
}

bool Pagination::get(unsigned int pageNum, unsigned int* layoutOffset, unsigned int* strOffset /* TODO attrs */)
{
    clc::Log::debug(LOG_NAME, "get page %u", pageNum);
    if (pageNum > m_numPages)
        return false;
    unsigned int chunk = pageNum / pagesPerChunk;
    if (chunk > m_pages.countItems()) {
        return false;
    }
    struct PageMapping* mapping = (struct PageMapping*)m_pages.itemAtFast(chunk);
    mapping += pageNum % pagesPerChunk;
    *layoutOffset = mapping->layoutOffset;
    *strOffset = mapping->strOffset;
    clc::Log::debug(LOG_NAME, "found page %u breaks at layoutOffset %u strOffset %u", pageNum, *layoutOffset, *strOffset);
    return true;
}
