/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2024 Inviwo Foundation
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

#include <inviwo/qt/editor/processorgraphicsitem.h>

#include <inviwo/core/ports/inport.h>
#include <inviwo/core/ports/outport.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/processors/progressbarowner.h>
#include <inviwo/core/processors/activityindicator.h>
#include <inviwo/core/metadata/processormetadata.h>
#include <inviwo/core/util/clock.h>
#include <inviwo/core/util/chronoutils.h>
#include <inviwo/core/util/document.h>
#include <inviwo/core/util/stdextensions.h>

#include <inviwo/qt/editor/networkeditor.h>
#include <inviwo/qt/editor/connectiongraphicsitem.h>
#include <inviwo/qt/editor/processorlinkgraphicsitem.h>
#include <inviwo/qt/editor/processorportgraphicsitem.h>
#include <inviwo/qt/editor/processorprogressgraphicsitem.h>
#include <inviwo/qt/editor/processorstatusgraphicsitem.h>
#include <inviwo/qt/editor/linkgraphicsitem.h>
#include <modules/qtwidgets/propertylistwidget.h>
#include <modules/qtwidgets/processors/processorwidgetqt.h>
#include <modules/qtwidgets/inviwoqtutils.h>
#include <modules/qtwidgets/qstringhelper.h>

#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QVector2D>
#include <QTextCursor>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>
#include <QPen>

#include <fmt/format.h>

namespace inviwo {

const QSizeF ProcessorGraphicsItem::size_ = {150.0, 50.0};

int pointSizeToPixelSize(const int pointSize) {
    // compute pixel size for fonts by assuming 96 dpi as basis
    return ((pointSize * 4) / 3);
}

ProcessorGraphicsItem::ProcessorGraphicsItem(Processor* processor)
    : ProcessorObserver()
    , LabelGraphicsItemObserver()
    , processor_(processor)
    , processorMeta_{processor->getMetaData<ProcessorMetaData>(ProcessorMetaData::classIdentifier)}
    , animation_{nullptr}
    , progressItem_(nullptr)
    , statusItem_{new ProcessorStatusGraphicsItem(this, processor_)}
    , linkItem_{new ProcessorLinkGraphicsItem(this, size_.width() / 2.0 + 1.0)}
    , highlight_(false)
    , backgroundColor_(
          processor_->getProcessorInfo().codeState == CodeState::Deprecated ? "#562e14" : "#3b3d3d")
    , backgroundJobs_{0}
#if IVW_PROFILING
    , processCount_(0)
    , maxEvalTime_(0.0)
    , evalTime_(0.0)
    , totEvalTime_(0.0)
#endif
    , errorText_{nullptr} {

    static constexpr int labelHeight = 8;
    auto width = static_cast<int>(size_.width());

    setZValue(depth::processor);
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable | ItemSendsGeometryChanges);
    setRect(-size_.width() / 2, -size_.height() / 2, size_.width(), size_.height());

    {
        displayNameLabel_ =
            new LabelGraphicsItem(this, width - 2 * labelHeight - 10, Qt::AlignBottom);
        displayNameLabel_->setDefaultTextColor(Qt::white);
        QFont nameFont("Segoe", labelHeight, QFont::Black, false);
        nameFont.setPixelSize(pointSizeToPixelSize(labelHeight));
        displayNameLabel_->setFont(nameFont);
        displayNameLabel_->setText(utilqt::toQString(processor_->getDisplayName()));
        LabelGraphicsItemObserver::addObservation(displayNameLabel_);

        nameChange_ =
            processor->onDisplayNameChange([this](std::string_view newName, std::string_view) {
                auto newDisplayName = utilqt::toQString(newName);
                if (newDisplayName != displayNameLabel_->text()) {
                    displayNameLabel_->setText(newDisplayName);
                }
            });
    }
    {
        identifierLabel_ = new LabelGraphicsItem(this, width - 2 * labelHeight, Qt::AlignTop);
        identifierLabel_->setDefaultTextColor(Qt::lightGray);
        QFont classFont("Segoe", labelHeight, QFont::Normal, false);
        classFont.setPixelSize(pointSizeToPixelSize(labelHeight));
        identifierLabel_->setFont(classFont);
        identifierLabel_->setText(utilqt::toQString(processor_->getIdentifier()));
        LabelGraphicsItemObserver::addObservation(identifierLabel_);

        idChange_ =
            processor_->onIdentifierChange([this](std::string_view newID, std::string_view) {
                auto newIdentifier = utilqt::toQString(newID);
                if (newIdentifier != identifierLabel_->text()) {
                    identifierLabel_->setText(newIdentifier);
                }
            });
    }
    {
        tagLabel_ = new LabelGraphicsItem(this, width / 2, Qt::AlignTop);
        tagLabel_->setDefaultTextColor(Qt::lightGray);
        QFont classFont("Segoe", labelHeight, QFont::Bold, false);
        classFont.setPixelSize(pointSizeToPixelSize(labelHeight));
        tagLabel_->setFont(classFont);
        tagLabel_->setText(
            utilqt::toQString(util::getPlatformTags(processor_->getTags()).getString()));
    }
    auto tagSize = tagLabel_->usedTextWidth();
    identifierLabel_->setCrop(width - 2 * labelHeight - (tagSize > 0 ? tagSize + 4 : 0));
    positionLabels();

