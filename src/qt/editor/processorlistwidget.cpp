/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2018 Inviwo Foundation
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

#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/qt/editor/processorlistwidget.h>
#include <inviwo/qt/editor/helpwidget.h>
#include <inviwo/core/processors/processorstate.h>
#include <inviwo/core/processors/processortags.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/network/networkutils.h>
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/core/util/document.h>
#include <inviwo/core/util/rendercontext.h>
#include <modules/qtwidgets/inviwoqtutils.h>
#include <inviwo/core/metadata/processormetadata.h>
#include <inviwo/qt/editor/processorpreview.h>
#include <inviwo/qt/editor/processormimedata.h>

#include <warn/push>
#include <warn/ignore/all>
#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QMimeData>
#include <QHeaderView>
#include <QSettings>
#include <QVariant>

#include <chrono>
#include <ctime>
#include <warn/pop>

namespace inviwo {

const int ProcessorTree::identifierRole = Qt::UserRole + 1;
const int ProcessorTree::sortRole = Qt::UserRole + 2;
const int ProcessorTree::viewRole = Qt::UserRole + 3;
const int ProcessorTree::typeRole = Qt::UserRole + 4;

void ProcessorTree::mousePressEvent(QMouseEvent* e) {
    if (e->buttons() & Qt::LeftButton) dragStartPosition_ = e->pos();

    QTreeWidget::mousePressEvent(e);
}

void ProcessorTree::mouseMoveEvent(QMouseEvent* e) {
    if (e->buttons() & Qt::LeftButton) {
        if ((e->pos() - dragStartPosition_).manhattanLength() < QApplication::startDragDistance())
            return;

        auto item = itemAt(dragStartPosition_);
        if (item &&
            item->data(0, ProcessorTree::typeRole).toInt() == ProcessorTree::ProcessoorType) {
            auto id = item->data(0, identifierRole).toString();
            if (auto p = processorTreeWidget_->createProcessor(id)) {
                new ProcessorDragObject(this, std::move(p));
            }
        }
    }
}

ProcessorTree::ProcessorTree(ProcessorTreeWidget* parent)
    : QTreeWidget(parent), processorTreeWidget_{parent} {}

ProcessorTreeWidget::ProcessorTreeWidget(InviwoMainWindow* parent, HelpWidget* helpWidget)
    : InviwoDockWidget(tr("Processors"), parent, "ProcessorTreeWidget")
    , app_{parent->getInviwoApplication()}
    , helpWidget_{helpWidget} {

    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    resize(QSize(400, 700));  // default size

    QWidget* centralWidget = new QWidget();
    QVBoxLayout* vLayout = new QVBoxLayout(centralWidget);
    vLayout->setSpacing(7);
    vLayout->setContentsMargins(7, 7, 7, 7);
    lineEdit_ = new QLineEdit(centralWidget);
    lineEdit_->setPlaceholderText("Filter processor list...");
    lineEdit_->setClearButtonEnabled(true);

    connect(lineEdit_, &QLineEdit::textChanged, this, [this]() { addProcessorsToTree(); });
    vLayout->addWidget(lineEdit_);
    QHBoxLayout* listViewLayout = new QHBoxLayout();
    listViewLayout->addWidget(new QLabel("Group by", centralWidget));
    listView_ = new QComboBox(centralWidget);
    listView_->addItem("Alphabet");
    listView_->addItem("Category");
    listView_->addItem("Code State");
    listView_->addItem("Module");
    listView_->addItem("Last Used");
    listView_->addItem("Most Used");
    listView_->setCurrentIndex(1);
    connect(listView_, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            [this]() { addProcessorsToTree(); });
    listView_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    listViewLayout->addWidget(listView_);
    vLayout->addLayout(listViewLayout);

    iconStable_ = QIcon(":/icons/processor_stable.png");
    iconExperimental_ = QIcon(":/icons/processor_experimental.png");
    iconBroken_ = QIcon(":/icons/processor_broken.png");
    iconDeprecated_ = QIcon(":/icons/processor_deprecated.png");

    processorTree_ = new ProcessorTree(this);
    processorTree_->setHeaderHidden(true);
    processorTree_->setColumnCount(2);
    processorTree_->setIndentation(10);
    processorTree_->setAnimated(true);
    processorTree_->header()->setStretchLastSection(false);
    processorTree_->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    processorTree_->header()->setSectionResizeMode(1, QHeaderView::Fixed);
    processorTree_->header()->setDefaultSectionSize(40);

    connect(processorTree_, &ProcessorTree::currentItemChanged, this,
            &ProcessorTreeWidget::currentItemChanged);

    vLayout->addWidget(processorTree_);
    centralWidget->setLayout(vLayout);
    setWidget(centralWidget);

    onModulesDidRegister_ = app_->getModuleManager().onModulesDidRegister([this]() {
        addProcessorsToTree();
        app_->getProcessorFactory()->addObserver(this);
    });
    onModulesWillUnregister_ = app_->getModuleManager().onModulesWillUnregister([this]() {
        processorTree_->clear();
        app_->getProcessorFactory()->removeObserver(this);
    });

    QSettings settings;
    settings.beginGroup(objectName());
    lineEdit_->setText(settings.value("filterText", "").toString());
    listView_->setCurrentIndex(settings.value("currentView", 1).toInt());

    {
        auto useCounts = settings.value("useCounts", QVariant(QMap<QString, QVariant>{})).toMap();
        for (auto it = useCounts.constBegin(); it != useCounts.constEnd(); ++it) {
            useCounts_[utilqt::fromQString(it.key())] = it.value().toLongLong();
        }
    }
    {
        auto useTimes = settings.value("useTimes", QVariant(QMap<QString, QVariant>{})).toMap();
        for (auto it = useTimes.constBegin(); it != useTimes.constEnd(); ++it) {
            useTimes_[utilqt::fromQString(it.key())] = std::time_t(it.value().toLongLong());
        }
    }

    settings.endGroup();

    addProcessorsToTree();
}

ProcessorTreeWidget::~ProcessorTreeWidget() = default;

void ProcessorTreeWidget::focusSearch() {
    raise();
    lineEdit_->setFocus();
    lineEdit_->selectAll();
}

void ProcessorTreeWidget::addSelectedProcessor() {
    QString id;
    auto items = processorTree_->selectedItems();
    if (items.size() > 0) {
        id = items[0]->data(0, ProcessorTree::identifierRole).toString();
    } else {
        auto count = processorTree_->topLevelItemCount();
        if (count == 1) {
            auto item = processorTree_->topLevelItem(0);
            if (item->childCount() == 1) {
                id = item->child(0)->data(0, ProcessorTree::identifierRole).toString();
            }
        }
    }
    if (!id.isEmpty()) {
        addProcessor(id);
        processorTree_->clearSelection();
    } else {
        processorTree_->setFocus();
        processorTree_->topLevelItem(0)->child(0)->setSelected(true);
    }
}

std::unique_ptr<Processor> ProcessorTreeWidget::createProcessor(QString cid) {
    // Make sure the default render context is active to make sure any FBOs etc created in
    // the processor belong to the default context.
    RenderContext::getPtr()->activateDefaultRenderContext();
    const auto className = utilqt::fromQString(cid);
    try {
        if (auto p = app_->getProcessorFactory()->create(className)) {
            recordProcessorUse(className);
            return p;
        }
    } catch (Exception& exception) {
        util::log(
            exception.getContext(),
            "Unable to create processor \"" + className + "\" due to:\n" + exception.getMessage(),
            LogLevel::Error);
    }
    return nullptr;
}

void ProcessorTreeWidget::addProcessor(QString className) {
    // create processor, add it to processor network, and generate it's widgets
    auto network = app_->getProcessorNetwork();
    if (auto processor = createProcessor(className)) {
        auto meta = processor->getMetaData<ProcessorMetaData>(ProcessorMetaData::CLASS_IDENTIFIER);
        const auto bb = util::getBoundingBox(network->getProcessors());
        meta->setPosition(ivec2{bb.first.x, bb.second.y} + ivec2(0, 75));

        auto p = network->addProcessor(std::move(processor));
        util::autoLinkProcessor(network, p);
    }
}

bool ProcessorTreeWidget::processorFits(ProcessorFactoryObject* processor, const QString& filter) {
    return (
        QString::fromStdString(processor->getDisplayName()).contains(filter, Qt::CaseInsensitive) ||
        QString::fromStdString(processor->getClassIdentifier())
            .contains(filter, Qt::CaseInsensitive) ||
        QString::fromStdString(processor->getTags().getString())
            .contains(filter, Qt::CaseInsensitive));
}

const QIcon* ProcessorTreeWidget::getCodeStateIcon(CodeState state) const {
    switch (state) {
        case CodeState::Stable:
            return &iconStable_;

        case CodeState::Broken:
            return &iconBroken_;

        case CodeState::Deprecated:
            return &iconDeprecated_;

        case CodeState::Experimental:
        default:
            return &iconExperimental_;
    }
}
QTreeWidgetItem* ProcessorTreeWidget::addToplevelItemTo(QString title, const std::string& desc) {
    QTreeWidgetItem* newItem = new ProcessorTreeItem(QStringList(title));
    QList<QVariant> sortVal;
    sortVal.append(title);
    newItem->setData(0, ProcessorTree::sortRole, sortVal);
    newItem->setData(0, ProcessorTree::viewRole, listView_->currentIndex());
    newItem->setData(0, ProcessorTree::typeRole, ProcessorTree::GroupType);
    if (!desc.empty()) {
        newItem->setToolTip(0, utilqt::toLocalQString(desc));
    }
    processorTree_->addTopLevelItem(newItem);
    processorTree_->setFirstItemColumnSpanned(newItem, true);

    return newItem;
}

void ProcessorTreeWidget::onRegister(ProcessorFactoryObject* item) { addProcessorsToTree(item); }

void ProcessorTreeWidget::onUnRegister(ProcessorFactoryObject*) { addProcessorsToTree(); }

void ProcessorTreeWidget::closeEvent(QCloseEvent* event) {
    QSettings settings;
    settings.beginGroup(objectName());
    settings.setValue("filterText", QVariant(lineEdit_->text()));
    settings.setValue("currentView", QVariant(listView_->currentIndex()));

    {
        QMap<QString, QVariant> useCounts;
        for (const auto& i : useCounts_) {
            useCounts[utilqt::toQString(i.first)] = QVariant::fromValue<qint64>(i.second);
        }
        settings.setValue("useCounts", QVariant(useCounts));
    }
    {
        QMap<QString, QVariant> useTimes;
        for (const auto& i : useTimes_) {
            useTimes[utilqt::toQString(i.first)] = QVariant::fromValue<qint64>(i.second);
        }
        settings.setValue("useTimes", QVariant(useTimes));
    }
    settings.endGroup();

    InviwoDockWidget::closeEvent(event);
}

void ProcessorTreeWidget::recordProcessorUse(const std::string& id) {
    ++useCounts_[id];
    useTimes_[id] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

void ProcessorTreeWidget::addProcessorsToTree(ProcessorFactoryObject* item) {
    processorTree_->clear();
    // add processors from all modules to the list

    if (listView_->currentIndex() == 2) {
        addToplevelItemTo("Stable Processors", "");
        addToplevelItemTo("Experimental Processors", "");
        addToplevelItemTo("Broken Processors", "");
    }

    for (auto& elem : app_->getModules()) {
        for (auto& processor : elem->getProcessors()) {
            if (processor->isVisible() &&
                (lineEdit_->text().isEmpty() || processorFits(processor, lineEdit_->text()))) {
                extractInfoAndAddProcessor(processor, elem.get());
            }
        }
    }

    if (item && (lineEdit_->text().isEmpty() || processorFits(item, lineEdit_->text()))) {
        extractInfoAndAddProcessor(item, nullptr);
    }

    // Apply sorting
    switch (listView_->currentIndex()) {
        case 2: {  // By Code State
            int i = 0;
            while (i < processorTree_->topLevelItemCount()) {
                auto widget = processorTree_->topLevelItem(i);
                if (widget->childCount() == 0) {
                    delete processorTree_->takeTopLevelItem(i);
                } else {
                    widget->sortChildren(0, Qt::AscendingOrder);
                    i++;
                }
            }
            break;
        }
        default:
            processorTree_->sortItems(0, Qt::AscendingOrder);
            break;
    }

    processorTree_->expandAll();
    processorTree_->resizeColumnToContents(1);
}

void ProcessorTreeWidget::extractInfoAndAddProcessor(ProcessorFactoryObject* processor,
                                                     InviwoModule* elem) {
    std::string categoryName;
    std::string categoryDesc;
    QList<QVariant> sortVal;
    sortVal.append(QString::fromStdString(processor->getDisplayName()));

    switch (listView_->currentIndex()) {
        case 0:  // By Alphabet
            categoryName = processor->getDisplayName().substr(0, 1);
            categoryDesc = "";
            break;
        case 1:  // By Category
            categoryName = processor->getCategory();
            categoryDesc = "";
            break;
        case 2:  // By Code State
            categoryName = toString(processor->getCodeState());
            categoryDesc = "";
            break;
        case 3:  // By Module
            categoryName = elem ? elem->getIdentifier() : "Unkonwn";
            categoryDesc = elem ? elem->getDescription() : "";
            break;
        case 4: {  // Last Used
            auto it = useTimes_.find(processor->getClassIdentifier());
            if (it != useTimes_.end()) {
                sortVal.prepend(QVariant::fromValue<qint64>(-it->second));
            } else {
                sortVal.prepend(0);
            }
            categoryName = "";
            categoryDesc = "";
            break;
        }
        case 5: {  // Most Used
            auto it = useCounts_.find(processor->getClassIdentifier());
            if (it != useCounts_.end()) {
                sortVal.prepend(QVariant::fromValue<qint64>(-static_cast<qint64>(it->second)));
            } else {
                sortVal.prepend(0);
            }
            categoryName = "";
            categoryDesc = "";
            break;
        }
        default:
            categoryName = "Unkonwn";
            categoryDesc = "";
    }

    QTreeWidgetItem* item = nullptr;
    if (!categoryName.empty()) {
        QString category = QString::fromStdString(categoryName);
        auto items = processorTree_->findItems(category, Qt::MatchFixedString, 0);

        if (items.empty()) items.push_back(addToplevelItemTo(category, categoryDesc));
        item = items[0];
    }
    std::string moduleId = elem ? elem->getIdentifier() : "Unknown";

    auto newItem = new ProcessorTreeItem();
    newItem->setIcon(0, *getCodeStateIcon(processor->getCodeState()));
    newItem->setText(0, QString::fromStdString(processor->getDisplayName()));

    newItem->setTextAlignment(1, Qt::AlignRight);
    newItem->setData(0, ProcessorTree::identifierRole,
                     QString::fromStdString(processor->getClassIdentifier()));
    newItem->setData(0, ProcessorTree::sortRole, sortVal);
    newItem->setData(0, ProcessorTree::viewRole, listView_->currentIndex());
    newItem->setData(0, ProcessorTree::typeRole, ProcessorTree::ProcessoorType);

    // add only platform tags to second column
    auto platformTags = util::getPlatformTags(processor->getTags());
    const bool hasTags = !platformTags.empty();

    if (hasTags) {
        newItem->setText(1, utilqt::toQString(platformTags.getString() + " "));

        QFont font = newItem->font(1);
        font.setWeight(QFont::Bold);
        newItem->setFont(1, font);
    }

    {
        Document doc;
        using P = Document::PathComponent;
        auto b = doc.append("html").append("body");
        b.append("b", processor->getDisplayName(), {{"style", "color:white;"}});
        using H = utildoc::TableBuilder::Header;
        utildoc::TableBuilder tb(b, P::end(), {{"identifier", "propertyInfo"}});

        tb(H("Module"), moduleId);
        tb(H("Identifier"), processor->getClassIdentifier());
        tb(H("Category"), processor->getCategory());
        tb(H("Code"), processor->getCodeState());
        tb(H("Tags"), processor->getTags().getString());

        {
            auto it = useCounts_.find(processor->getClassIdentifier());
            if (it != useCounts_.end()) {
                tb(H("Uses"), it->second);
            } else {
                tb(H("Uses"), 0);
            }
        }

        {
            auto it = useTimes_.find(processor->getClassIdentifier());
            if (it != useTimes_.end()) {
                tb(H("Last"), std::string(std::ctime(&(it->second))));
            } else {
                tb(H("Last"), "Never");
            }
        }

        newItem->setToolTip(0, utilqt::toQString(doc));
        if (hasTags) {
            newItem->setToolTip(1, utilqt::toQString(doc));
        }
    }
    if (item) {
        item->addChild(newItem);
    } else {
        processorTree_->addTopLevelItem(newItem);
    }
    if (!hasTags) {
        processorTree_->setFirstItemColumnSpanned(newItem, true);
    }
}

void ProcessorTreeWidget::currentItemChanged(QTreeWidgetItem* current,
                                             QTreeWidgetItem* /*previous*/) {
    if (!current) return;
    auto classname =
        utilqt::fromQString(current->data(0, ProcessorTree::identifierRole).toString());
    if (!classname.empty()) {
        helpWidget_->showDocForClassName(classname);
    }
}

static QString mimeType = "inviwo/ProcessorDragObject";

ProcessorDragObject::ProcessorDragObject(QWidget* source, std::unique_ptr<Processor> processor)
    : QDrag(source) {
    auto img = QPixmap::fromImage(utilqt::generateProcessorPreview(processor.get(), 0.8));
    setPixmap(img);
    auto mime = new ProcessorMimeData(std::move(processor));
    setMimeData(mime);
    setHotSpot(QPoint(img.width() / 2, img.height() / 2));
    start(Qt::MoveAction);
}

bool ProcessorDragObject::canDecode(const QMimeData* mimeData) {
    return mimeData->hasFormat(mimeType);
}

bool ProcessorDragObject::decode(const QMimeData* mimeData, QString& className) {
    QByteArray byteData = mimeData->data(mimeType);

    if (byteData.isEmpty()) return false;

    QDataStream ds(&byteData, QIODevice::ReadOnly);
    ds >> className;
    return true;
}

bool ProcessorTreeItem::operator<(const QTreeWidgetItem& other) const {
    auto i = data(0, ProcessorTree::viewRole).toInt();
    auto a = data(0, ProcessorTree::sortRole);
    auto b = other.data(0, ProcessorTree::sortRole);

    switch (i) {
        case 0:  // By Alphabet
        case 1:  // By Category
        case 2:  // By Code State
        case 3:  // By Module
            return QString::compare(a.toList().front().toString(), b.toList().front().toString(),
                                    Qt::CaseInsensitive) < 0;

        case 4:  // Last used
        case 5:  // Most Used
            if (a.toList().front().toLongLong() == b.toList().front().toLongLong()) {
                return QString::compare(a.toList()[1].toString(), b.toList()[1].toString(),
                                        Qt::CaseInsensitive) < 0;
            } else {
                return a.toList().front().toLongLong() < b.toList().front().toLongLong();
            }
        default:
            return QString::compare(a.toList().front().toString(), b.toList().front().toString(),
                                    Qt::CaseInsensitive) < 0;
    }
}

}  // namespace inviwo
