#ifndef LIBCLC_BUFFER_H
#define LIBCLC_BUFFER_H

#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stdint.h>

#ifdef USE_CLC_MEMRCHR
static inline void* memrchr(const void* s, int c, size_t n) {
    while (n-- > 0) {
        if ((int)(*((const char*)s)+n) == c)
            return (void*)(((const char*)s)+n);
    };
    return 0;
}
#endif

namespace clc
{

/**
 *  An efficient copy-on-write string, which can hold C strings or binary data.
 *  @note  This class is 8-bit clean.  If a length is required by the API, the Buffer is assumed
 *      to hold binary data and the length is honored.  If a length is not required or provided,
 *      then the Buffer is assumed to be a C string.
 *  @note  The Buffer is intended to be lightweight (copy-on-write with no internal locking).  If
 *      Buffers are intended to be shared among threads, either external locking must be used,
 *      or all threads must treat the Buffer as read-only, or after a thread gives a copy of the
 *      Buffer to another thread which may mutate it, the giving thread can only safely destroy
 *      its object but not dereference it.  If threading is not used, compile with SINGLE_THREADED.
 *  @todo  Where possible, make compatible with std::string
 *  @todo  fork vs _DetachWith  (fork is used in some places _DetachWith would be
 *          better; _DetachWith has dead code path; possibly combine?)
 */
class Buffer {
private:
    char* m_data;

public:
    /**
     * Special error value.
     */
    static const size_t NotFound = ((size_t)-1);

    Buffer();
    explicit Buffer(const char* string);
    Buffer(const char* string, size_t maxLength);
    Buffer(const Buffer& string);
    Buffer(int dummy, const char* fmt, ...) __attribute__((format(printf, 3, 4)));
    ~Buffer();

    // Drop-in for std::string:
    char* c_str() { return m_data; }
    const char* c_str() const { return m_data; }
    operator char*() { return m_data; }
    operator const char*() const { return m_data; }
    char* data() { return m_data; }
    const char* data() const { return m_data; }
    void clear() { setTo("", 0); }
    size_t length() const;
    size_t max_size() const { return ((size_t)-1) - sizeof(size_t) - 4; }
    size_t size() const { return length(); }
    bool empty() const { return length() == 0; }

    /**
     *  "Forks" the string, so that it is guaranteed to not share any data with any other strings.
     *  This is internally automatically called by mutating functions.
     */
    void fork();

    /**
     *  @return  Number of UTF8 characters.
     */
    size_t       countChars() const;

    inline bool isEmpty() const { return length() == 0; }

    // Assignment
    Buffer&    operator=(const Buffer& string);
    Buffer&    operator=(const char* string);
    Buffer&    operator=(char c);

    Buffer& formatList(const char* fmt, va_list argList);
    Buffer& format(const char* fmt, ...) __attribute__((format(printf, 2, 3)));
    Buffer& appendFormatList(const char* fmt, va_list argList);
    Buffer& appendFormat(const char* fmt, ...) __attribute__((format(printf, 2, 3)));

    Buffer&    setTo(const char* string);
    Buffer&    setTo(const char* string, size_t maxLength);

    Buffer&    setTo(const Buffer& string);
    Buffer&    adopt(Buffer& from);

    Buffer&    setTo(const Buffer& string, size_t maxLength);
    Buffer&    adopt(Buffer& from, size_t maxLength);

    Buffer&    setTo(char c, size_t count);

    // Substring copying
    Buffer&    CopyInto(Buffer& into, size_t fromOffset, size_t length) const;
    void        CopyInto(char* into, size_t fromOffset, size_t length) const;

    // Appending
    Buffer&    operator+=(const Buffer& string);
    Buffer&    operator+=(const char* string);
    Buffer&    operator+=(char c);

    Buffer&    append(const Buffer& string);
    Buffer&    append(const char* string);

    Buffer&    append(const Buffer& string, size_t length);
    Buffer&    append(const char* string, size_t length);
    Buffer&    append(char c, size_t count);