    processor_->ProcessorObservable::addObserver(this);
    processorMeta_->addObserver(this);

    for (auto& inport : processor_->getInports()) {
        addInport(inport);
    }
    for (auto& outport : processor_->getOutports()) {
        addOutport(outport);
    }

    statusItem_->setPos(rect().topRight() + QPointF(-9.0f, 9.0f));

    if (auto progressBarOwner = dynamic_cast<ProgressBarOwner*>(processor_)) {
        progressItem_ =
            new ProcessorProgressGraphicsItem(this, &(progressBarOwner->getProgressBar()));
        progressItem_->setPos(QPointF(0.0f, 9.0f));

        progressBarOwner->getProgressBar().ActivityIndicator::addObserver(statusItem_);
    }

    if (auto activityInd = dynamic_cast<ActivityIndicatorOwner*>(processor_)) {
        activityInd->getActivityIndicator().addObserver(statusItem_);
    }

    setVisible(processorMeta_->isVisible());
    setSelected(processorMeta_->isSelected());
    setPos(QPointF(processorMeta_->getPosition().x, processorMeta_->getPosition().y));

    if (processor_->status() == ProcessorStatus::Error) {
        setErrorText(processor_->status().reason());
    }
    statusItem_->updateState();
}

void ProcessorGraphicsItem::positionLabels() {
    static constexpr int labelMargin = 7;

    displayNameLabel_->setPos(QPointF(rect().left() + labelMargin, -2));
    identifierLabel_->setPos(QPointF(rect().left() + labelMargin, -3));

    auto offset = identifierLabel_->usedTextWidth();
    tagLabel_->setPos(QPointF(rect().left() + labelMargin + offset + 4, -3));
}

QPointF ProcessorGraphicsItem::portOffset(PortType type, size_t index) {
    const QPointF offset = {12.5f, (type == PortType::In ? 1.0f : -1.0f) * 4.5f};
    const QPointF delta = {12.5f, 0.0f};
    const QPointF rowDelta = {0.0f, (type == PortType::In ? -1.0f : 1.0f) * 12.5f};
    const size_t portsPerRow = 10;

    auto poffset = QPointF{-ProcessorGraphicsItem::size_.width() / 2,
                           (type == PortType::In ? -ProcessorGraphicsItem::size_.height() / 2
                                                 : ProcessorGraphicsItem::size_.height() / 2)};

    return poffset + offset + rowDelta * static_cast<qreal>(index / portsPerRow) +
           delta * static_cast<qreal>(index % portsPerRow);
}

QPointF ProcessorGraphicsItem::portPosition(PortType type, size_t index) {
    return rect().center() + portOffset(type, index);
}

void ProcessorGraphicsItem::addInport(Inport* port) {
    auto pos = portPosition(PortType::In, inportItems_.size());
    inportItems_[port] = new ProcessorInportGraphicsItem(this, pos, port);
}

void ProcessorGraphicsItem::addOutport(Outport* port) {
    auto pos = portPosition(PortType::Out, outportItems_.size());
    outportItems_[port] = new ProcessorOutportGraphicsItem(this, pos, port);
}

