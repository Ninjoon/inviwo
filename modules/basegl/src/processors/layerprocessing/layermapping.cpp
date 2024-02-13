/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024 Inviwo Foundation
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

#include <modules/basegl/processors/layerprocessing/layermapping.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/texture/textureutils.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo LayerMapping::processorInfo_{"org.inviwo.LayerMapping",  // Class identifier
                                                 "Layer Mapping",            // Display name
                                                 "Layer Operation",          // Category
                                                 CodeState::Stable,          // Code state
                                                 Tags::GL,                   // Tags
                                                 R"(
Maps the input image to an output image with the help of a transfer function.
)"_unindentHelp};

const ProcessorInfo LayerMapping::getProcessorInfo() const { return processorInfo_; }

LayerMapping::LayerMapping()
    : LayerGLProcessor{utilgl::findShaderResource("img_mapping.frag")}
    , channel_{"channel", "Channel", "Selected channel used for mapping"_help,
               util::enumeratedOptions("Channel", 4)}

    , transferFunction_(
          "transferFunction", "Transfer Function",
          "The transfer function used for mapping input to output values including the "
          "alpha channel."_help) {
    addProperties(channel_, transferFunction_);
}

void LayerMapping::preProcess(TextureUnitContainer& container, const Layer& input, Layer& output) {
    utilgl::bindAndSetUniforms(shader_, container, transferFunction_);
    utilgl::setUniforms(shader_, channel_);
}

LayerConfig LayerMapping::outputConfig([[maybe_unused]] const Layer& input) const {
    auto inputFormat = input.getDataFormat();
    auto outputFormat =
        DataFormatBase::get(inputFormat->getNumericType(), 4, inputFormat->getPrecision());
    return input.config().updateFrom({.format = outputFormat});
}

}  // namespace inviwo
