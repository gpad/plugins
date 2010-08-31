/* * This file is part of meego-keyboard *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */



#ifndef MVIRTUALKEYBOARDSTYLE_H
#define MVIRTUALKEYBOARDSTYLE_H

#include <mwidgetstyle.h>
#include <QFont>

class MVirtualKeyboard;
class MScalableImage;

/*!
 * \class MVirtualKeyboardStyle
 * \brief This class provides access to style attributes for MVirtualKeyboard
 */
class MVirtualKeyboardStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(MVirtualKeyboardStyle)

public:
    M_STYLE_ATTRIBUTE(QSize, keyboardAreaSize, KeyboardAreaSize)
    M_STYLE_ATTRIBUTE(qreal, spacingVertical, SpacingVertical)
    M_STYLE_ATTRIBUTE(qreal, spacingHorizontal, SpacingHorizontal)
    M_STYLE_ATTRIBUTE(QColor, fontColor, FontColor)
    M_STYLE_ATTRIBUTE(qreal, fontOpacity, FontOpacity)
    M_STYLE_ATTRIBUTE(QFont, font, Font)
    M_STYLE_ATTRIBUTE(QFont, secondaryFont, SecondaryFont)
    M_STYLE_ATTRIBUTE(int, labelMarginTop, LabelMarginTop)
    M_STYLE_ATTRIBUTE(int, labelMarginLeftWithSecondary, LabelMarginLeftWithSecondary)
    M_STYLE_ATTRIBUTE(int, secondaryLabelSeparation, SecondaryLabelSeparation)
    M_STYLE_ATTRIBUTE(QSize, tabButtonSize, TabButtonSize)

    M_STYLE_ATTRIBUTE(QSize, menuSize, MenuSize)

    M_STYLE_ATTRIBUTE(QFont, notificationFont, NotificationFont)
    M_STYLE_ATTRIBUTE(QColor, notificationBorderColor, NotificationBorderColor)
    M_STYLE_ATTRIBUTE(QColor, notificationBackgroundColor, NotificationBackgroundColor)
    M_STYLE_ATTRIBUTE(QColor, notificationTextColor, NotificationTextColor)
    M_STYLE_ATTRIBUTE(qreal, notificationOpacity, NotificationOpacity)

    M_STYLE_ATTRIBUTE(QColor, deadkeyLockedColor, DeadkeyLockedColor)

    M_STYLE_ATTRIBUTE(int, flickGestureTimeout, FlickGestureTimeout)
    M_STYLE_ATTRIBUTE(qreal, flickGestureThresholdRatio, FlickGestureThresholdRatio)

    M_STYLE_ATTRIBUTE(QSize, keyNormalSize, KeyNormalSize)

    M_STYLE_ATTRIBUTE(QSizeF, keySizeSmall, KeySizeSmall)
    M_STYLE_ATTRIBUTE(QSize,  keySizeSmallFixed, KeySizeSmallFixed)

    M_STYLE_ATTRIBUTE(QSizeF, keySizeMedium, KeySizeMedium)
    M_STYLE_ATTRIBUTE(QSize,  keySizeMediumFixed, KeySizeMediumFixed)

    M_STYLE_ATTRIBUTE(QSizeF, keySizeLarge,  KeySizeLarge)
    M_STYLE_ATTRIBUTE(QSize,  keySizeLargeFixed,  KeySizeLargeFixed)

    M_STYLE_ATTRIBUTE(QSizeF, keySizeXLarge, KeySizeXLarge)
    M_STYLE_ATTRIBUTE(QSize,  keySizeXLargeFixed, KeySizeXLargeFixed)

    M_STYLE_ATTRIBUTE(QSizeF, keySizeXxLarge, KeySizeXxLarge)
    M_STYLE_ATTRIBUTE(QSize,  keySizeXxLargeFixed, KeySizeXxLargeFixed)

    M_STYLE_ATTRIBUTE(QSizeF, keySizeStretched, KeySizeStretched)
    M_STYLE_ATTRIBUTE(QSize,  keySizeStretchedFixed, KeySizeStretchedFixed)

    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackground, KeyBackground)
    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackgroundPressed, KeyBackgroundPressed)
    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackgroundSelected, KeyBackgroundSelected)
    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackgroundPressedSelected, KeyBackgroundPressedSelected)

    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackgroundSpecial, KeyBackgroundSpecial)
    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackgroundSpecialPressed, KeyBackgroundSpecialPressed)
    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackgroundSpecialSelected, KeyBackgroundSpecialSelected)
    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackgroundSpecialPressedSelected, KeyBackgroundSpecialPressedSelected)

    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackgroundDeadkey, KeyBackgroundDeadkey)
    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackgroundDeadkeyPressed, KeyBackgroundDeadkeyPressed)
    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackgroundDeadkeySelected, KeyBackgroundDeadkeySelected)
    M_STYLE_PTR_ATTRIBUTE(MScalableImage *, keyBackgroundDeadkeyPressedSelected, KeyBackgroundDeadkeyPressedSelected)

    M_STYLE_ATTRIBUTE(QSize,   keyBackspaceIconSize,    KeyBackspaceIconSize)
    M_STYLE_ATTRIBUTE(QString, keyBackspaceIconId,      KeyBackspaceIconId)
    M_STYLE_ATTRIBUTE(QString, keyBackspaceIconIdRtl,   KeyBackspaceIconIdRtl)

    M_STYLE_ATTRIBUTE(QSize, keyMenuIconSize, KeyMenuIconSize)
    M_STYLE_ATTRIBUTE(QString, keyMenuIconId, KeyMenuIconId)
    M_STYLE_ATTRIBUTE(QString, keyMenuIconIdRtl, KeyMenuIconIdRtl)

    M_STYLE_ATTRIBUTE(QSize, keyEnterIconSize, KeyEnterIconSize)
    M_STYLE_ATTRIBUTE(QString, keyEnterIconId, KeyEnterIconId)
    M_STYLE_ATTRIBUTE(QString, keyEnterIconIdRtl, KeyEnterIconIdRtl)

    M_STYLE_ATTRIBUTE(QSize, keyTabIconSize, KeyTabIconSize)
    M_STYLE_ATTRIBUTE(QString, keyTabIconId, KeyTabIconId)

    M_STYLE_ATTRIBUTE(QSize, keyShiftIconSize, KeyShiftIconSize)
    M_STYLE_ATTRIBUTE(QString, keyShiftIconId, KeyShiftIconId)
    M_STYLE_ATTRIBUTE(QString, keyShiftUppercaseIconId, KeyShiftUppercaseIconId)
};

class MVirtualKeyboardStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(MVirtualKeyboardStyle)

    M_STYLE_MODE(Landscape)
    M_STYLE_MODE(Portrait)

    friend class MVirtualKeyboard;
};

#endif