void ProcessorGraphicsItem::removeInport(Inport* port) {
    delete inportItems_[port];
    inportItems_.erase(port);

    size_t count = 0;
    for (auto& item : inportItems_) {
        item.second->setPos(portPosition(PortType::In, count));
        count++;
    }
    update();
}

void ProcessorGraphicsItem::removeOutport(Outport* port) {
    delete outportItems_[port];
    outportItems_.erase(port);
    size_t count = 0;
    for (auto& item : outportItems_) {
        item.second->setPos(portPosition(PortType::Out, count));
        count++;
    }
    update();
}

void ProcessorGraphicsItem::onProcessorMetaDataPositionChange() {
    if (!animation_) {
        animation_ = new QPropertyAnimation{this, "pos", this};
    }

    const auto iPos = processorMeta_->getPosition();
    const auto qPos = QPointF(iPos.x, iPos.y);

    if (animation_->state() == QAbstractAnimation::Running) {
        if (animation_->endValue().toPointF() == qPos) {
            return;
        } else {
            animation_->setCurrentTime(0);
            animation_->setStartValue(pos());
            animation_->setEndValue(qPos);
        }
    } else if (qPos != pos()) {
        animation_->setDuration(500);
        animation_->setStartValue(pos());
        animation_->setEndValue(qPos);
        animation_->setEasingCurve(QEasingCurve::InOutQuad);
        animation_->start();
    }
}

void ProcessorGraphicsItem::onProcessorMetaDataVisibilityChange() {
    if (processorMeta_->isVisible() != isVisible()) {
        setVisible(processorMeta_->isVisible());
    }
}

void ProcessorGraphicsItem::onProcessorMetaDataSelectionChange() {
    if (processorMeta_->isSelected() != isSelected()) {
        setSelected(processorMeta_->isSelected());
    }
}

ProcessorInportGraphicsItem* ProcessorGraphicsItem::getInportGraphicsItem(Inport* port) const {
    return util::map_find_or_null(inportItems_, port);
}
ProcessorOutportGraphicsItem* ProcessorGraphicsItem::getOutportGraphicsItem(Outport* port) const {
    return util::map_find_or_null(outportItems_, port);
}

ProcessorGraphicsItem::~ProcessorGraphicsItem() = default;

Processor* ProcessorGraphicsItem::getProcessor() const { return processor_; }

void ProcessorGraphicsItem::editDisplayName() {
    setFocus();
    displayNameLabel_->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

    QTextCursor cur = displayNameLabel_->textCursor();
    cur.movePosition(QTextCursor::End);
    displayNameLabel_->setTextCursor(cur);
    displayNameLabel_->setTextInteractionFlags(Qt::TextEditorInteraction);
    displayNameLabel_->setFocus();
    displayNameLabel_->setSelected(true);
}

void ProcessorGraphicsItem::editIdentifier() {
    setFocus();
    identifierLabel_->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

    QTextCursor cur = identifierLabel_->textCursor();
    cur.movePosition(QTextCursor::End);
    identifierLabel_->setTextCursor(cur);
    identifierLabel_->setTextInteractionFlags(Qt::TextEditorInteraction);
    identifierLabel_->setFocus();
    identifierLabel_->setSelected(true);
}

void ProcessorGraphicsItem::paint(QPainter* p,
                                  [[maybe_unused]] const QStyleOptionGraphicsItem* options,
                                  [[maybe_unused]] QWidget* widget) {

    const float roundedCorners = 9.0f;

    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);
    QColor selectionColor("#7a191b");
    QColor borderColor("#282828");

    if (isSelected()) {
        p->setBrush(selectionColor);
    } else {
        p->setBrush(backgroundColor_);
    }
    p->setPen(QPen(QBrush(borderColor), 2.0));

    p->drawRoundedRect(rect(), roundedCorners, roundedCorners);

#if IVW_PROFILING
    QFont font("Segoe", 8, QFont::Normal, false);
    font.setPixelSize(pointSizeToPixelSize(8));
    p->setFont(font);
    p->setPen(Qt::lightGray);
    p->drawText(rect().adjusted(120.0, -40.0, -5.0, 0.0), Qt::AlignRight | Qt::AlignBottom,
                QString::number(processCount_));
#endif

    p->restore();
}

bool ProcessorGraphicsItem::isEditingProcessorName() {
    return (displayNameLabel_->textInteractionFlags() == Qt::TextEditorInteraction);
}