    // Prepending
    Buffer&    prepend(const char* string);
    Buffer&    prepend(const Buffer& string);
    Buffer&    prepend(const char* string, size_t length);
    Buffer&    prepend(const Buffer& string, size_t length);
    Buffer&    prepend(char c, size_t count);

    // Inserting
    Buffer&    insert(const char* string, ssize_t position);
    Buffer&    insert(const char* string, size_t length, ssize_t position);
    Buffer&    insert(const char* string, size_t fromOffset, size_t length,
                    ssize_t position);
    Buffer&    insert(const Buffer& string, ssize_t position);
    Buffer&    insert(const Buffer& string, size_t length, ssize_t position);
    Buffer&    insert(const Buffer& string, size_t fromOffset, size_t length,
                    ssize_t position);
    Buffer&    insert(char c, size_t count, ssize_t position);

    // Removing
    Buffer&    truncate(size_t newLength);
    Buffer&    remove(size_t from, size_t length);

    Buffer&    RemoveFirst(const Buffer& string);
    Buffer&    RemoveLast(const Buffer& string);
    Buffer&    RemoveAll(const Buffer& string);

    Buffer&    RemoveFirst(const char* string);
    Buffer&    RemoveLast(const char* string);
    Buffer&    RemoveAll(const char* string);

    Buffer&    RemoveSet(const char* setOfCharsToRemove);

    Buffer&    MoveInto(Buffer& into, size_t from, size_t length);
    void       MoveInto(char* into, size_t from, size_t length);

    // Compare functions
    bool        operator<(const Buffer& string) const;
    bool        operator<=(const Buffer& string) const;
    bool        operator==(const Buffer& string) const;
    bool        operator>=(const Buffer& string) const;
    bool        operator>(const Buffer& string) const;
    bool        operator!=(const Buffer& string) const;

    bool        operator<(const char* string) const;
    bool        operator<=(const char* string) const;
    bool        operator==(const char* string) const;
    bool        operator>=(const char* string) const;
    bool        operator>(const char* string) const;
    bool        operator!=(const char* string) const;

    /**
     *  Compares two strings.
     */
    int         Compare(const Buffer& string) const;
    /**
     *  Compares a string against a C string.
     */
    int         Compare(const char* string) const;
    /**
     *  Compares two strings, at most the first len characters.
     */
    int         Compare(const Buffer& string, size_t len) const;
    /**
     *  Compares a string against a C string.
     */
    int         Compare(const char* string, size_t len) const;
    /**
     *  Compares two C strings, case insensitive.
     */
    int         ICompare(const Buffer& string) const;
    /**
     *  Compares two C strings, case insensitive.
     */
    int         ICompare(const char* string) const;
    /**
     *  Compares two C strings, case insensitive, at most the first len characters.
     */
    int         ICompare(const Buffer& string, size_t len) const;
    /**
     *  Compares two C strings, case insensitive, at most the first len characters.
     */
    int         ICompare(const char* string, size_t len) const;
    int endsWith(const char* suffix);
    int endsWithI(const char* suffix);

    // Searching
    size_t        FindFirst(const Buffer& string) const;
    size_t        FindFirst(const char* string) const;
    size_t        FindFirst(const Buffer& string, size_t fromOffset) const;
    size_t        FindFirst(const char* string, size_t fromOffset) const;
    size_t        FindFirst(char c) const;
    size_t        FindFirst(char c, size_t fromOffset) const;

    size_t        FindLast(const Buffer& string) const;
    size_t        FindLast(const char* string) const;
    size_t        FindLast(const Buffer& string, size_t beforeOffset) const;
    size_t        FindLast(const char* string, size_t beforeOffset) const;
    size_t        FindLast(char c) const;
    size_t        FindLast(char c, size_t beforeOffset) const;

    size_t        IFindFirst(const Buffer& string) const;
    size_t        IFindFirst(const char* string) const;
    size_t        IFindFirst(const Buffer& string, size_t fromOffset) const;
    size_t        IFindFirst(const char* string, size_t fromOffset) const;

