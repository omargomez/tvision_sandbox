/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   SCRNCELL.H                                                            */
/*                                                                         */
/*   Defines the low level structs used to represent text and attributes   */
/*   on the screen, most notably TCellAttribs and TScreenCell.             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#ifndef TVISION_SCRNCELL_H
#define TVISION_SCRNCELL_H

#ifdef __BORLANDC__

typedef ushort TScreenCell;
typedef uchar TCellAttribs;
typedef uchar TCellChar;

inline const TCellAttribs &getAttr(const TScreenCell &cell)
{
    return ((uchar *) &cell)[1];
}

inline void setAttr(TScreenCell &cell, TCellAttribs attr)
{
    ((uchar *) &cell)[1] = attr;
}

inline const TCellChar &getChar(const TScreenCell &cell)
{
    return ((uchar *) &cell)[0];
}

inline void setChar(TScreenCell &cell, TCellChar ch)
{
    ((uchar *) &cell)[0] = ch;
}

inline void setCell(TScreenCell &cell, TCellChar ch, TCellAttribs attr)
{
    cell = ushort((attr << 8) | ch);
}

#else

#include <cstring>
#include <type_traits>
#include <algorithm>

template<typename T>
struct alignas(T) trivially_convertible {

    // If you want the derived classes to be trivial, make sure you also
    // define a trivial default constructor in them.
    trivially_convertible() = default;

    trivially_convertible(T asT)
    {
        *this = asT;
    }

    T operator=(T asT)
    {
        memcpy(this, &asT, sizeof(T));
        return asT;
    }

    operator T() const
    {
        T asT;
        memcpy(&asT, this, sizeof(T));
        return asT;
    }

protected:

    template<class C>
    static constexpr bool check_trivial()
    {
        static_assert(std::is_trivial<C>(), "");
        static_assert(sizeof(C) == sizeof(T), "");
        static_assert(alignof(C) == alignof(T), "");
        return true;
    }

};

// TCellAttribs Attribute masks

const ushort
    afFgDefault = 0x0001,
    afBgDefault = 0x0002,
    afBold      = 0x0004,
    afItalic    = 0x0008,
    afUnderline = 0x0010,
    afReverse   = 0x0020;

struct TCellAttribs : trivially_convertible<uint16_t>
{

    uint8_t
        fgBlue      : 1,
        fgGreen     : 1,
        fgRed       : 1,
        fgBright    : 1,
        bgBlue      : 1,
        bgGreen     : 1,
        bgRed       : 1,
        bgBright    : 1;
    uint8_t
        fgDefault   : 1,
        bgDefault   : 1,
        bold        : 1,
        italic      : 1,
        underline   : 1,
        reverse     : 1;

private:
    uint8_t
        shadow      : 1;

    friend struct TVWrite;
public:

    using trivially_convertible::trivially_convertible;
    TCellAttribs() = default;

    TCellAttribs(uint8_t color, ushort flags)
    {
        *this = color | (flags << 8);
    }

    uint8_t fgGet() const
    {
        return uint8_t(*this) & 0x0F;
    }

    uint8_t bgGet() const
    {
        return uint8_t(*this) >> 4;
    }

    void fgSet(uint8_t fg)
    {
        if (fg != fgGet())
            fgDefault = 0;
        *this = (*this & ~0x0F) | (fg & 0x0F);
    }

    void bgSet(uint8_t bg)
    {
        if (bg != bgGet())
            bgDefault = 0;
        *this = (*this & ~0xF0) | ((bg & 0x0F) << 4);
    }

    void attrSet(TCellAttribs other)
    {
        *this = (*this & ~0xFF00) | (other & 0xFF00);
    }

    static constexpr void check_assumptions()
    {
        check_trivial<TCellAttribs>();
    }

};

struct TScreenCellA : trivially_convertible<uint16_t>
{

    uint8_t Char;
    uint8_t Attr;

    using trivially_convertible::trivially_convertible;
    TScreenCellA() = default;

    static constexpr void check_assumptions()
    {
        check_trivial<TScreenCellA>();
    }

};

struct alignas(4) TCellChar
{

    uint8_t bytes[12];

    TCellChar() = default;

    TCellChar(uint64_t ch)
    {
        *this = ch;
    }

    TCellChar(TStringView text)
    {
        *this = text;
    }

    TCellChar& operator=(uint64_t ch)
    {
        memset(this, 0, sizeof(*this));
        memcpy(bytes, &ch, std::min(sizeof(bytes), sizeof(ch)));
        return *this;
    }

    TCellChar& operator=(TStringView text)
    {
        memset(this, 0, sizeof(*this));
        if (text.size() <= sizeof(bytes))
            memcpy(bytes, text.data(), text.size());
        return *this;
    }

    bool operator==(TCellChar other) const
    {
        return memcmp(bytes, other.bytes, sizeof(bytes)) == 0;
    }

    bool operator!=(TCellChar other) const
    {
        return !(*this == other);
    }

    uint8_t& operator[](size_t i)
    {
        return bytes[i];
    }

    const uint8_t& operator[](size_t i) const
    {
        return bytes[i];
    }

    void append(TStringView text)
    {
        size_t sz = size();
        if (text.size() <= sizeof(bytes) - sz)
            memcpy(&bytes[sz], text.data(), text.size());
    }

    size_t size() const
    {
        size_t i = 0;
        while (++i < sizeof(bytes) && bytes[i]);
        return i;
    }

    TStringView asText() const
    {
        return {(const char*) bytes, size()};
    }

    static constexpr void check_assumptions()
    {
        static_assert(std::is_trivial<TCellChar>(), "");
    }

};

struct TScreenCell
{

    TCellChar Char;
    TCellAttribs Attr;
    uint8_t
        extraWidth : 3;

    TScreenCell() = default;

    TScreenCell(TScreenCellA pair)
    {
        *this = {};
        Char = pair.Char;
        Attr = pair.Attr;
    }

    TScreenCell(uint64_t ch)
    {
        memset(this, 0, sizeof(*this));
        memcpy(this, &ch, std::min(sizeof(*this), sizeof(ch)));
    }

    static constexpr uint32_t wideCharTrail = (uint32_t) -2;

    static constexpr void check_assumptions()
    {
        static_assert(std::is_trivial<TScreenCell>(), "");
    }

};

inline const TCellAttribs &getAttr(const TScreenCell &cell)
{
    return cell.Attr;
}

inline void setAttr(TScreenCell &cell, TCellAttribs attr)
{
    cell.Attr = attr;
}

inline const TCellChar &getChar(const TScreenCell &cell)
{
    return cell.Char;
}

inline void setChar(TScreenCell &cell, TCellChar ch, uchar extraWidth=0)
{
    TScreenCell c = cell;
    c.Char = ch;
    c.extraWidth = extraWidth;
    cell = c;
}

inline void setChar(TScreenCell &cell, TStringView text, uchar extraWidth=0)
{
    TCellChar ch = text;
    ::setChar(cell, ch, extraWidth);
}

inline void setChar(TScreenCell &cell, uchar ch)
{
    TScreenCell c = cell;
    c.Char = ch;
    c.extraWidth = 0;
    cell = c;
}

inline void setCell(TScreenCell &cell, TCellChar ch, TCellAttribs attr, uchar extraWidth=0)
{
    TScreenCell c = 0;
    ::setChar(c, ch, extraWidth);
    ::setAttr(c, attr);
    memcpy(&cell, &c, sizeof(TScreenCell));
}

#endif // __BORLANDC__

#endif
