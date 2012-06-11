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

#ifndef MALIIT_KEYBOARD_WORDENGINE_H
#define MALIIT_KEYBOARD_WORDENGINE_H

#include "models/text.h"
#include <QtCore>

namespace MaliitKeyboard {
namespace Logic {

class WordEnginePrivate;

//! \brief Provides word prediction and error correction
//!
//! Provides error correction based on preedit (default engine: hunspell) and
//! word predication based on surrounding text and preedit (default engine:
//! presage). WordEngine is a consumer of the shared Model::Text master copy,
//! but does not modify it.
class WordEngine
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(WordEngine)
    Q_DECLARE_PRIVATE(WordEngine)

public:
    //! C'tor.
    //! \param parent the parent object, takes ownership if valid.
    explicit WordEngine(QObject *parent = 0);
    //! D'tor.
    virtual ~WordEngine();

    //! Called when text model changed. Can trigger emission of
    //! candidatesUpdated signal. Can update face of preedit in
    //! \a text.
    //! \param text the shared text model
    Q_SLOT virtual void onTextChanged(const Model::SharedText &text);
    //! Emitted when new candidates have been calculuated, usually as a result
    //! of text changes.
    //! \param candidates the list of updated candidates
    Q_SIGNAL void candidatesUpdated(const QStringList &candidates);

private:
    const QScopedPointer<WordEnginePrivate> d_ptr;
};

}} // namespace Logic, MaliitKeyboard

#endif // MALIIT_KEYBOARD_WORDENGINE_H
