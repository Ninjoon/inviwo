/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2013-2018 Inviwo Foundation
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

#ifndef IVW_INTERACTIONEVENT_H
#define IVW_INTERACTIONEVENT_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/io/serialization/serializable.h>
#include <inviwo/core/interaction/events/event.h>
#include <inviwo/core/interaction/events/keyboardkeys.h>

#include <functional>

namespace inviwo {

class IVW_CORE_API InteractionEvent : public Event {
public:
    InteractionEvent(KeyModifiers modifiers = KeyModifiers(flags::empty));
    InteractionEvent(const InteractionEvent& rhs) = default;
    InteractionEvent& operator=(const InteractionEvent& that) = default;
    virtual InteractionEvent* clone() const override = 0;
    virtual ~InteractionEvent() = default;

    KeyModifiers modifiers() const;
    void setModifiers(KeyModifiers modifiers);
    std::string modifierNames() const;
    

    using ToolTipCallback = std::function<void(const std::string&)>;
    void setToolTipCallback(ToolTipCallback callback);
    const ToolTipCallback& getToolTipCallback() const;

protected:
    KeyModifiers modifiers_;
    ToolTipCallback tooltip_;
};

}  // namespace

#endif // IVW_INTERACTIONEVENT_H