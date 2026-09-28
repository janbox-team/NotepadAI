/*
 * This file is part of NotepadAI.
 * Copyright 2026 NotepadAI contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "MiniAppDefinition.h"

#include <QList>
#include <QString>
#include <QStringList>

struct MiniAppMenuNode
{
    QString name;
    QList<MiniAppDefinition> apps;
    QList<MiniAppMenuNode> children;
};

inline constexpr int kMaxMiniAppGroupDepth = 8;

QStringList splitMiniAppGroupPath(const QString &group);
QStringList uniqueMiniAppGroups(const QList<MiniAppDefinition> &apps);
MiniAppMenuNode buildMiniAppMenuTree(const QList<MiniAppDefinition> &merged);
