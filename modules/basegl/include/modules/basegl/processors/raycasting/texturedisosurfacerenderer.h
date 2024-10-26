/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2022-2024 Inviwo Foundation
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

#include <modules/basegl/baseglmoduledefine.h>

#include <inviwo/core/datastructures/representationconverter.h>         // for RepresentationCo...
#include <inviwo/core/datastructures/representationconverterfactory.h>  // for RepresentationCo...
#include <inviwo/core/processors/processorinfo.h>                       // for ProcessorInfo
#include <inviwo/core/properties/transferfunctionproperty.h>            // for TransferFunctionPr...
#include <inviwo/core/util/zip.h>                                       // for zipper
#include <modules/basegl/processors/raycasting/volumeraycasterbase.h>   // for VolumeRaycasterBase
#include <modules/basegl/shadercomponents/backgroundcomponent.h>        // for BackgroundComponent
#include <modules/basegl/shadercomponents/cameracomponent.h>            // for CameraComponent
#include <modules/basegl/shadercomponents/entryexitcomponent.h>         // for EntryExitComponent
#include <modules/basegl/shadercomponents/tfcomponent.h>
#include <modules/basegl/shadercomponents/isocomponent.h>
#include <modules/basegl/shadercomponents/lightcomponent.h>              // for LightComponent
#include <modules/basegl/shadercomponents/positionindicatorcomponent.h>  // for PositionIndicato...
#include <modules/basegl/shadercomponents/raycastingcomponent.h>         // for RaycastingComponent
#include <modules/basegl/shadercomponents/sampletransformcomponent.h>    // for SampleTransformC...
#include <modules/basegl/shadercomponents/sphericalcomponent.h>          // for SphericalComponent
#include <modules/basegl/shadercomponents/volumecomponent.h>             // for VolumeComponent

#include <array>          // for array
#include <memory>         // for unique_ptr
#include <string_view>    // for string_view
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include <fmt/core.h>  // for format, basic_st...

namespace inviwo {

class IVW_MODULE_BASEGL_API TexturedIsoSurfaceComponent : public ShaderComponent {
public:
    TexturedIsoSurfaceComponent(std::string_view volume);

    virtual std::string_view getName() const override;
    virtual std::vector<std::tuple<Inport*, std::string>> getInports() override;
    virtual std::vector<Segment> getSegments() override;
    virtual std::vector<Property*> getProperties() override;

    virtual void initializeResources(Shader& shader) override;
    virtual void process(Shader& shader, TextureUnitContainer& cont) override;

    std::string volume;
    VolumeInport colorPort;

    IsoValueProperty* iso;
    TransferFunctionProperty* tf;

    OptionPropertyInt isoChannel;
    OptionPropertyInt colorChannel;
    FloatProperty samplingRate;
};

class IVW_MODULE_BASEGL_API TexturedIsosurfaceRenderer : public VolumeRaycasterBase {
public:
    TexturedIsosurfaceRenderer(std::string_view identifier = "", std::string_view displayName = "");
    virtual ~TexturedIsosurfaceRenderer() = default;

    virtual const ProcessorInfo& getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    VolumeComponent volume_;
    TexturedIsoSurfaceComponent texturedComponent_;
    IsoComponent iso_;
    TFComponent tf_;
    EntryExitComponent entryExit_;
    BackgroundComponent background_;

    CameraComponent camera_;
    LightComponent light_;
    PositionIndicatorComponent positionIndicator_;
    SampleTransformComponent sampleTransform_;
};

}  // namespace inviwo
