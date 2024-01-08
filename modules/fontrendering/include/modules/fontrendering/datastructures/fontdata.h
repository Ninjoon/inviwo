/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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
#pragma once

#include <modules/fontrendering/fontrenderingmoduledefine.h>

#include <inviwo/core/util/glmvec.h>
#include <modules/fontrendering/datastructures/fontsettings.h>
#include <modules/fontrendering/util/fontutils.h>

#include <string>

namespace inviwo {

class IVW_MODULE_FONTRENDERING_API FontData : public FontSettings {
public:
    FontData() = default;
    FontData(const FontSettings& s);
    FontData(const FontData&) = default;
    FontData(FontData&&) noexcept = default;
    FontData& operator=(const FontData&) = default;
    FontData& operator=(FontData&&) noexcept = default;
    virtual ~FontData() = default;

    // Inherited via FontSettings
    virtual const std::filesystem::path& getFontFace() const override;
    virtual int getFontSize() const override;
    virtual float getLineSpacing() const override;
    virtual vec2 getAnchorPos() const override;

    std::filesystem::path fontFace = font::getFont(font::FontType::Default);
    int fontSize = 14;
    float lineSpacing = 0.0f;
    vec2 anchorPos = vec2(-1.0f);
};

}  // namespace inviwo
