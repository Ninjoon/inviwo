#pragma once

#include <ticpp/ticppapi.h>
#include <ticpp/fwd.h>

#include <ticpp/node.h>
#include <ticpp/attribute.h>

#include <sstream>

/** The element is a container class. It has a value, the element name,
    and can contain other elements, text, comments, and unknowns.
    Elements also contain an arbitrary number of attributes.
*/
class TICPP_API TiXmlElement : public TiXmlNode {
public:
    TiXmlElement(std::string_view value);
    TiXmlElement(const TiXmlElement&);
    void operator=(const TiXmlElement& base);
    virtual ~TiXmlElement();

    /** Given an attribute name, Attribute() returns the value
     *  for the attribute of that name, or null if none exists.
     */
    const std::string* Attribute(std::string_view name) const;

    /** Sets an attribute of name to a given value. The attribute
        will be created if it does not exist, or changed if it does.
    */
    void SetAttribute(std::string_view name, std::string_view _value);

    /** Deletes an attribute with the given name.
     */
    void RemoveAttribute(std::string_view name);

    /// Access the first attribute in this element.
    const TiXmlAttribute* FirstAttribute() const { return attributeSet.First(); }
    TiXmlAttribute* FirstAttribute() { return attributeSet.First(); }
    /// Access the first attribute in this element.
    const TiXmlAttribute* LastAttribute() const { return attributeSet.Last(); }
    TiXmlAttribute* LastAttribute() { return attributeSet.Last(); }

    /** Convenience function for easy access to the text inside an element. Although easy
        and concise, GetText() is limited compared to getting the TiXmlText child
        and accessing it directly.

        If the first child of 'this' is a TiXmlText, the GetText()
        returns the character string of the Text node, else null is returned.

        This is a convenient method for getting the text of simple contained text:
        @verbatim
        <foo>This is text</foo>
        const char* str = fooElement->GetText();
        @endverbatim

        'str' will be a pointer to "This is text".

        Note that this function can be misleading. If the element foo was created from
        this XML:
        @verbatim
        <foo><b>This is text</b></foo>
        @endverbatim

        then the value of str would be null. The first child node isn't a text node, it is
        another element. From this XML:
        @verbatim
        <foo>This is <b>text</b></foo>
        @endverbatim
        GetText() will return "This is ".

        WARNING: GetText() accesses a child node - don't become confused with the
                 similarly named TiXmlHandle::Text() and TiXmlNode::ToText() which are
                 safe type casts on the referenced node.
    */
    const std::string* GetText() const;

    /// Creates a new Element and returns it - the returned element is a copy.
    virtual TiXmlNode* Clone() const;
    // Print the Element to a FILE stream.
    virtual void Print(FILE* cfile, int depth) const;

    /*	Attribute parsing starts: next char past '<'
        returns: next char past '>'
    */
    virtual const char* Parse(const char* p, TiXmlParsingData* data);

    /// Cast to a more defined type. Will return null not of the requested type.
    virtual const TiXmlElement* ToElement() const { return this; }
    /// Cast to a more defined type. Will return null not of the requested type.
    virtual TiXmlElement* ToElement() { return this; }

    /// Walk the XML tree visiting this node and all of its children.
    virtual bool Accept(TiXmlVisitor* visitor) const;

protected:
    void CopyTo(TiXmlElement* target) const;
    void ClearThis();  // like clear, but initializes 'this' object as well

    /*	[internal use]
        Reads the "value" of the element -- another element, or text.
        This should terminate with the current end tag.
    */
    const char* ReadValue(const char* in, TiXmlParsingData* prevData);

private:
    TiXmlAttributeSet attributeSet;
};
