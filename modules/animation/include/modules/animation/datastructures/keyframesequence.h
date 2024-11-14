/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2024 Inviwo Foundation
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

#include <modules/animation/animationmoduledefine.h>  // for IVW_MODULE_ANIMAT...

#include <inviwo/core/io/serialization/serializable.h>                  // for Serializable
#include <modules/animation/datastructures/animationtime.h>             // for Seconds
#include <modules/animation/datastructures/keyframesequenceobserver.h>  // for KeyframeSequenceO...

#include <cstddef>  // for size_t
#include <memory>   // for unique_ptr
#include <utility>  // for pair

namespace inviwo {
class Deserializer;
class Serializer;

namespace animation {
class Keyframe;

/** \class KeyframeSequence
 * Interface for a sequence of keyframes, which will be evaluated during an animation.
 * The KeyframeSequence is a part of a Track and owns Keyframes.
 * All keyframes in the sequence are interpolated using the same Interpolation method.
 * @note Interpolation will only be performed if more than two key frames exist.
 * @see KeyFrame
 * @see Interpolation
 */
class IVW_MODULE_ANIMATION_API KeyframeSequence : public Serializable,
                                                  public KeyframeSequenceObserverble {

public:
    virtual KeyframeSequence* clone() const = 0;

    /**
     * Return number of keyframes in the sequence.
     */
    virtual size_t size() const = 0;

    virtual const Keyframe& operator[](size_t i) const = 0;
    virtual Keyframe& operator[](size_t i) = 0;

    virtual const Keyframe& getFirst() const = 0;
    virtual Keyframe& getFirst() = 0;
    virtual const Keyframe& getLast() const = 0;
    virtual Keyframe& getLast() = 0;

    Seconds getFirstTime() const;
    Seconds getLastTime() const;
    /*
     * Return time of previous keyframe, or false if not found.
     */
    virtual std::optional<Seconds> getPrevTime(Seconds at) const = 0;
    /*
     * Return time of next keyframe, or false if not found.
     */
    virtual std::optional<Seconds> getNextTime(Seconds at) const = 0;
    std::pair<Seconds, Seconds> getTimeSpan() const;

    /**
     * Add Keyframe and call KeyframeObserver::notifyKeyframeAdded
     */
    virtual Keyframe* add(std::unique_ptr<Keyframe> key) = 0;

    /**
     * Remove Keyframe and call KeyframeObserver::notifyKeyframeRemoved
     */
    virtual std::unique_ptr<Keyframe> remove(size_t i) = 0;
    virtual std::unique_ptr<Keyframe> remove(Keyframe* key) = 0;

    virtual bool isSelected() const = 0;
    virtual void setSelected(bool selected) = 0;

    bool isAnyKeyframeSelected() const;

    virtual void serialize(Serializer& s) const override = 0;
    virtual void deserialize(Deserializer& d) override = 0;

    friend bool operator<(const KeyframeSequence& a, const KeyframeSequence& b) {
        return a.getFirstTime() < b.getFirstTime();
    }
    friend bool operator<=(const KeyframeSequence& a, const KeyframeSequence& b) {
        return a.getFirstTime() <= b.getFirstTime();
    }
    friend bool operator>(const KeyframeSequence& a, const KeyframeSequence& b) {
        return a.getFirstTime() > b.getFirstTime();
    }
    friend bool operator>=(const KeyframeSequence& a, const KeyframeSequence& b) {
        return a.getFirstTime() >= b.getFirstTime();
    }

    friend bool operator<(const KeyframeSequence& a, const Seconds& b) {
        return a.getFirstTime() < b;
    }
    friend bool operator<=(const KeyframeSequence& a, const Seconds& b) {
        return a.getFirstTime() <= b;
    }
    friend bool operator>(const KeyframeSequence& a, const Seconds& b) {
        return a.getFirstTime() > b;
    }
    friend bool operator>=(const KeyframeSequence& a, const Seconds& b) {
        return a.getFirstTime() >= b;
    }

    friend bool operator<(const Seconds& a, const KeyframeSequence& b) {
        return a < b.getFirstTime();
    }
    friend bool operator<=(const Seconds& a, const KeyframeSequence& b) {
        return a <= b.getFirstTime();
    }
    friend bool operator>(const Seconds& a, const KeyframeSequence& b) {
        return a > b.getFirstTime();
    }
    friend bool operator>=(const Seconds& a, const KeyframeSequence& b) {
        return a >= b.getFirstTime();
    }
};

}  // namespace animation

}  // namespace inviwo
