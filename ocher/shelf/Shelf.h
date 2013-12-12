/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_SHELF_H
#define OCHER_SHELF_H

#include "clc/data/List.h"


class Meta;

/**
 * A grouping of books (physical or logical).  Groups are both both subject and observer (groups
 * can potentially aggregate and filter other groups).
 */
class GroupOfBooks
{
public:
	virtual ~GroupOfBooks() {}

	void attach(GroupOfBooks* observer);
	void detach(GroupOfBooks* observer);
	void notify();

	// TODO: updateMetadata vs updateMembership ?
	virtual void update(GroupOfBooks*) {}
	virtual const clc::List* getList() const = 0;

protected:
	clc::List m_observers;
};


/**
 * A Shelf is a filtered, sorted, "view" of other GroupOfBooks.
 */
class Shelf : public GroupOfBooks
{
public:
	Shelf();
	~Shelf();

	enum SortKeys {
		ByAuthor,
		By
	};

	/**
	 * Shelves watch other GroupOfBooks and must implement update().
	 */
	virtual void update(GroupOfBooks* changed) = 0;

#if 0
	addFilterTag();
	removeFilterTag();
	sort();
	clearSort();
#endif
};


/**
 * The Library contains references to all of the user's books.  Unlike the Shelf, it owns the
 * actual books.
 */
class Library : public GroupOfBooks
{
public:
	Library() {}
	~Library();

	/**
	 * Adds the metadata to the Library.  Ownership is transferred.
	 * Caller should call notify() when done adding.
	 */
	void add(Meta*);

	const clc::List* getList() const { return &m_meta; }

protected:
	clc::List m_meta;
};


/**
 */
class ShortList : public Shelf
{
public:
	ShortList(GroupOfBooks* base) : m_base(base) { m_base->attach(this); }
	~ShortList() { m_base->detach(this); }

	void update(GroupOfBooks* changed);
	const clc::List* getList() const { return &m_meta; }

protected:
	clc::List m_meta;
	GroupOfBooks* m_base;
};

#endif
