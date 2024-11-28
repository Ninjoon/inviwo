#pragma once

#include <ticpp/ticppapi.h>
#include <ticpp/fwd.h>
#include <ticpp/base.h>

#include <string>
#include <memory_resource>

/** An attribute is a name-value pair. Elements have an arbitrary
 * number of attributes, each with a unique name.
 *
 * @note The attributes are not TiXmlNodes, since they are not
 * part of the tinyXML document object model. There are other
 * suggested ways to look at this problem.
 */
class TICPP_API TiXmlAttribute {
    friend class TiXmlAttributeSet;

public:
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

    TiXmlAttribute(const allocator_type& alloc = {});
    TiXmlAttribute(TiXmlCursor _location, const allocator_type& alloc = {});
    TiXmlAttribute(std::string_view _name, std::string_view _value,
                   const allocator_type& alloc = {});

    TiXmlAttribute(const TiXmlAttribute&) = delete;
    TiXmlAttribute& operator=(const TiXmlAttribute& base) = delete;
    TiXmlAttribute(TiXmlAttribute&&) = delete;
    TiXmlAttribute& operator=(TiXmlAttribute&& base) = delete;

    ~TiXmlAttribute();

    std::string_view Name() const { return name; }    ///< Return the name of this attribute.
    std::string_view Value() const { return value; }  ///< Return the value of this attribute.

    void SetName(std::string_view _name) { name = _name; }
    void SetValue(std::string_view _value) { value = _value; }

    /// Get the next sibling attribute in the DOM. Returns null at end.
    const TiXmlAttribute* Next() const;
    TiXmlAttribute* Next();

    /// Get the previous sibling attribute in the DOM. Returns null at beginning.
    const TiXmlAttribute* Previous() const;
    TiXmlAttribute* Previous();

    bool operator==(const TiXmlAttribute& rhs) const { return rhs.name == name; }
    bool operator<(const TiXmlAttribute& rhs) const { return name < rhs.name; }
    bool operator>(const TiXmlAttribute& rhs) const { return name > rhs.name; }

    // Prints this Attribute to a FILE stream.
    void Print(FILE* file) const;
    void Print(std::string* str) const;

    int Row() const { return location.row + 1; }
    int Column() const { return location.col + 1; }

private:
    std::pmr::string name;
    std::pmr::string value;
    TiXmlAttribute* prev;
    TiXmlAttribute* next;
    TiXmlCursor location;
};

/**	A class used to manage a group of attributes.
 * It is only used internally, both by the ELEMENT and the DECLARATION.
 *
 * The set can be changed transparent to the Element and Declaration
 * classes that use it, but NOT transparent to the Attribute
 * which has to implement a next() and previous() method. Which makes
 * it a bit problematic and prevents the use of STL.
 *
 * This version is implemented with circular lists because:
 *  - I like circular lists
 *  - it demonstrates some independence from the (typical) doubly linked list.
 */
class TICPP_API TiXmlAttributeSet {
public:
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

    TiXmlAttributeSet(const allocator_type& alloc = {});
    ~TiXmlAttributeSet();
    TiXmlAttributeSet(const TiXmlAttributeSet&) = delete;
    TiXmlAttributeSet& operator=(const TiXmlAttributeSet&) = delete;

    void Add(std::string_view name, std::string_view value);
    void Remove(std::string_view name);
    void Clear();

    const TiXmlAttribute* First() const {
        return (sentinel.next == &sentinel) ? nullptr : sentinel.next;
    }
    TiXmlAttribute* First() { return (sentinel.next == &sentinel) ? nullptr : sentinel.next; }
    const TiXmlAttribute* Last() const {
        return (sentinel.prev == &sentinel) ? nullptr : sentinel.prev;
    }
    TiXmlAttribute* Last() { return (sentinel.prev == &sentinel) ? nullptr : sentinel.prev; }

    const TiXmlAttribute* Find(std::string_view name) const;
    TiXmlAttribute* Find(std::string_view name) {
        return const_cast<TiXmlAttribute*>(
            (const_cast<const TiXmlAttributeSet*>(this))->Find(name));
    }

    /*	Attribute parsing starts: first letter of the name
        returns: the next char after the value end quote
    */
    const char* Parse(const char* p, TiXmlParsingData* data);

private:
    TiXmlAttribute sentinel;
};