void ProcessorGraphicsItem::snapToGrid() {
    QPointF newpos = NetworkEditor::snapToGrid(pos());
    if (newpos != pos()) {
        setPos(newpos);
    }
}

QVariant ProcessorGraphicsItem::itemChange(GraphicsItemChange change, const QVariant& value) {
#include <warn/push>
#include <warn/ignore/switch-enum>
    const auto isAnimating = [this]() {
        return animation_ != nullptr && animation_->state() == QAbstractAnimation::Running;
    };

    switch (change) {
        case QGraphicsItem::ItemPositionHasChanged: {
            QPointF newPos = value.toPointF();
            newPos.setX(round(newPos.x()));
            newPos.setY(round(newPos.y()));
            if (processorMeta_ && !isAnimating() &&
                QApplication::mouseButtons() == Qt::MouseButton::NoButton) {
                processorMeta_->setPosition(ivec2(newPos.x(), newPos.y()));
            }
            if (auto editor = qobject_cast<NetworkEditor*>(scene())) {
                editor->updateSceneSize();
            }
            return newPos;
        }
        case QGraphicsItem::ItemSelectedHasChanged:
            updateWidgets();
            if (!highlight_ && processorMeta_) {
                processorMeta_->setSelected(isSelected());
            }
            if (errorText_) {
                errorText_->setActive(isSelected() && scene()->selectedItems().size() == 1);
            }
            break;
        case QGraphicsItem::ItemVisibleHasChanged: {
            if (processorMeta_) {
                processorMeta_->setVisible(isVisible());
            }
            if (auto editor = qobject_cast<NetworkEditor*>(scene())) {
                editor->updateSceneSize();
            }
            break;
        }
        case QGraphicsItem::ItemSceneHasChanged:
            updateWidgets();
            if (processor_->status() == ProcessorStatus::Error) {
                setErrorText(processor_->status().reason());
            }
            break;
        default:
            break;
    }
#include <warn/pop>
    return QGraphicsItem::itemChange(change, value);
}

void ProcessorGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* e) {
    if (auto editor = getNetworkEditor()) {
        if (QApplication::keyboardModifiers() & Qt::AltModifier) {
            editor->showProcessorHelp(processor_->getClassIdentifier(), true);
            return;
        }
    }
    QGraphicsItem::mousePressEvent(e);
}
void ProcessorGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
    if (auto editor = getNetworkEditor()) {
        if (QApplication::keyboardModifiers() & Qt::AltModifier) {
            editor->showProcessorHelp(processor_->getClassIdentifier(), true);
            return;
        }
    }
    QGraphicsItem::mouseReleaseEvent(e);
}

void ProcessorGraphicsItem::updateWidgets() {
    if (isSelected()) {
        setZValue(depth::processorSelected);
        if (!highlight_) {
            if (auto editor = getNetworkEditor()) {
                editor->addPropertyWidgets(processor_);
            }
        }
    } else {
        setZValue(depth::processor);
        if (auto editor = getNetworkEditor()) {
            editor->removePropertyWidgets(processor_);
        }
    }
}

void ProcessorGraphicsItem::onLabelGraphicsItemChanged(LabelGraphicsItem* item) {
    if (item == displayNameLabel_ && displayNameLabel_->isFocusOut()) {
        auto newName = utilqt::fromQString(displayNameLabel_->text());
        if (!newName.empty()) {
            processor_->setDisplayName(newName);
            displayNameLabel_->setNoFocusOut();
        }
    } else if (item == identifierLabel_ && identifierLabel_->isFocusOut()) {
        auto newId = utilqt::fromQString(identifierLabel_->text());
        if (!newId.empty()) {
            try {
                processor_->setIdentifier(newId);
                identifierLabel_->setNoFocusOut();
            } catch (Exception& e) {
                identifierLabel_->setText(utilqt::toQString(processor_->getIdentifier()));
                LogWarn(e.getMessage());
            }
            positionLabels();
        }
    }
}

void ProcessorGraphicsItem::onLabelGraphicsItemEdited(LabelGraphicsItem*) { positionLabels(); }

std::string ProcessorGraphicsItem::getIdentifier() const { return processor_->getIdentifier(); }

ProcessorLinkGraphicsItem* ProcessorGraphicsItem::getLinkGraphicsItem() const { return linkItem_; }

