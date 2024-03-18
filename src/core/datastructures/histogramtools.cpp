/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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

#include <inviwo/core/datastructures/histogramtools.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/zip.h>

namespace inviwo {

HistogramCache::HistogramCache() : state_{std::make_shared<State>()} {}
HistogramCache::HistogramCache(const HistogramCache& rhs) : state_{std::make_shared<State>()} {
    std::scoped_lock lock{rhs.state_->mutex};
    state_->histograms = rhs.state_->histograms;
    state_->callbacks = rhs.state_->callbacks;
    state_->status = rhs.state_->status;
}
HistogramCache::HistogramCache(HistogramCache&& rhs) noexcept : state_{std::move(rhs.state_)} {}
HistogramCache& HistogramCache::operator=(const HistogramCache& that) {
    if (this != &that) {
        state_ = std::make_shared<State>();
        std::scoped_lock lock{that.state_->mutex};
        state_->histograms = that.state_->histograms;
        state_->callbacks = that.state_->callbacks;
        state_->status = that.state_->status;
    }
    return *this;
}
HistogramCache& HistogramCache::operator=(HistogramCache&& that) noexcept {
    if (this != &that) {
        state_ = std::move(that.state_);
    }
    return *this;
}

DispatcherHandle<HistogramCache::Callback> HistogramCache::calculateHistograms(
    std::function<std::vector<Histogram1D>()> calculate,
    std::function<void(const std::vector<Histogram1D>&)> whenDone) const {
    std::scoped_lock lock{state_->mutex};

    DispatcherHandle<HistogramCache::Callback> handle = nullptr;

    if (state_->status == Status::Valid && whenDone) {
        whenDone(state_->histograms);
    } else if (state_->status != Status::Valid && whenDone) {
        handle = state_->callbacks.add(whenDone);
    }

    if (state_->status == Status::NotSet) {
        if (whenDone) {
            handle = state_->callbacks.add(whenDone);
        }
        state_->status = Status::Calculating;
        dispatchPool([calculate, weakState = std::weak_ptr<State>(state_)]() {
            if (auto state = weakState.lock()) {
                auto newHistograms = calculate();
                dispatchFrontAndForget([weakState = std::weak_ptr<State>(state),
                                        newHistograms = std::move(newHistograms)]() {
                    if (auto state = weakState.lock()) {
                        std::scoped_lock lock{state->mutex};
                        state->histograms = std::move(newHistograms);
                        state->status = Status::Valid;
                        state->callbacks.invoke(state->histograms);
                    }
                });
            }
        });
    }

    return handle;
}

void HistogramCache::forEach(
    const std::function<void(const Histogram1D&, size_t)>& callback) const {
    std::scoped_lock lock{state_->mutex};
    for (auto&& [channel, histogram] : util::enumerate(state_->histograms)) {
        callback(histogram, channel);
    }
}

void HistogramCache::discard(std::function<std::vector<Histogram1D>()> calculate) {
    bool reCalculate = false;
    std::shared_ptr<State> newState;
    {
        std::scoped_lock lock{state_->mutex};

        if (state_->status == Status::NotSet) {
            return;
        } else if (state_->status == Status::Valid) {
            state_->status = Status::NotSet;
            reCalculate = true;
        } else if (state_->status == Status::Calculating) {
            newState = std::make_shared<State>();
            newState->callbacks = std::move(state_->callbacks);
            reCalculate = true;
        }
    }
    if (newState) {
        state_ = std::move(newState);
    }
    if (reCalculate) {
        calculateHistograms(calculate, nullptr);
    }
}

}  // namespace inviwo
