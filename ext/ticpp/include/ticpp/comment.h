#pragma once

#include <ticpp/ticppapi.h>
#include <ticpp/fwd.h>

#include <ticpp/node.h>

/**	An XML comment.
 */
class TICPP_API TiXmlComment final : public TiXmlNode {
public:
    /// Constructs an empty comment.
    TiXmlComment(const allocator_type& alloc = {}) : TiXmlNode(TiXmlNode::COMMENT, "", alloc) {}
    /// Construct a comment from text.
    TiXmlComment(std::string_view _value, const allocator_type& alloc = {})
        : TiXmlNode(TiXmlNode::COMMENT, _value, alloc) {}
        
    TiXmlComment(const TiXmlComment&);
    void operator=(const TiXmlComment& base);

    virtual ~TiXmlComment() {}

    /// Returns a copy of this Comment.
    virtual TiXmlNode* Clone() const;
    // Write this Comment to a FILE stream.
    virtual void Print(FILE* cfile, int depth) const;

    /*	Attribute parsing starts: at the ! of the !--
        returns: next char past '>'
    */
    virtual const char* Parse(const char* p, TiXmlParsingData* data, const allocator_type& alloc);

    /// Cast to a more defined type. Will return null not of the requested type.
    virtual const TiXmlComment* ToComment() const { return this; }
    /// Cast to a more defined type. Will return null not of the requested type.
    virtual TiXmlComment* ToComment() { return this; }

    /// Walk the XML tree visiting this node and all of its children.
    virtual bool Accept(TiXmlVisitor* visitor) const;

protected:
    void CopyTo(TiXmlComment* target) const;
};
