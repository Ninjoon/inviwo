/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2015-2024 Inviwo Foundation
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

#include <modules/vectorfieldvisualizationgl/processors/2d/vector2ddivergence.h>

#include <inviwo/core/util/formats.h>
#include <modules/opengl/shader/shaderutils.h>

#include <glm/gtx/component_wise.hpp>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo Vector2DDivergence::processorInfo_{
    "org.inviwo.Vector2DDivergence",  // Class identifier
    "Vector 2D Divergence",           // Display name
    "Vector Field Visualization",     // Category
    CodeState::Stable,                // Code state
    Tags::GL | Tag{"Layer"},          // Tags
    R"(Computes the divergence of a 2D vector field.)"_unindentHelp,
};
const ProcessorInfo& Vector2DDivergence::getProcessorInfo() const { return processorInfo_; }

Vector2DDivergence::Vector2DDivergence()
    : LayerGLProcessor{utilgl::findShaderResource("vector2ddivergence.frag")} {}

LayerConfig Vector2DDivergence::outputConfig(const Layer& input) const {
    const double max = glm::compMax(glm::abs(input.dataMap.dataRange));
    const double gradientEstimate = max / glm::compMax(input.getWorldSpaceGradientSpacing());
    return input.config().updateFrom({.format = DataFloat32::get(),
                                      .swizzleMask = swizzlemasks::defaultData(0),
                                      .dataRange = dvec2{0.0, 3.0 * gradientEstimate},
                                      .valueRange = dvec2{0.0, 3.0 * gradientEstimate}});
}

}  // namespace inviwo
