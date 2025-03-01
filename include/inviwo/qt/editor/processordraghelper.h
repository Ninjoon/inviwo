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

#include <inviwo/qt/editor/networkautomation.h>

#include <warn/push>
#include <warn/ignore/all>
#include <QObject>
#include <warn/pop>

#include <unordered_map>
#include <memory>

class QEvent;
class QGraphicsSceneDragDropEvent;

namespace inviwo {

class Processor;
class ProcessorMimeData;
class ConnectionGraphicsItem;
class NetworkEditor;
class AutoLinker;
class LinkConnectionDragGraphicsItem;
class ConnectionOutportDragGraphicsItem;
class ProcessorGraphicsItem;
class Property;
class Inport;
class Outport;

class IVW_QTEDITOR_API ProcessorDragHelper : public QObject {
    Q_OBJECT

public:
    explicit ProcessorDragHelper(NetworkEditor& editor);
    virtual ~ProcessorDragHelper();

    virtual bool eventFilter(QObject* obj, QEvent* event) override;

    void clear(ConnectionGraphicsItem* connection);
    void clear(ProcessorGraphicsItem* processor);

    bool enter(QGraphicsSceneDragDropEvent* e, const ProcessorMimeData* mime);
    bool move(QGraphicsSceneDragDropEvent* e, const ProcessorMimeData* mime);
    bool leave(QGraphicsSceneDragDropEvent* e);
    bool drop(QGraphicsSceneDragDropEvent* e, const ProcessorMimeData* mime);

private:
    NetworkEditor& editor_;

    NetworkAutomation automator_;
};

}  // namespace inviwo
