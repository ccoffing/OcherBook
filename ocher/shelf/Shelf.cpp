/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "ocher/shelf/Meta.h"
#include "ocher/shelf/Shelf.h"

#define LOG_NAME "ocher.shelf"


void GroupOfBooks::attach(GroupOfBooks* observer)
{
    m_observers.add(observer);
}

void GroupOfBooks::detach(GroupOfBooks* observer)
{
    m_observers.removeItem(observer);
}

void GroupOfBooks::notify()
{
    unsigned int n = m_observers.size();
    for (unsigned int i = 0; i < n; ++i) {
        GroupOfBooks* group = (GroupOfBooks*)m_observers.get(i);
        group->update(this);
    }
}


Shelf::Shelf()
{
}

Shelf::~Shelf()
{
}


Library::~Library()
{
    size_t n = m_meta.countItems();
    for (size_t i = 0; i < n; ++i) {
        delete (Meta*)m_meta.get(i);
    }

    notify();
}

void Library::add(Meta* meta)
{
    m_meta.add(meta);
}


void ShortList::update(GroupOfBooks* changed)
{
    m_meta.clear();
    const clc::List* books = changed->getList();
    unsigned int n = books->size();
    for (unsigned int i = 0; i < n; ++i) {
        Meta* meta = (Meta*)books->get(i);
        if (meta->record.shortlist) {
            m_meta.add(meta);
        }
    }
}