    size_t        IFindLast(const Buffer& string) const;
    size_t        IFindLast(const char* string) const;
    size_t        IFindLast(const Buffer& string, size_t beforeOffset) const;
    size_t        IFindLast(const char* string, size_t beforeOffset) const;

    // Replacing
    Buffer&    ReplaceFirst(char replaceThis, char withThis);
    Buffer&    ReplaceLast(char replaceThis, char withThis);
    Buffer&    ReplaceAll(char replaceThis, char withThis, size_t fromOffset = 0);
    Buffer&    Replace(char replaceThis, char withThis, unsigned int maxReplaceCount,
                    size_t fromOffset = 0);
    Buffer&    ReplaceFirst(const char* replaceThis, const char* withThis);
    Buffer&    ReplaceLast(const char* replaceThis, const char* withThis);
    Buffer&    ReplaceAll(const char* replaceThis, const char* withThis,
                    size_t fromOffset = 0);
    Buffer&    Replace(const char* replaceThis, const char* withThis,
                    unsigned int maxReplaceCount, size_t fromOffset = 0);

    Buffer&    IReplaceFirst(char replaceThis, char withThis);
    Buffer&    IReplaceLast(char replaceThis, char withThis);
    Buffer&    IReplaceAll(char replaceThis, char withThis, size_t fromOffset = 0);
    Buffer&    IReplace(char replaceThis, char withThis, unsigned int maxReplaceCount,
                    size_t fromOffset = 0);
    Buffer&    IReplaceFirst(const char* replaceThis, const char* withThis);
    Buffer&    IReplaceLast(const char* replaceThis, const char* withThis);
    Buffer&    IReplaceAll(const char* replaceThis, const char* withThis,
                    size_t fromOffset = 0);
    Buffer&    IReplace(const char* replaceThis, const char* withThis,
                    unsigned int maxReplaceCount, size_t fromOffset = 0);

    Buffer&    ReplaceSet(const char* setOfChars, char with);
    Buffer&    ReplaceSet(const char* setOfChars, const char* with);

    // Unchecked char access
    char       operator[](size_t index) const;

    char&  operator[](size_t index);

    // Checked char access
    char        ByteAt(size_t index) const;

    // Fast low-level manipulation
    /**
     *  Locks the buffer.  Buffer must be unlocked when done.
     *  (Or the object can be destroyed safely).
     *  @return  A buffer which may be written directly to, up to maxLength chars.
     */
    char*      lockBuffer(size_t maxLength);
    /**
     *  @param length  The amount of the locked buffer that was filled.  Default (0) implies
     *      using strlen to determine the length.
     */
    Buffer&    unlockBuffer(size_t length = 0);

    // Upercase <-> Lowercase
    //Buffer&    ToLower();
    //Buffer&    ToUpper();

    //Buffer&    Capitalize();

    // Escaping and De-escaping
    Buffer&    CharacterEscape(const char* original, const char* setOfCharsToEscape, char escapeWith);
    Buffer&    CharacterEscape(const char* setOfCharsToEscape, char escapeWith);
    Buffer&    CharacterDeescape(const char* original, char escapeChar);
    Buffer&    CharacterDeescape(char escapeChar);

    // Insert
    Buffer&    operator<<(const char* string);
    Buffer&    operator<<(const Buffer& string);
    Buffer&    operator<<(char c);
    Buffer&    operator<<(unsigned int value);
    Buffer&    operator<<(int32_t value);
    Buffer&    operator<<(uint64_t value);
    Buffer&    operator<<(int64_t value);
    // float output hardcodes %.2f style formatting
    Buffer&    operator<<(float value);

private:
    class PosVect;

    // Management
    char*        _Alloc(size_t length, bool adoptReferenceCount = true);
    char*        _Realloc(size_t length);
    void        _Init(const char* src, size_t length);
    char*        _Clone(const char* data, size_t length);
    char*        _OpenAtBy(size_t offset, size_t length);
    char*        _ShrinkAtBy(size_t offset, size_t length);
    void        _DetachWith(const char* string, size_t dataLen, size_t totalLen);

