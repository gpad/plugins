/*
 * This file is part of Maliit Plugins
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 *
 * Contact: Mohammad Anwari <Mohammad.Anwari@nokia.com>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list
 * of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * Neither the name of Nokia Corporation nor the names of its contributors may be
 * used to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "glass.h"
#include "logic/hitlogic.h"
#include "models/keyarea.h"
#include "models/wordribbon.h"

#include <QGraphicsView>
#include <QWidget>

namespace MaliitKeyboard {

namespace {
void removeActiveKey(QVector<Key> *active_keys,
                     const Key &key)
{
    if (not active_keys) {
        return;
    }

    for (int index = 0; index < active_keys->count(); ++index) {
        if (active_keys->at(index) == key) {
            active_keys->remove(index);
            break;
        }
    }
}

}

class GlassPrivate
{
public:
    QWidget *window;
    QWidget *extendedWindow;
    QSharedPointer<Maliit::Plugins::AbstractGraphicsViewSurface> surface;
    QSharedPointer<Maliit::Plugins::AbstractGraphicsViewSurface> extendedSurface;
    QVector<SharedLayout> layouts;
    QVector<Key> active_keys;
    WordCandidate active_candidate;
    QPoint last_pos;
    QPoint press_pos;
    QElapsedTimer gesture_timer;
    bool gesture_triggered;
    QTimer long_press_timer;
    SharedLayout long_press_layout;
    bool mouse_captured;

    explicit GlassPrivate()
        : window(0)
        , extendedWindow(0)
        , surface()
        , extendedSurface()
        , layouts()
        , active_keys()
        , active_candidate()
        , last_pos()
        , press_pos()
        , gesture_timer()
        , gesture_triggered(false)
        , long_press_timer()
        , long_press_layout()
        , mouse_captured(false)
    {
        long_press_timer.setInterval(300);
        long_press_timer.setSingleShot(true);
    }
};

Glass::Glass(QObject *parent)
    : QObject(parent)
    , d_ptr(new GlassPrivate)
{
    connect(&d_ptr->long_press_timer, SIGNAL(timeout()),
            this,                     SLOT(onLongPressTriggered()),
            Qt::UniqueConnection);
}

Glass::~Glass()
{}

void Glass::setSurface(const QSharedPointer<Maliit::Plugins::AbstractGraphicsViewSurface> &surface)
{
    Q_D(Glass);

    QWidget *window = surface ? surface->view()->viewport() : 0;

    if (not window) {
        qCritical() << __PRETTY_FUNCTION__
                    << "No window given";
        return;
    }

    d->surface = surface;
    d->window = window;
    clearLayouts();

    d->window->installEventFilter(this);
}

void Glass::setExtendedSurface(const QSharedPointer<Maliit::Plugins::AbstractGraphicsViewSurface> &surface)
{
    Q_D(Glass);

    QWidget *window = surface ? surface->view()->viewport() : 0;

    if (not window) {
        qCritical() << __PRETTY_FUNCTION__
                    << "No window given";
        return;
    }

    d->extendedSurface = surface;
    d->extendedWindow = window;

    window->installEventFilter(this);
}

void Glass::addLayout(const SharedLayout &layout)
{
    Q_D(Glass);
    d->layouts.append(layout);
}

void Glass::clearLayouts()
{
    Q_D(Glass);
    d->layouts.clear();
}


QPoint translatePosition(const QPoint &pt, const QPoint &offset)
{
    return QPoint(pt.x() - offset.x(), pt.y() - offset.y());
}

QString describe(QEvent::Type type)
{
    switch(type)
    {
        case QEvent::Hide: return "QEvent::Hide";
        case QEvent::PaletteChange: return "QEvent::PaletteChange";
        case QEvent::DynamicPropertyChange: return "QEvent::DynamicPropertyChange";
        case QEvent::StyleChange: return "QEvent::StyleChange";
        case QEvent::Show: return "QEvent::Show";
        case QEvent::ToolTip: return "QEvent::ToolTip";
        case QEvent::MouseMove: return "QEvent::MouseMove";
        case QEvent::MouseButtonPress: return "QEvent::MouseButtonPress";
        case QEvent::MouseButtonRelease: return "QEvent::MouseButtonRelease";
        case QEvent::Enter: return "QEvent::Enter";
        case QEvent::Leave: return "QEvent::Leave";
        default:
            return QString("%1").arg(type);
    }
}

bool Glass::eventFilter(QObject *obj,
                        QEvent *ev)
{
    Q_D(Glass);
    static bool measure_fps(QCoreApplication::arguments().contains("-measure-fps"));

    if (not obj || not ev) {
        return false;
    }

    const QSharedPointer<Maliit::Plugins::AbstractGraphicsViewSurface> &eventSurface(obj == d->extendedWindow ? d->extendedSurface : d->surface);

    if (ev->type() != QEvent::Paint)
        qDebug() << "Glass::eventFilter " << describe(ev->type());
    switch(ev->type()) {
    case QEvent::Paint: {
        if (measure_fps) {
            static int count = 0;
            static QElapsedTimer fps_timer;

            if (0 == count % 120) {
                qDebug() << "FPS:" << count / ((0.01 + fps_timer.elapsed()) / 1000) << count;
                fps_timer.restart();
                count = 0;
            }

            d->window->update();
            ++count;
        }
    } break;

    case QKeyEvent::MouseButtonPress:
        if (!d->mouse_captured)
        {
            qDebug() << "CAPTURE mouse";
            d->window->grabMouse();
            d->mouse_captured = true;
        }
        d->gesture_timer.restart();
        d->gesture_triggered = false;

        return handlePressReleaseEvent(ev, eventSurface);

    case QKeyEvent::MouseButtonRelease:
        if (d->mouse_captured)
        {
            qDebug() << "RELEASE mouse";
            d->window->releaseMouse();
            d->mouse_captured = false;
        }
        d->long_press_timer.stop();

        if (d->gesture_triggered) {
            return false;
        }

        return handlePressReleaseEvent(ev, eventSurface);

    case QKeyEvent::MouseMove: {
        if (d->gesture_triggered) {
            qDebug() << "MouseMove -> exit for gesture_triggered";
            return false;
        }

        QMouseEvent *qme = static_cast<QMouseEvent *>(ev);
        ev->accept();

        Q_FOREACH (const SharedLayout &layout, d->layouts) {
            const QSharedPointer<Maliit::Plugins::AbstractGraphicsViewSurface> targetSurface(layout->activePanel() == Layout::ExtendedPanel ? d->extendedSurface : d->surface);

            QPoint pos(targetSurface->translateEventPosition(qme->pos(), eventSurface));
            QPoint last_pos(targetSurface->translateEventPosition(d->last_pos, eventSurface));

            QPoint press_pos(targetSurface->translateEventPosition(d->press_pos, eventSurface));

            const QRect &rect(layout->activeKeyAreaGeometry());

            if (layout->activePanel() == Layout::ExtendedPanel)
            {
                qDebug() << "MouseMove --> translate position from "
                << "offset: " << layout->extendedPanelOffset()
                << "pos: " << qme->pos()
                << "last_pos: " << d->last_pos
                << "press_pos: " << d->press_pos;
                pos = translatePosition(qme->pos(), layout->extendedPanelOffset());
                last_pos = d->last_pos;
                press_pos = d->press_pos;
                qDebug() << "MouseMove --> translate position to "
                << "pos: " << pos
                << "last_pos: " << last_pos
                << "press_pos: " << press_pos;
            }
            d->last_pos = qme->pos();

            if (d->gesture_timer.elapsed() < 250) {
                if (pos.y() > (press_pos.y() - rect.height() * 0.33)
                    && pos.y() < (press_pos.y() + rect.height() * 0.33)) {
                    if (pos.x() < (press_pos.x() - rect.width() * 0.33)) {
                        d->gesture_triggered = true;
                        qDebug() << "MouseMove -> switchRight";
                        Q_EMIT switchRight(layout);
                    } else if (pos.x() > (press_pos.x() + rect.width() * 0.33)) {
                        d->gesture_triggered = true;
                        qDebug() << "MouseMove -> switchLeft";
                        Q_EMIT switchLeft(layout);
                    }
                } else if (pos.x() > (press_pos.x() - rect.width() * 0.33)
                           && pos.x() < (press_pos.x() + rect.width() * 0.33)) {
                    if (pos.y() > (press_pos.y() + rect.height() * 0.50)) {
                        d->gesture_triggered = true;
                        qDebug() << "MouseMove -> keyboardClosed pos: " << pos << " press_pos: " << press_pos << " rect: " << rect;
                        Q_EMIT keyboardClosed();
                    }
                }
            }

            if (d->gesture_triggered) {
                Q_FOREACH (const Key &k, d->active_keys) {
                    qDebug() << "MouseMove -> keyExited: " << pos << "rect: " << rect;
                    Q_EMIT keyExited(k, layout);
                }

                d->active_keys.clear();

                return true;
            }

            const Key &last_key(Logic::keyHit(d->active_keys, rect, last_pos));


            const Key &key(Logic::keyHit(layout->activeKeyArea().keys(),
                                         layout->activeKeyAreaGeometry(),
                                         pos));

            if (last_key != key) {
                if (last_key.valid()) {
                    removeActiveKey(&d->active_keys, last_key);
                    d->long_press_timer.stop();
                    Q_EMIT keyExited(last_key, layout);
                }

                if (key.valid()) {
                    d->active_keys.append(key);

                    if (key.hasExtendedKeys()) {
                        d->long_press_timer.start();
                        d->long_press_layout = layout;
                    }

                    Q_EMIT keyEntered(key, layout);
                }
                // TODO: CHECK
                return true;
            }
        }
    } break;

    default:
        break;
    }

    return false;
}

void Glass::onLongPressTriggered()
{
    Q_D(Glass);
    qDebug() << "Glass::onLongPressTriggered";
    if (d->mouse_captured)
    {
        qDebug() << "RELEASE Mouse";
        d->window->releaseMouse();
        d->mouse_captured = false;
    }

    if (d->gesture_triggered || d->active_keys.isEmpty()
        || d->long_press_layout.isNull()
        || d->long_press_layout->activePanel() == Layout::ExtendedPanel) {
        return;
    }

    Q_FOREACH (const Key &k, d->active_keys) {
        Q_EMIT keyExited(k, d->long_press_layout); // Not necessarily correct layout for the key ...
    }

    Q_EMIT keyLongPressed(d->active_keys.last(), d->long_press_layout);
    d->active_keys.clear();
}

QDebug& operator<<(QDebug &xxx,  MaliitKeyboard::Label label)
{
    xxx << "---LABEL ---"  << "\n"
        << "text:" << label.text() << "\n"
        << "rect:" << label.rect() << "\n"
        << "------------" << "\n";
        //TODO font
    return xxx;
}

QDebug& operator<<(QDebug &xxx, MaliitKeyboard::Area area)
{
    xxx << "Area size:" << area.size() << "\n";
    return xxx;
}

QDebug& operator<<(QDebug &xxx, MaliitKeyboard::Key key)
{
    xxx << "--- KEY ---" << "\n"
        << "valid:" << key.valid() << "\n"
        << "rect:" << key.rect() << "\n"
        << "origin:" << key.origin() << "\n"
        << "area:" << key.area() << "\n"
        << "label:" << key.label() << "\n"
        << "action:" << key.action() << "\n"
        << "margins:" << key.margins() << "\n"
        << "hasExtendedKeys:" << key.hasExtendedKeys() << "\n"
        << "-----------"  << "\n";
    return xxx;
}

QDebug& operator<<(QDebug &xxx,  MaliitKeyboard::Layout layout)
{
    xxx << "--- LAYOUT ---" << "\n"
        << "--------------" << "\n";
    return xxx;
}

bool Glass::handlePressReleaseEvent(QEvent *ev, const QSharedPointer<Maliit::Plugins::AbstractGraphicsViewSurface> &eventSurface)
{
    qDebug() << "Glass::handlePressReleaseEvent";
    if (not ev) {
        return false;
    }

    Q_D(Glass);

    bool consumed = false;
    QMouseEvent *qme = static_cast<QMouseEvent *>(ev);
    d->last_pos = qme->pos();
    d->press_pos = qme->pos(); // FIXME: dont update on mouse release, clear instead.
    ev->accept();

    Q_FOREACH (const SharedLayout &layout, d->layouts) {
        const QSharedPointer<Maliit::Plugins::AbstractGraphicsViewSurface> targetSurface(layout->activePanel() == Layout::ExtendedPanel ? d->extendedSurface : d->surface);

        // const QPoint &pos(targetSurface->translateEventPosition(qme->pos(), eventSurface));
        QPoint pos(targetSurface->translateEventPosition(qme->pos(), eventSurface));

        switch (qme->type()) {
        case QKeyEvent::MouseButtonPress: {
            qDebug() << "[Glass::handlePressReleaseEvent] QKeyEvent::MouseButtonPress";

            const Key &key(Logic::keyHit(layout->activeKeyArea().keys(),
                                         layout->activeKeyAreaGeometry(),
                                         pos, d->active_keys));

            if (key.valid()) {
                d->active_keys.append(key);
                Q_EMIT keyPressed(key, layout);
                Q_EMIT keyAreaPressed(layout->activePanel(), layout);

                if (key.hasExtendedKeys()) {
                    d->long_press_timer.start();
                    d->long_press_layout = layout;
                }

                consumed = true;
            } else {
                const WordCandidate &candidate(Logic::wordCandidateHit(layout->wordRibbon().candidates(),
                                                                       layout->wordRibbonGeometry(),
                                                                       pos));

                if (candidate.valid()) {
                    d->active_candidate = candidate;
                    Q_EMIT wordCandidatePressed(candidate, layout);
                    consumed = true;
                }
            }
        } break;

        case QKeyEvent::MouseButtonRelease: {
            if (layout->activePanel() == Layout::ExtendedPanel)
            {
                // pos = QPoint(
                //     qme->pos().x() - layout->extendedPanelOffset().x(),
                //     qme->pos().y() - layout->extendedPanelOffset().y()
                //     );
                pos = translatePosition(qme->pos(), layout->extendedPanelOffset());
                qDebug() << "translating from " << qme->pos() << " to " << pos;
            }
            const Key &key(Logic::keyHit(layout->activeKeyArea().keys(),
                                         layout->activeKeyAreaGeometry(),
                                         pos, d->active_keys, Logic::AcceptIfInFilter));

            QString prefix = "[Glass::handlePressReleaseEvent] QKeyEvent::MouseButtonRelease ";
            // qDebug() << prefix;
            qDebug() 
            // << prefix << "keys:" << layout->activeKeyArea().keys() << "\n"
            // << prefix << "area:" << layout->activeKeyAreaGeometry() << "\n"
            << prefix << "pos:" << pos << "\n";
            // << prefix << "activepanel:" << layout->activePanel() << "\n"
            // << prefix << "layout->extendedPanelOffset" << layout->extendedPanelOffset() << "\n"
            // << prefix << "layout->activePanel() == Layout::ExtendedPanel" << (layout->activePanel() == Layout::ExtendedPanel) << "\n"
            // << prefix << "d->pos(): " << d->window->pos() << "\n"
            // << prefix << "other translate" << d->surface->translateEventPosition(qme->pos(), eventSurface) << "\n"
            // << prefix << "other translate es1" << targetSurface->translateEventPosition(qme->pos(), d->extendedSurface) << "\n"
            // << prefix << "other translate es1" << targetSurface->translateEventPosition(qme->pos(), d->surface) << "\n"

            // << prefix << "qme->pos(): " << qme->pos() << "\n"
            // << prefix << "eventSourface: " << eventSurface << "\n"
            // << prefix << "active_keys" << d->active_keys << "\n"
            // << prefix << "result -> key.valid:" << key.valid() << "\n"
            // << prefix << "result -> key.label.text:" << key.label().text() << "\n";

            if (key.valid()) {
                removeActiveKey(&d->active_keys, key);
                Q_EMIT keyReleased(key, layout);
                Q_EMIT keyAreaReleased(layout->activePanel(), layout);
                consumed = true;
            } else {
                const WordCandidate &candidate(Logic::wordCandidateHit(layout->wordRibbon().candidates(),
                                                                       layout->wordRibbonGeometry(),
                                                                       pos));

                if (candidate.valid() && candidate == d->active_candidate) {
                    d->active_candidate = WordCandidate();
                    Q_EMIT wordCandidateReleased(candidate, layout);
                    consumed = true;
                }
            }

        } break;

        default:
            break;
        }

        Layout::Panel panel = Layout::NumPanels;

        if (layout->centerPanelGeometry().contains(pos))
            panel = Layout::CenterPanel;
        else if (layout->extendedPanelGeometry().contains(pos))
            panel = Layout::ExtendedPanel;
        else if (layout->leftPanelGeometry().contains(pos))
            panel = Layout::LeftPanel;
        else if (layout->rightPanelGeometry().contains(pos))
            panel = Layout::RightPanel;

        if (panel != Layout::NumPanels) {
            if (qme->type() == QKeyEvent::MouseButtonPress) {
                Q_EMIT keyAreaPressed(panel, layout);
            } else if (qme->type() == QKeyEvent::MouseButtonRelease) {
                Q_EMIT keyAreaReleased(panel, layout);
            }

            return true;
        }
    }

    return consumed;
}

} // namespace MaliitKeyboard
