/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2013-2024 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <modules/fontrendering/fontrenderingmodule.h>

#include <inviwo/core/common/inviwomodule.h>                          // for InviwoModule
#include <inviwo/core/common/modulepath.h>                            // for ModulePath, ModuleP...
#include <inviwo/core/io/serialization/ticpp.h>                       // for TxElement
#include <inviwo/core/io/serialization/versionconverter.h>            // for Kind, createNode
#include <modules/fontrendering/processors/textoverlaygl.h>           // for TextOverlayGL
#include <modules/fontrendering/properties/fontfaceoptionproperty.h>  // for FontFaceOptionProperty
#include <modules/fontrendering/properties/fontproperty.h>            // for FontProperty
// Autogenerated
#include <modules/fontrendering/shader_resources.h>  // for addShaderResources
#include <modules/opengl/shader/shadermanager.h>     // for ShaderManager

#include <functional>        // for __base, function
#include <initializer_list>  // for initializer_list
#include <string>            // for string, char_traits
#include <vector>            // for vector

#include <fmt/core.h>  // for format

namespace inviwo {
class InviwoApplication;

FontRenderingModule::FontRenderingModule(InviwoApplication* app)
    : InviwoModule(app, "FontRendering") {
    fontrendering::addShaderResources(ShaderManager::getPtr(), {getPath(ModulePath::GLSL)});
    registerProcessor<TextOverlayGL>();

    registerProperty<FontProperty>();
    registerProperty<FontFaceOptionProperty>();
}

int FontRenderingModule::getVersion() const { return 5; }

std::unique_ptr<VersionConverter> FontRenderingModule::getConverter(int version) const {
    return std::make_unique<Converter>(version);
}

FontRenderingModule::Converter::Converter(int version) : version_(version) {}

bool FontRenderingModule::Converter::convert(TxElement* root) {

    bool res = false;
    switch (version_) {
        case 0: {
            const std::vector<xml::IdentifierReplacement> repl = {
                // TextOverlayGL
                {{xml::Kind::processor("org.inviwo.TextOverlayGL"),
                  xml::Kind::property("org.inviwo.OptionPropertyInt")},
                 "Font size",
                 "fontSize"}};
            res |= xml::changeIdentifiers(root, repl);
            [[fallthrough]];
        }
        case 1: {
            const std::vector<xml::IdentifierReplacement> repl2 = {
                // TextOverlayGL
                {{xml::Kind::processor("org.inviwo.TextOverlayGL"),
                  xml::Kind::property("org.inviwo.FloatVec2Property")},
                 "Position",
                 "position"},
                {{xml::Kind::processor("org.inviwo.TextOverlayGL"),
                  xml::Kind::property("org.inviwo.FloatVec2Property")},
                 "Anchor",
                 "anchor"},
                {{xml::Kind::processor("org.inviwo.TextOverlayGL"),
                  xml::Kind::property("org.inviwo.FloatVec4Property")},
                 "color_",
                 "color"},
                {{xml::Kind::processor("org.inviwo.TextOverlayGL"),
                  xml::Kind::property("org.inviwo.StringProperty")},
                 "Text",
                 "text"}};
            res |= xml::changeIdentifiers(root, repl2);
            [[fallthrough]];
        }
        case 2: {

            res |= xml::changeTag(root,
                                  {{
                                      xml::Kind::processor("org.inviwo.TextOverlayGL"),
                                      xml::Kind::property("org.inviwo.FontProperty"),
                                      xml::Kind::property("org.inviwo.OptionPropertyInt"),
                                  }},
                                  "selectedIdentifier", "value");

            res |= xml::changeAttributeRecursive(
                root,
                {{xml::Kind::processor("org.inviwo.TextOverlayGL"),
                  xml::Kind::property("org.inviwo.FontProperty"),
                  xml::Kind::property("org.inviwo.OptionPropertyInt")}},
                "type", "org.inviwo.OptionPropertyInt", "org.inviwo.IntProperty");

            res |= xml::changeAttributeRecursive(
                root, {{xml::Kind::propertyLinkSource("org.inviwo.OptionPropertyInt", "fontSize")}},
                "type", "org.inviwo.OptionPropertyInt", "org.inviwo.IntProperty");
            res |= xml::changeAttributeRecursive(
                root,
                {{xml::Kind::propertyLinkDestination("org.inviwo.OptionPropertyInt", "fontSize")}},
                "type", "org.inviwo.OptionPropertyInt", "org.inviwo.IntProperty");

            [[fallthrough]];
        }
        case 3: {
            // TextOverlayGL restructure
            xml::visitMatchingNodesRecursive(
                root, xml::ElementMatcher{"Processor", {{"type", "org.inviwo.TextOverlayGL"}}},
                [&](TxElement* node) {
                    // Move color into font properties
                    if (auto color =
                            xml::getElement(node, "Properties/Property&identifier=color")) {
                        if (auto fontProps = xml::getElement(
                                node, "Properties/Property&identifier=font/Properties")) {
                            fontProps->InsertEndChild(*color);
                        } else {
                            auto props = xml::getElement(node, "Properties");
                            if (!props) {
                                props = xml::createNode("Properties", node);
                            }
                            fontProps = xml::createNode(
                                "Property&type=org.inviwo.FontProperty&identifier=font/Properties",
                                props);
                            fontProps->InsertEndChild(*color);
                        }
                    }

                    // Move only text/pos/offset into new ListProperty
                    auto props = xml::getElement(node, "Properties");
                    auto texts = xml::createNode(
                        "Property&type=org.inviwo.ListProperty&identifier=texts", props);
                    xml::createNode("OwnedPropertyIdentifiers/PropertyIdentifier&content=text0",
                                    texts);

                    auto textsProps = xml::createNode("Properties", texts);
                    auto overlayProps = xml::createNode(
                        "Property&type=org.inviwo.TextOverlayProperty&identifier=text0/Properties",
                        textsProps);
                    for (auto&& i : {"text", "position", "offset"}) {
                        if (auto p = xml::getElement(
                                node, fmt::format("Properties/Property&identifier={}", i))) {
                            overlayProps->InsertEndChild(*p);
                        }
                    }
                    res = true;
                });
            [[fallthrough]];
        }
        case 4: {
            TraversingVersionConverter conv{[&](TxElement* node) -> bool {
                auto key = node->Value();
                if (key != "Property") return true;
                const auto& type = node->GetAttribute("type");
                if (type != "org.inviwo.FontProperty") return true;

                if (auto elem = xml::getElement(
                        node, "Properties/Property&type=org.inviwo.OptionPropertyString")) {
                    elem->SetAttribute("type", "org.inviwo.FontFaceOptionProperty");
                    res = true;
                }

                return true;
            }};
            conv.convert(root);

            return res;
        }
        default:
            return false;  // No changes
    }
}

}  // namespace inviwo
