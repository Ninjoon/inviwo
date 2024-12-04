/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2024 Inviwo Foundation
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

#include <modules/webbrowser/cefimageconverter.h>

#include <inviwo/core/datastructures/image/imagetypes.h>  // for ImageType, ImageType::ColorPicking
#include <inviwo/core/ports/imageport.h>                  // for BaseImageInport
#include <inviwo/core/util/glmvec.h>                      // for vec3
#include <modules/opengl/inviwoopengl.h>                  // for GL_ONE, GL_ONE_MINUS_SRC_ALPHA
#include <modules/opengl/openglutils.h>                   // for BlendModeState
#include <modules/opengl/shader/shader.h>                 // for Shader
#include <modules/opengl/shader/shaderutils.h>            // for ImageInport
#include <modules/opengl/texture/texture2d.h>             // for Texture2D
#include <modules/opengl/texture/textureunit.h>           // for TextureUnit
#include <modules/opengl/texture/textureutils.h>          // for activateAndClearTarget, activat...
#include <modules/opengl/image/imagegl.h>
#include <modules/opengl/image/layergl.h>

namespace inviwo {

CefImageConverter::CefImageConverter(vec3 pickingColor)
    : shader_{"img_convert_cef.frag", Shader::Build::No}, pickingColor_{pickingColor} {

#if defined(WIN32)
    shader_.getFragmentShaderObject()->addShaderDefine("SwizzleColor");
#endif
    shader_.build();
}

void CefImageConverter::convert(const Texture2D& fromCefOutput, ImageOutport& toInviwoOutput,
                                const ImageInport* background) {

    utilgl::activateTarget(toInviwoOutput, ImageType::ColorDepthPicking);

    shader_.activate();

    TextureUnitContainer units;
    if (background && background->hasData()) {
        if (!shader_.getFragmentShaderObject()->hasShaderDefine("BACKGROUND_AVAILABLE")) {
            shader_.getFragmentShaderObject()->addShaderDefine("BACKGROUND_AVAILABLE");
            shader_.build();
        }

        utilgl::bindAndSetUniforms(shader_, units, *background->getData(), "bg",
                                   ImageType::ColorDepthPicking);

    } else {
        if (shader_.getFragmentShaderObject()->hasShaderDefine("BACKGROUND_AVAILABLE")) {
            shader_.getFragmentShaderObject()->removeShaderDefine("BACKGROUND_AVAILABLE");
            shader_.build();
        }
    }

    utilgl::setShaderUniforms(shader_, toInviwoOutput, "outportParameters");

    // bind input image
    TextureUnit texUnit;
    utilgl::bindTexture(fromCefOutput, texUnit);
    shader_.setUniform("inport", texUnit);
    shader_.setUniform("pickingColor", pickingColor_);

    utilgl::singleDrawImagePlaneRect();
    shader_.deactivate();
    utilgl::deactivateCurrentTarget();
}

}  // namespace inviwo
