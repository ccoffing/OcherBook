/*
 * Copyright (c) 2013, Chuck Coffing
 * OcherBook is released under the BSD 2-clause license.  See COPYING.
 */

#ifndef OCHER_FILETREE_H
#define OCHER_FILETREE_H

/** @file
 * Represents a simple filesystem in memory.
 */

#include <list>

#include "clc/data/Buffer.h"

class TreeFile
{
public:
    TreeFile(const clc::Buffer& _name) : name(_name) {}
    TreeFile(const clc::Buffer& _name, clc::Buffer& _data) : name(_name), data(_data) {}
    const clc::Buffer name;
    clc::Buffer data;
    // error
};

class TreeDirectory
{
public:
    TreeDirectory(const clc::Buffer& _name) : name(_name) {}
    ~TreeDirectory() {
        for (std::list<TreeFile*>::const_iterator i = files.begin(); i != files.end(); ++i)
            delete *i;
        for (std::list<TreeDirectory*>::const_iterator i = subdirs.begin(); i != subdirs.end(); ++i)
            delete *i;
    }

    clc::Buffer name;

    std::list<TreeDirectory*> subdirs;
    std::list<TreeFile*> files;

    TreeFile* createFilepp(const char* _name, const char* _data) {
        clc::Buffer nameBuf(_name);
        clc::Buffer dataBuf(_data);
        return createFile(nameBuf, dataBuf);
    }
    TreeFile* createFilep(const char* _name, clc::Buffer _data) {
        clc::Buffer nameBuf(_name);
        return createFile(nameBuf, _data);
    }
    TreeFile* createFile(const clc::Buffer& _name, clc::Buffer& _data) {
        TreeFile* file = getFile(_name);
        if (! file) {
            file = new TreeFile(_name, _data);
            files.push_back(file);
        }
        return file;
    }
    TreeDirectory* createDirectory(const char* _name) {
        return createDirectory(clc::Buffer(_name));
    }
    TreeDirectory* createDirectory(const clc::Buffer& _name) {
        TreeDirectory* dir = getDirectory(_name);
        if (! dir) {
            dir = new TreeDirectory(_name);
            subdirs.push_back(dir);
        }
        return dir;
    }

    TreeFile* getFile(const clc::Buffer& _name) const {
        for (std::list<TreeFile*>::const_iterator i = files.begin(); i != files.end(); ++i) {
            if ((*i)->name == _name)
                return *i;
        }
        return 0;
    }
    TreeFile* getFile(const char* _name) const {
        for (std::list<TreeFile*>::const_iterator i = files.begin(); i != files.end(); ++i) {
            if ((*i)->name == _name)
                return *i;
        }
        return 0;
    }
    TreeDirectory* getDirectory(const clc::Buffer& _name) const {
        for (std::list<TreeDirectory*>::const_iterator i = subdirs.begin(); i != subdirs.end(); ++i) {
            if ((*i)->name == _name)
                return *i;
        }
        return 0;
    }
    TreeFile* findFile(const char* _name) const {
        const char* slash = strchr(_name, '/');
        if (slash) {
            size_t len = slash - _name;
            for (std::list<TreeDirectory*>::const_iterator i = subdirs.begin(); i != subdirs.end(); ++i) {
                if ((*i)->name.length() == len && strncmp((*i)->name.c_str(), _name, len) == 0) {
                    return (*i)->findFile(slash+1);
                }
            }
        } else {
            for (std::list<TreeFile*>::const_iterator i = files.begin(); i != files.end(); ++i) {
                if ((*i)->name == _name) {
                    return (*i);
                }
            }
        }
        return 0;
    }
};


#endif
