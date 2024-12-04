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
#pragma once

#include <inviwo/qt/editor/inviwoqteditordefine.h>
#include <inviwo/core/util/glmvec.h>

#include <warn/push>
#include <warn/ignore/all>
#include <QObject>
#include <QPointF>
#include <warn/pop>

#include <memory>

class QEvent;

namespace inviwo {

class NetworkEditor;
class ConnectionOutportDragGraphicsItem;
class ConnectionInportDragGraphicsItem;
class ProcessorOutportGraphicsItem;
class ProcessorInportGraphicsItem;

class IVW_QTEDITOR_API ConnectionOutDragHelper : public QObject {
    Q_OBJECT

public:
    explicit ConnectionOutDragHelper(NetworkEditor& editor);
    virtual ~ConnectionOutDragHelper();

    void start(ProcessorOutportGraphicsItem* outport, QPointF endPoint, uvec3 color);
    void reset();

    virtual bool eventFilter(QObject* obj, QEvent* event) override;

private:
    NetworkEditor& editor_;
    std::unique_ptr<ConnectionOutportDragGraphicsItem> outConnection_;
};

class IVW_QTEDITOR_API ConnectionInDragHelper : public QObject {
    Q_OBJECT

public:
    explicit ConnectionInDragHelper(NetworkEditor& editor);
    virtual ~ConnectionInDragHelper();

    void start(ProcessorInportGraphicsItem* inport, QPointF endPoint, uvec3 color);
    void reset();

    virtual bool eventFilter(QObject* obj, QEvent* event) override;

private:
    NetworkEditor& editor_;
    std::unique_ptr<ConnectionInportDragGraphicsItem> inConnection_;
};

}  // namespace inviwo
