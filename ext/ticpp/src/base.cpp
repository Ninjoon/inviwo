#include <ticpp/base.h>

#include <istream>
#include <cstring>

#include <fmt/format.h>

bool TiXmlBase::condenseWhiteSpace = true;

void TiXmlBase::EncodeString(const std::string& str, std::string* outString) {
    int i = 0;

    while (i < (int)str.length()) {
        unsigned char c = (unsigned char)str[i];

        if (c == '&' && i < ((int)str.length() - 2) && str[i + 1] == '#' && str[i + 2] == 'x') {
            // Hexadecimal character reference.
            // Pass through unchanged.
            // &#xA9;	-- copyright symbol, for example.
            //
            // The -1 is a bug fix from Rob Laveaux. It keeps
            // an overflow from happening if there is no ';'.
            // There are actually 2 ways to exit this loop -
            // while fails (error case) and break (semicolon found).
            // However, there is no mechanism (currently) for
            // this function to return an error.
            while (i < (int)str.length() - 1) {
                outString->append(str.c_str() + i, 1);
                ++i;
                if (str[i] == ';') break;
            }
        } else if (c == entity[0].chr) {
            outString->append(entity[0].str);
            ++i;
        } else if (c == entity[1].chr) {
            outString->append(entity[1].str);
            ++i;
        } else if (c == entity[2].chr) {
            outString->append(entity[2].str);
            ++i;
        } else if (c == entity[3].chr) {
            outString->append(entity[3].str);
            ++i;
        } else if (c == entity[4].chr) {
            outString->append(entity[4].str);
            ++i;
        } else if (c < 32) {
            // Easy pass at non-alpha/numeric/symbol
            // Below 32 is symbolic.
            outString->append(fmt::format("&#x{:02X};", c));
            ++i;
        } else {
            *outString += static_cast<char>(c);
            ++i;
        }
    }
}

const char* TiXmlBase::errorString[TIXML_ERROR_STRING_COUNT] = {
    "No error",
    "Error",
    "Failed to open file",
    "Memory allocation failed.",
    "Error parsing Element.",
    "Failed to read Element name",
    "Error reading Element value.",
    "Error reading Attributes.",
    "Error: empty tag.",
    "Error reading end tag.",
    "Error parsing Unknown.",
    "Error parsing Comment.",
    "Error parsing Declaration.",
    "Error document empty.",
    "Error null (0) or unexpected EOF found in input stream.",
    "Error parsing CDATA.",
    "Error when TiXmlDocument added to document, because TiXmlDocument can only be at the root.",
};

// Bunch of unicode info at:
//		http://www.unicode.org/faq/utf_bom.html
// Including the basic of this table, which determines the #bytes in the
// sequence from the lead byte. 1 placed for invalid sequences --
// although the result will be junk, pass it through as much as possible.
// Beware of the non-characters in UTF-8:
//				ef bb bf (Microsoft "lead bytes")
//				ef bf be
//				ef bf bf

// clang-format off
const int TiXmlBase::utf8ByteTable[256] = {
	//	0	1	2	3	4	5	6	7	8	9	a	b	c	d	e	f
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x00
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x10
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x20
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x30
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x40
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x50
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x60
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x70	End of ASCII range
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x80 0x80 to 0xc1 invalid
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x90
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0xa0
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0xb0
		1,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	// 0xc0 0xc2 to 0xdf 2 byte
		2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	// 0xd0
		3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	// 0xe0 0xe0 to 0xef 3 byte
		4,	4,	4,	4,	4,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1	// 0xf0 0xf0 to 0xf4 4 byte, 0xf5 and higher invalid
};
// clang-format on

