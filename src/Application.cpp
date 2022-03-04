// Copyright (c) 2021-2021.
// License: CC0 1.0 Universal
// Permissions:
// - Commercial use
// - Modification
// - Distribution
// - Private use
//
// Limitations:
// - Liability
// - Trademark use
// - Patent use
// - Warranty
//
// Author: Danil Andreev | danssg08@gmail.com | https://github.com/DanilAndreev

#include "Application.h"

#include <QApplication>

const char* Application::LOG_EVENT = "log";

Application::Application(QApplication* qApplication): registeredTools{}, qApplication(qApplication), _history(this) {
    this->_document = new Document{};
    this->initialize(this);
    this->mainWindow = new MainWindow{this};
}

Application &Application::showGUI() noexcept {
    this->mainWindow->resize(800, 600);
    this->mainWindow->show();
    return *this;
}

int Application::exec() {
    return this->qApplication->exec();
}

Application::~Application() {
    for (ITool* tool : this->registeredTools) {
        tool->uninitialize(this);
    }
    delete this->mainWindow;
    this->uninitialize(this);
    delete this->_document;
}

Application &Application::registerTool(ITool *tool) noexcept {
    tool->initialize(this);
    this->registeredTools.insert(tool);
    return *this;
}

MainWindow& Application::getMainWindow() const {
    return *mainWindow;
}

History &Application::history() noexcept {
    return this->_history;
}

void Application::log(const QString &message, LogEvent::LOG_LEVEL level) {
    LogEvent event{message, level};
    this->emit_event(Application::LOG_EVENT, event);
}

Document *Application::document() noexcept {
    return this->_document;
}

void Application::snapshotCreateEventHandler(SnapshotCreateEvent& event) {
    auto memento = new DocumentMemento{};
    auto* targetImage = dynamic_cast<QImage*>(&this->document()->image());
    if(!targetImage) return; //TODO: error to console.
    memento->image = *targetImage;
    event.snapshot->insert(std::make_pair("document", memento));
}

void Application::snapshotRestoreEventHandler(SnapshotRestoreEvent& event) {
    auto memento = dynamic_cast<DocumentMemento *>(event.snapshot->at("document"));
    auto oldImage = this->document()->swapImage(new Image{memento->image});
    delete oldImage;
    this->getMainWindow().getViewport()->canvas()->repaint();
}
