/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2024 Inviwo Foundation
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

#include <inviwo/core/properties/isotfproperty.h>

namespace inviwo {

std::string_view IsoTFProperty::getClassIdentifier() const { return classIdentifier; }

IsoTFProperty::IsoTFProperty(std::string_view identifier, std::string_view displayName,
                             Document help, const IsoValueCollection& isovalues,
                             const TransferFunction& tf, TFData port,
                             InvalidationLevel invalidationLevel, PropertySemantics semantics)
    : CompositeProperty{identifier, displayName, std::move(help), invalidationLevel,
                        std::move(semantics)}
    , isovalues_{"isovalues", "Iso Values", isovalues, port}
    , tf_{"transferFunction", "Transfer Function", tf, std::move(port)} {

    addProperties(isovalues_, tf_);

    tf_.TFPropertyObservable::addObserver(this);
    isovalues_.TFPropertyObservable::addObserver(this);
}

IsoTFProperty::IsoTFProperty(std::string_view identifier, std::string_view displayName,
                             const IsoValueCollection& isovalues, const TransferFunction& tf,
                             TFData port, InvalidationLevel invalidationLevel,
                             PropertySemantics semantics)
    : CompositeProperty{identifier, displayName, invalidationLevel, std::move(semantics)}
    , isovalues_{"isovalues", "Iso Values", isovalues, port}
    , tf_{"transferFunction", "Transfer Function", tf, std::move(port)} {

    addProperties(isovalues_, tf_);

    tf_.TFPropertyObservable::addObserver(this);
    isovalues_.TFPropertyObservable::addObserver(this);
}

IsoTFProperty::IsoTFProperty(std::string_view identifier, std::string_view displayName,
                             Document help, TFData port, InvalidationLevel invalidationLevel,
                             PropertySemantics semantics)
    : IsoTFProperty{identifier,
                    displayName,
                    std::move(help),
                    {},
                    TransferFunction({{0.0, vec4(0.0f)}, {1.0, vec4(1.0f)}}),
                    std::move(port),
                    invalidationLevel,
                    std::move(semantics)} {}

IsoTFProperty::IsoTFProperty(std::string_view identifier, std::string_view displayName, TFData port,
                             InvalidationLevel invalidationLevel, PropertySemantics semantics)
    : IsoTFProperty{identifier,
                    displayName,
                    {},
                    TransferFunction({{0.0, vec4(0.0f)}, {1.0, vec4(1.0f)}}),
                    std::move(port),
                    invalidationLevel,
                    std::move(semantics)} {}

IsoTFProperty::IsoTFProperty(const IsoTFProperty& rhs)
    : CompositeProperty{rhs}, isovalues_{rhs.isovalues_}, tf_{rhs.tf_} {

    addProperties(isovalues_, tf_);

    tf_.TFPropertyObservable::addObserver(this);
    isovalues_.TFPropertyObservable::addObserver(this);
}

IsoTFProperty* IsoTFProperty::clone() const { return new IsoTFProperty(*this); }

std::string_view IsoTFProperty::getClassIdentifierForWidget() const {
    return IsoTFProperty::classIdentifier;
}

void IsoTFProperty::set(const Property* property) {
    if (const auto* isoTF = dynamic_cast<const CompositeProperty*>(property)) {
        CompositeProperty::set(isoTF);
    } else if (const auto* iso = dynamic_cast<const IsoValueProperty*>(property)) {
        isovalues_.set(iso);
    } else if (const auto* tf = dynamic_cast<const TransferFunctionProperty*>(property)) {
        tf_.set(tf);
    }
}

void IsoTFProperty::set(const IsoValueProperty* p) { isovalues_.set(p); }

void IsoTFProperty::set(const TransferFunctionProperty* p) { tf_.set(p); }

void IsoTFProperty::setMask(double maskMin, double maskMax) { tf_.setMask(maskMin, maskMax); }

dvec2 IsoTFProperty::getMask() const { return tf_.getMask(); }

void IsoTFProperty::clearMask() { tf_.clearMask(); }

void IsoTFProperty::setZoomH(double zoomHMin, double zoomHMax) {
    tf_.setZoomH(zoomHMin, zoomHMax);
    isovalues_.setZoomH(zoomHMin, zoomHMax);
}

const dvec2& IsoTFProperty::getZoomH() const { return tf_.getZoomH(); }

void IsoTFProperty::setZoomV(double zoomVMin, double zoomVMax) {
    tf_.setZoomV(zoomVMin, zoomVMax);
    isovalues_.setZoomV(zoomVMin, zoomVMax);
}

const dvec2& IsoTFProperty::getZoomV() const { return tf_.getZoomV(); }

IsoTFProperty& IsoTFProperty::setHistogramMode(HistogramMode type) {
    tf_.setHistogramMode(type);
    isovalues_.setHistogramMode(type);
    return *this;
}

HistogramMode IsoTFProperty::getHistogramMode() const { return tf_.getHistogramMode(); }

IsoTFProperty& IsoTFProperty::setHistogramSelection(HistogramSelection selection) {
    tf_.setHistogramSelection(selection);
    isovalues_.setHistogramSelection(selection);
    return *this;
}

HistogramSelection IsoTFProperty::getHistogramSelection() const {
    return tf_.getHistogramSelection();
}

void IsoTFProperty::onZoomHChange(const dvec2& zoomH) { notifyZoomHChange(zoomH); }

void IsoTFProperty::onZoomVChange(const dvec2& zoomV) { notifyZoomVChange(zoomV); }

void IsoTFProperty::onHistogramModeChange(HistogramMode mode) { notifyHistogramModeChange(mode); }

void IsoTFProperty::onHistogramSelectionChange(HistogramSelection selection) {
    notifyHistogramSelectionChange(selection);
}

}  // namespace inviwo