void ProcessorGraphicsItem::onProcessorReadyChanged(Processor*) {
    statusItem_->resetState();

    if (processor_->status() == ProcessorStatus::Error) {
        setErrorText(processor_->status().reason());
    } else if (errorText_) {
        errorText_->clear();
    }
}

void ProcessorGraphicsItem::onProcessorPortAdded(Processor*, Port* port) {
    if (auto inport = dynamic_cast<Inport*>(port)) {
        addInport(inport);
    } else if (auto outport = dynamic_cast<Outport*>(port)) {
        addOutport(outport);
    }
}

void ProcessorGraphicsItem::onProcessorPortRemoved(Processor*, Port* port) {
    if (auto inport = dynamic_cast<Inport*>(port)) {
        removeInport(inport);
    } else if (auto outport = dynamic_cast<Outport*>(port)) {
        removeOutport(outport);
    }
}

void ProcessorGraphicsItem::onProcessorAboutToProcess(Processor*) {
#if IVW_PROFILING
    processCount_++;
    clock_.reset();
    clock_.start();
    update(rect().adjusted(120.0, -40.0, -5.0, 0.0));
#endif

    statusItem_->resetState();
    if (errorText_) {
        errorText_->clear();
    }
}

void ProcessorGraphicsItem::onProcessorFinishedProcess(Processor*) {
#if IVW_PROFILING
    clock_.stop();
    evalTime_ = clock_.getElapsedMilliseconds();
    maxEvalTime_ = maxEvalTime_ < evalTime_ ? evalTime_ : maxEvalTime_;
    totEvalTime_ += evalTime_;
#endif
}

#if IVW_PROFILING
void ProcessorGraphicsItem::resetTimeMeasurements() {
    processCount_ = 0;
    maxEvalTime_ = 0.0;
    evalTime_ = 0.0;
    totEvalTime_ = 0.0;
    update(rect().adjusted(120.0, -40.0, -5.0, 0.0));
}
#endif

ProcessorStatusGraphicsItem* ProcessorGraphicsItem::getStatusItem() const { return statusItem_; }

void ProcessorGraphicsItem::setErrorText(std::string_view error) {
    // Avoid adding the error text when we use generateProcessorPreview
    if (!scene() || utilqt::fromQString(scene()->objectName()) != NetworkEditor::name) return;
    if (!errorText_) {
        errorText_ = std::make_unique<ProcessorErrorItem>(this);
    }
    errorText_->setText(error);
    errorText_->setActive(isSelected());
}

void ProcessorGraphicsItem::showToolTip(QGraphicsSceneHelpEvent* e) {
    using P = Document::PathComponent;
    using H = utildoc::TableBuilder::Header;

    Document doc;
    auto b = doc.append("html").append("body");
    b.append("b", processor_->getDisplayName(), {{"style", "color:white;"}});
    utildoc::TableBuilder tb(b, P::end());

    tb(H("Identifier"), processor_->getIdentifier());
    tb(H("Class"), processor_->getClassIdentifier());
    tb(H("Category"), processor_->getCategory());
    tb(H("Code"), processor_->getCodeState());
    tb(H("Tags"), processor_->getTags().getString());
    tb(H("Status"), fmt::to_string(processor_->status()));
    if (auto error = processor_->getMetaData<StringMetaData>("ProcessError")) {
        tb(H("Error"), error->get());
    }
    tb(H("Valid"), processor_->getInvalidationLevel());
    tb(H("Source"), processor_->isSource() ? "Yes" : "No");
    tb(H("Sink"), processor_->isSink() ? "Yes" : "No");
    tb(H("Jobs"), fmt::to_string(backgroundJobs_));

#if IVW_PROFILING
    tb(H("Eval Count"), processCount_);
    tb(H("Eval Time"), util::msToString(evalTime_, true, true));
    tb(H("Mean Time"),
       util::msToString(totEvalTime_ / std::max(static_cast<double>(processCount_), 1.0), true,
                        true));
    tb(H("Max Time"), util::msToString(maxEvalTime_, true, true));
#endif

    showToolTipHelper(e, utilqt::toLocalQString(doc));
}

void ProcessorGraphicsItem::setHighlight(bool val) { highlight_ = val; }

}  // namespace inviwo
