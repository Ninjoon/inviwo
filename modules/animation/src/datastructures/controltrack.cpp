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

#include <modules/animation/datastructures/controltrack.h>

#include <inviwo/core/util/indirectiterator.h>                         // for IndirectIterator
#include <modules/animation/datastructures/animationstate.h>           // for AnimationState
#include <modules/animation/datastructures/animationtime.h>            // for Seconds
#include <modules/animation/datastructures/basetrack.h>                // for BaseTrack<>::key_type
#include <modules/animation/datastructures/controlkeyframe.h>          // for ControlKeyframe
#include <modules/animation/datastructures/controlkeyframesequence.h>  // for ControlKeyframeSeq...
#include <modules/animation/datastructures/keyframesequence.h>         // for operator<

#include <algorithm>  // for upper_bound
#include <chrono>     // for operator<, operator>
#include <iterator>   // for prev

namespace inviwo {

namespace animation {

ControlTrack::ControlTrack() : BaseTrack<ControlKeyframeSequence>{"Control Track", 0} {}

ControlTrack::~ControlTrack() = default;

ControlTrack* ControlTrack::clone() const { return new ControlTrack(*this); }

std::string_view ControlTrack::classIdentifier() { return "org.inviwo.animation.ControlTrack"; }
std::string_view ControlTrack::getClassIdentifier() const { return classIdentifier(); }

/**
 * Track of sequences
 * ----------X======X====X-----------X=========X-------X=====X--------
 * |- case 1-|-case 2----------------|-case 2----------|-case 2------|
 *           |-case 2a---|-case 2b---|
 */
AnimationTimeState ControlTrack::operator()(Seconds from, Seconds to, AnimationState state) const {
    if (!isEnabled() || empty()) return {to, state};

    // 'it' will be the first seq. with a first time larger than 'to'.
    auto it =
        std::upper_bound(begin(), end(), to, [](const auto& a, const auto& b) { return a < b; });

    if (it == begin()) {
        if (from > it->getFirstTime()) {  // case 1
            return (*it)(from, to, state);
        }
    } else {  // case 2
        auto& seq1 = *std::prev(it);

        if (to < seq1.getLastTime()) {  // case 2a
            return seq1(from, to, state);
        } else {  // case 2b
            if (from < seq1.getLastTime()) {
                // We came from before the previous key
                return seq1(from, to, state);
            } else if (it != end() && from > it->getFirstTime()) {
                // We came form after the next key
                return (*it)(from, to, state);
            }
            // we moved in an unmarked region, do nothing.
        }
    }
    return {to, state};
}

}  // namespace animation

}  // namespace inviwo
