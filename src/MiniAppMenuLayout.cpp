/*
 * This file is part of NotepadAI.
 * Copyright 2026 NotepadAI contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "MiniAppMenuLayout.h"

#include <QSet>

#include <algorithm>

QStringList splitMiniAppGroupPath(const QString &group)
{
    const QStringList raw = group.split(QLatin1Char('/'));
    QStringList parts;
    parts.reserve(raw.size());
    for (const QString &seg : raw) {
        const QString part = seg.trimmed();
        if (part.isEmpty())
            continue;
        parts.append(part);
        if (parts.size() == kMaxMiniAppGroupDepth)
            break;
    }
    return parts;
}

QStringList uniqueMiniAppGroups(const QList<MiniAppDefinition> &apps)
{
    QStringList names;
    QSet<QString> seen;
    names.reserve(apps.size());
    for (const MiniAppDefinition &def : apps) {
        const QStringList parts = splitMiniAppGroupPath(def.group);
        QString acc;
        for (const QString &part : parts) {
            acc = acc.isEmpty() ? part : acc + QLatin1Char('/') + part;
            if (seen.contains(acc))
                continue;
            seen.insert(acc);
            names.append(acc);
        }
    }
    std::stable_sort(names.begin(), names.end(), [](const QString &a, const QString &b) {
        return QString::compare(a, b, Qt::CaseInsensitive) < 0;
    });
    return names;
}

namespace {

MiniAppMenuNode *childByName(MiniAppMenuNode &node, const QString &name)
{
    for (MiniAppMenuNode &child : node.children) {
        if (child.name == name)
            return &child;
    }
    node.children.append({name, {}, {}});
    return &node.children.last();
}

void insertApp(MiniAppMenuNode &root, const QStringList &parts, const MiniAppDefinition &def)
{
    MiniAppMenuNode *node = &root;
    for (const QString &part : parts)
        node = childByName(*node, part);
    node->apps.append(def);
}

void sortChildren(MiniAppMenuNode &node)
{
    std::stable_sort(node.children.begin(), node.children.end(),
                     [](const MiniAppMenuNode &a, const MiniAppMenuNode &b) {
                         return QString::compare(a.name, b.name, Qt::CaseInsensitive) < 0;
                     });
    for (MiniAppMenuNode &child : node.children)
        sortChildren(child);
}

} // namespace

MiniAppMenuNode buildMiniAppMenuTree(const QList<MiniAppDefinition> &merged)
{
    MiniAppMenuNode root;
    for (const MiniAppDefinition &def : merged)
        insertApp(root, splitMiniAppGroupPath(def.group), def);
    sortChildren(root);
    return root;
}
