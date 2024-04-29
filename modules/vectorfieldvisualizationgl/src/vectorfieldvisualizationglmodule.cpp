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

#include <modules/vectorfieldvisualizationgl/vectorfieldvisualizationglmodule.h>

#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/core/io/serialization/ticpp.h>
#include <inviwo/core/io/serialization/versionconverter.h>
#include <modules/opengl/shader/shadermanager.h>
#include <modules/vectorfieldvisualizationgl/processors/2d/hedgehog2d.h>
#include <modules/vectorfieldvisualizationgl/processors/2d/lic2d.h>
#include <modules/vectorfieldvisualizationgl/processors/2d/vector2dcurl.h>
#include <modules/vectorfieldvisualizationgl/processors/2d/vector2ddivergence.h>
#include <modules/vectorfieldvisualizationgl/processors/2d/vector2dmagnitude.h>
#include <modules/vectorfieldvisualizationgl/processors/3d/lic3d.h>
#include <modules/vectorfieldvisualizationgl/processors/3d/streamparticles.h>
#include <modules/vectorfieldvisualizationgl/processors/3d/vector3dcurl.h>
#include <modules/vectorfieldvisualizationgl/processors/3d/vector3ddivergence.h>
#include <modules/vectorfieldvisualizationgl/processors/4d/tmip.h>
#include <modules/vectorfieldvisualizationgl/processors/datageneration/lorenzsystem.h>
#include <modules/vectorfieldvisualizationgl/processors/datageneration/vectorfieldgenerator2d.h>
#include <modules/vectorfieldvisualizationgl/processors/datageneration/vectorfieldgenerator3d.h>
#include <modules/vectorfieldvisualizationgl/processors/datageneration/vectorfieldgenerator4d.h>
// Autogenerated
#include <modules/vectorfieldvisualizationgl/shader_resources.h>

#include <string>
#include <utility>
#include <vector>

namespace inviwo {

VectorFieldVisualizationGLModule::VectorFieldVisualizationGLModule(InviwoApplication* app)
    : InviwoModule(app, "VectorFieldVisualizationGL") {
    vectorfieldvisualizationgl::addShaderResources(ShaderManager::getPtr(),
                                                   {getPath(ModulePath::GLSL)});

    registerProcessor<LorenzSystem>();
    registerProcessor<VectorFieldGenerator2D>();
    registerProcessor<VectorFieldGenerator3D>();
    registerProcessor<LIC2D>();
    registerProcessor<HedgeHog2D>();

    registerProcessor<Vector2DMagnitude>();
    registerProcessor<Vector2DCurl>();
    registerProcessor<Vector2DDivergence>();

    registerProcessor<LIC3D>();
    registerProcessor<Vector3DCurl>();
    registerProcessor<Vector3DDivergence>();
    registerProcessor<TMIP>();
    registerProcessor<VectorFieldGenerator4D>();

    registerProcessor<StreamParticles>();
}

int VectorFieldVisualizationGLModule::getVersion() const { return 1; }

std::unique_ptr<VersionConverter> VectorFieldVisualizationGLModule::getConverter(
    int version) const {
    return std::make_unique<Converter>(version);
}

VectorFieldVisualizationGLModule::Converter::Converter(int version) : version_(version) {}

bool VectorFieldVisualizationGLModule::Converter::convert(TxElement* root) {
    std::vector<xml::IdentifierReplacement> repl = {};

    const std::vector<std::pair<std::string, std::string>> volumeGLrepl = {
        {"Vector3DCurl", "vector3dcurl.frag"}, {"Vector3DDivergence", "vector3ddivergence.frag"}};
    for (const auto& i : volumeGLrepl) {
        xml::IdentifierReplacement inport = {{xml::Kind::processor("org.inviwo." + i.first),
                                              xml::Kind::inport("org.inviwo.VolumeInport")},
                                             i.second + "inport",
                                             "inputVolume"};
        xml::IdentifierReplacement outport = {{xml::Kind::processor("org.inviwo." + i.first),
                                               xml::Kind::outport("org.inviwo.VolumeOutport")},
                                              i.second + "outport",
                                              "outputVolume"};
        repl.push_back(inport);
        repl.push_back(outport);
    }

    bool res = false;
    switch (version_) {
        case 0: {
            res |= xml::changeIdentifiers(root, repl);
            return res;
        }

        default:
            return false;  // No changes
    }
    return true;
}

}  // namespace inviwo