void TiXmlBase::ConvertUTF32ToUTF8(unsigned long input, char* output, int* length) {
    const unsigned long BYTE_MASK = 0xBF;
    const unsigned long BYTE_MARK = 0x80;
    const unsigned long FIRST_BYTE_MARK[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

    if (input < 0x80)
        *length = 1;
    else if (input < 0x800)
        *length = 2;
    else if (input < 0x10000)
        *length = 3;
    else if (input < 0x200000)
        *length = 4;
    else {
        *length = 0;
        return;
    }  // This code won't covert this correctly anyway.

    output += *length;

    // Scary scary fall throughs.
    switch (*length) {
        case 4:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 3:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 2:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 1:
            --output;
            *output = (char)(input | FIRST_BYTE_MARK[*length]);
    }
}

bool TiXmlBase::IsAlpha(int anyByte) {
    // This will only work for low-ascii, everything else is assumed to be a valid
    // letter. I'm not sure this is the best approach, but it is quite tricky trying
    // to figure out alphabetical vs. not across encoding. So take a very
    // conservative approach.

    if (anyByte < 127)
        return std::isalpha(anyByte);
    else
        return true;  // What else to do? The unicode set is huge...get the english ones right.
}

bool TiXmlBase::IsAlphaNum(int anyByte) {
    // This will only work for low-ascii, everything else is assumed to be a valid
    // letter. I'm not sure this is the best approach, but it is quite tricky trying
    // to figure out alhabetical vs. not across encoding. So take a very
    // conservative approach.

    if (anyByte < 127)
        return std::isalnum(anyByte);
    else
        return true;  // What else to do? The unicode set is huge...get the english ones right.
}

const char* TiXmlBase::SkipWhiteSpace(const char* p) {
    if (!p || !*p) {
        return nullptr;
    }

    while (*p) {
        const unsigned char* pU = (const unsigned char*)p;

        // Skip the UTF-8 Byte order marks
        if (*(pU + 0) == TIXML_UTF_LEAD_0 && *(pU + 1) == TIXML_UTF_LEAD_1 &&
            *(pU + 2) == TIXML_UTF_LEAD_2) {
            p += 3;
            continue;
        } else if (*(pU + 0) == TIXML_UTF_LEAD_0 && *(pU + 1) == 0xbfU && *(pU + 2) == 0xbeU) {
            p += 3;
            continue;
        } else if (*(pU + 0) == TIXML_UTF_LEAD_0 && *(pU + 1) == 0xbfU && *(pU + 2) == 0xbfU) {
            p += 3;
            continue;
        }

        // Still using old rules for white space.
        if (IsWhiteSpace(*p) || *p == '\n' || *p == '\r') {
            ++p;
        } else {
            break;
        }
    }

    return p;
}

/*static*/ bool TiXmlBase::StreamWhiteSpace(std::istream* in, std::string* tag) {
    for (;;) {
        if (!in->good()) return false;

        int c = in->peek();
        // At this scope, we can't get to a document. So fail silently.
        if (!IsWhiteSpace(c) || c <= 0) return true;

        *tag += (char)in->get();
    }
}

/*static*/ bool TiXmlBase::StreamTo(std::istream* in, int character, std::string* tag) {
    // assert( character > 0 && character < 128 );	// else it won't work in utf-8
    while (in->good()) {
        int c = in->peek();
        if (c == character) return true;
        if (c <= 0)  // Silent failure: can't get document at this scope
            return false;

        in->get();
        *tag += (char)c;
    }
    return false;
}

// One of TinyXML's more performance demanding functions. Try to keep the memory overhead down. The
// "assign" optimization removes over 10% of the execution time.
const char* TiXmlBase::ReadName(const char* p, std::string* name) {
    // Oddly, not supported on some compilers,
    // name->clear();
    // So use this:
    *name = "";
    assert(p);

    // Names start with letters or underscores.
    // Of course, in unicode, tinyxml has no idea what a letter *is*. The
    // algorithm is generous.
    //
    // After that, they can be letters, underscores, numbers,
    // hyphens, or colons. (Colons are valid only for namespaces,
    // but tinyxml can't tell namespaces from names.)
    if (p && *p && (IsAlpha(*p) || *p == '_')) {
        const char* start = p;
        while (p && *p && (IsAlphaNum(*p) || *p == '_' || *p == '-' || *p == '.' || *p == ':')) {
            //(*name) += *p; // expensive
            ++p;
        }
        if (p - start > 0) {
            name->assign(start, p - start);
        }
        return p;
    }
    return nullptr;
}

const char* TiXmlBase::GetEntity(const char* p, char* value, int* length) {
    // Presume an entity, and pull it out.
    std::string ent;
    int i;
    *length = 0;

    if (*(p + 1) && *(p + 1) == '#' && *(p + 2)) {
        unsigned long ucs = 0;
        ptrdiff_t delta = 0;
        unsigned mult = 1;

        if (*(p + 2) == 'x') {
            // Hexadecimal.
            if (!*(p + 3)) return 0;

            const char* q = p + 3;
            q = strchr(q, ';');

            if (!q || !*q) return 0;

            delta = q - p;
            --q;

            while (*q != 'x') {
                if (*q >= '0' && *q <= '9')
                    ucs += mult * (*q - '0');
                else if (*q >= 'a' && *q <= 'f')
                    ucs += mult * (*q - 'a' + 10);
                else if (*q >= 'A' && *q <= 'F')
                    ucs += mult * (*q - 'A' + 10);
                else
                    return 0;
                mult *= 16;
                --q;
            }
        } else {
            // Decimal.
            if (!*(p + 2)) return 0;

            const char* q = p + 2;
            q = strchr(q, ';');

            if (!q || !*q) return 0;

            delta = q - p;
            --q;

            while (*q != '#') {
                if (*q >= '0' && *q <= '9')
                    ucs += mult * (*q - '0');
                else
                    return 0;
                mult *= 10;
                --q;
            }
        }

        // convert the UCS to UTF-8
        ConvertUTF32ToUTF8(ucs, value, length);

        return p + delta + 1;
    }

    // Now try to match it.
    for (i = 0; i < entity.size(); ++i) {
        if (strncmp(entity[i].str.data(), p, entity[i].str.size()) == 0) {
            *value = entity[i].chr;
            *length = 1;
            return (p + entity[i].str.size());
        }
    }

    // So it wasn't an entity, its unrecognized, or something like that.
    *value = *p;  // Don't put back the last one, since we return it!
    //*length = 1;	// Leave unrecognized entities - this doesn't really work.
    // Just writes strange XML.
    return p + 1;
}

bool TiXmlBase::StringEqual(const char* p, const char* tag, bool ignoreCase) {
    assert(p);
    assert(tag);
    if (!p || !*p) {
        assert(0);
        return false;
    }

    const char* q = p;

    if (ignoreCase) {
        while (*q && *tag && ToLower(*q) == ToLower(*tag)) {
            ++q;
            ++tag;
        }

        if (*tag == 0) return true;
    } else {
        while (*q && *tag && *q == *tag) {
            ++q;
            ++tag;
        }

        if (*tag == 0)  // Have we found the end of the tag, and everything equal?
            return true;
    }
    return false;
}

const char* TiXmlBase::ReadText(const char* p, std::string* text, bool trimWhiteSpace,
                                const char* endTag, bool caseInsensitive) {
    *text = "";
    if (!trimWhiteSpace          // certain tags always keep whitespace
        || !condenseWhiteSpace)  // if true, whitespace is always kept
    {
        // Keep all the white space.
        while (p && *p && !StringEqual(p, endTag, caseInsensitive)) {
            int len;
            char cArr[4] = {0, 0, 0, 0};
            p = GetChar(p, cArr, &len);
            text->append(cArr, len);
        }
    } else {
        bool whitespace = false;

        // Remove leading white space:
        p = SkipWhiteSpace(p);
        while (p && *p && !StringEqual(p, endTag, caseInsensitive)) {
            if (*p == '\r' || *p == '\n') {
                whitespace = true;
                ++p;
            } else if (IsWhiteSpace(*p)) {
                whitespace = true;
                ++p;
            } else {
                // If we've found whitespace, add it before the
                // new character. Any whitespace just becomes a space.
                if (whitespace) {
                    (*text) += ' ';
                    whitespace = false;
                }
                int len;
                char cArr[4] = {0, 0, 0, 0};
                p = GetChar(p, cArr, &len);
                if (len == 1)
                    (*text) += cArr[0];  // more efficient
                else
                    text->append(cArr, len);
            }
        }
    }
    if (p) p += strlen(endTag);
    return p;
}
