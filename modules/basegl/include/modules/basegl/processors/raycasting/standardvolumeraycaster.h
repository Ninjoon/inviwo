/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021-2024 Inviwo Foundation
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

#include <modules/basegl/baseglmoduledefine.h>  // for IVW_MODULE_BASEG...

#include <inviwo/core/datastructures/representationconverter.h>          // for RepresentationCo...
#include <inviwo/core/datastructures/representationconverterfactory.h>   // for RepresentationCo...
#include <inviwo/core/processors/processorinfo.h>                        // for ProcessorInfo
#include <inviwo/core/util/zip.h>                                        // for zipper
#include <modules/basegl/processors/raycasting/volumeraycasterbase.h>    // for VolumeRaycasterBase
#include <modules/basegl/shadercomponents/backgroundcomponent.h>         // for BackgroundComponent
#include <modules/basegl/shadercomponents/cameracomponent.h>             // for CameraComponent
#include <modules/basegl/shadercomponents/entryexitcomponent.h>          // for EntryExitComponent
#include <modules/basegl/shadercomponents/isotfcomponent.h>              // for IsoTFComponent
#include <modules/basegl/shadercomponents/lightcomponent.h>              // for LightComponent
#include <modules/basegl/shadercomponents/positionindicatorcomponent.h>  // for PositionIndicato...
#include <modules/basegl/shadercomponents/raycastingcomponent.h>         // for RaycastingComponent
#include <modules/basegl/shadercomponents/sampletransformcomponent.h>    // for SampleTransformC...
#include <modules/basegl/shadercomponents/volumecomponent.h>             // for VolumeComponent

#include <array>          // for array
#include <memory>         // for unique_ptr
#include <string_view>    // for string_view
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include <fmt/core.h>  // for format, basic_st...

namespace inviwo {

/** \docpage{org.inviwo.StandardVolumeRaycaster, Standard Volume Raycaster}
 * ![](org.inviwo.StandardVolumeRaycaster.png?classIdentifier=org.inviwo.StandardVolumeRaycaster)
 *
 * Processor for visualizing volumetric data by means of volume raycasting. Only one channel of the
 * volume will be used. Besides the volume data, entry and exit point locations of the bounding box
 * are required. These can be created with the EntryExitPoints processor. The camera properties
 * between these two processors need to be linked.
 *
 * ### Inports
 *   * __volume__ input volume (Only one channel will be rendered)
 *   * __entry__  entry point locations of input volume (image generated by EntryExitPoints
 *                processor)
 *   * __exit__   exit point positions of input volume (image generated by EntryExitPoints
 *                processor)
 *   * __bg__     optional background image. The depth channel is used to terminate the raycasting.
 *
 * ### Outports
 *   * __outport__ output image containing volume rendering of the input
 *
 */
class IVW_MODULE_BASEGL_API StandardVolumeRaycaster : public VolumeRaycasterBase {
public:
    StandardVolumeRaycaster(std::string_view identifier = "", std::string_view displayName = "");
    virtual ~StandardVolumeRaycaster() = default;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    VolumeComponent volume_;
    EntryExitComponent entryExit_;
    BackgroundComponent background_;
    IsoTFComponent<1> isoTF_;
    RaycastingComponent raycasting_;
    CameraComponent camera_;
    LightComponent light_;
    PositionIndicatorComponent positionIndicator_;
    SampleTransformComponent sampleTransform_;
};

}  // namespace inviwo