    // Data
    void        _SetLength(size_t length);
    void        _DoAppend(const char* string, size_t length);
    void        _DoPrepend(const char* string, size_t length);
    void        _DoInsert(const char* string, size_t offset, size_t length);

    // Search
    size_t        _ShortFindAfter(const char* string, size_t len) const;
    size_t        _FindAfter(const char* string, size_t offset, size_t strlen) const;
    size_t        _IFindAfter(const char* string, size_t offset, size_t strlen) const;

    size_t        _FindBefore(const char* string, size_t offset, size_t strlen) const;
    size_t        _IFindBefore(const char* string, size_t offset, size_t strlen) const;

    // Escape
    Buffer&    _DoCharacterEscape(const char* string,
                    const char* setOfCharsToEscape, char escapeChar);
    Buffer&    _DoCharacterDeescape(const char* string, char escapeChar);

    // Replace
    Buffer&    _DoReplace(const char* findThis, const char* replaceWith,
                    unsigned int maxReplaceCount, size_t fromOffset, bool ignoreCase);
    void        _ReplaceAtPositions(const PosVect* positions, size_t searchLen,
                    const char* with, size_t withLen);

    int32_t&        refCount();
    const int32_t&  refCount() const;
    int32_t incRef();
    int32_t decRef();

    bool           _IsShareable() const;
    void           _FreePrivateData();
};


// Commutative compare operators
bool operator<(const char* a, const Buffer& b);
bool operator<=(const char* a, const Buffer& b);
bool operator==(const char* a, const Buffer& b);
bool operator>(const char* a, const Buffer& b);
bool operator>=(const char* a, const Buffer& b);
bool operator!=(const char* a, const Buffer& b);


// Non-member compare for sorting, etc.
int Compare(const Buffer& a, const Buffer& b);
int ICompare(const Buffer& a, const Buffer& b);
int Compare(const Buffer* a, const Buffer* b);
int ICompare(const Buffer* a, const Buffer* b);


inline size_t
Buffer::length() const
{
    return *(((size_t*)m_data) - 1);
}


inline Buffer &
Buffer::setTo(const char* string)
{
    return operator=(string);
}


inline char
Buffer::operator[](size_t index) const
{
    return m_data[index];
}


inline char&
Buffer::operator[](size_t index)
{
    return m_data[index];
}

inline char
Buffer::ByteAt(size_t index) const
{
    if (index > length())
        return 0;
    return m_data[index];
}


inline Buffer &
Buffer::operator+=(const Buffer &string)
{
    _DoAppend(string.c_str(), string.length());
    return *this;
}


inline Buffer &
Buffer::append(const Buffer &string)
{
    _DoAppend(string.c_str(), string.length());
    return *this;
}


inline Buffer &
Buffer::append(const char* string)
{
    return operator+=(string);
}


inline bool
Buffer::operator==(const Buffer &string) const
{
    return Compare(string) == 0;
}


inline bool
Buffer::operator<(const Buffer &string) const
{
    return Compare(string) < 0;
}


inline bool
Buffer::operator<=(const Buffer &string) const
{
    return Compare(string) <= 0;
}


inline bool
Buffer::operator>=(const Buffer &string) const
{
    return Compare(string) >= 0;
}


inline bool
Buffer::operator>(const Buffer &string) const
{
    return Compare(string) > 0;
}


inline bool
Buffer::operator!=(const Buffer &string) const
{
    return Compare(string) != 0;
}


inline bool
Buffer::operator!=(const char* string) const
{
    return !operator==(string);
}


inline bool
operator<(const char* str, const Buffer &string)
{
    return string > str;
}


inline bool
operator<=(const char* str, const Buffer &string)
{
    return string >= str;
}


inline bool
operator==(const char* str, const Buffer &string)
{
    return string == str;
}


inline bool
operator>(const char* str, const Buffer &string)
{
    return string < str;
}


inline bool
operator>=(const char* str, const Buffer &string)
{
    return string <= str;
}


inline bool
operator!=(const char* str, const Buffer &string)
{
    return string != str;
}

}

#endif
