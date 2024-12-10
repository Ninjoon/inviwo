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

#pragma once

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/datastructures/camera/camera.h>

#include <memory>
#include <string>
#include <string_view>

namespace inviwo {

class IVW_CORE_API CameraFactoryObject {
public:
    CameraFactoryObject(std::string_view classIdentifier);
    virtual ~CameraFactoryObject() = default;

    std::unique_ptr<Camera> create(vec3 lookFrom = cameradefaults::lookFrom,
                                   vec3 lookTo = cameradefaults::lookTo,
                                   vec3 lookUp = cameradefaults::lookUp,
                                   float nearPlane = cameradefaults::nearPlane,
                                   float farPlane = cameradefaults::farPlane,
                                   float aspectRatio = cameradefaults::aspectRatio) const {
        return createImpl(lookFrom, lookTo, lookUp, nearPlane, farPlane, aspectRatio);
    }
    std::string_view getClassIdentifier() const;

protected:
    virtual std::unique_ptr<Camera> createImpl(vec3 lookFrom, vec3 lookTo, vec3 lookUp,
                                               float nearPlane, float farPlane,
                                               float aspectRatio) const = 0;

private:
    std::string classIdentifier_;
};
template <typename T>
class CameraFactoryObjectTemplate : public CameraFactoryObject {
public:
    CameraFactoryObjectTemplate(std::string_view classIdentifier)
        : CameraFactoryObject(classIdentifier) {}

    virtual std::unique_ptr<Camera> createImpl(vec3 lookFrom, vec3 lookTo, vec3 lookUp,
                                               float nearPlane, float farPlane,
                                               float aspectRatio) const override {
        return std::make_unique<T>(lookFrom, lookTo, lookUp, nearPlane, farPlane, aspectRatio);
    }
};

}  // namespace inviwo
