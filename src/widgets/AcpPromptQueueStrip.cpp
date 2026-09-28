/*
 * This file is part of Notepad Next.
 * Copyright 2026 NotepadAI contributors
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "AcpPromptQueueStrip.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

AcpPromptQueueStrip::AcpPromptQueueStrip(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("acpPromptQueueStrip"));
    setStyleSheet(QStringLiteral(
        "QWidget#acpPromptQueueStrip { background: palette(alternate-base); border: 1px solid palette(mid); border-radius: 4px; }"
        "QLabel#queueWhisper { color: palette(placeholder-text); background: transparent; border: none; }"
        "QLabel#queuePreview { background: transparent; border: none; }"
        "QToolButton#queueRemove { color: palette(placeholder-text); background: transparent; border: none; padding: 1px 4px; border-radius: 3px; }"
        "QToolButton#queueRemove:hover { color: palette(text); background: palette(midlight); }"));

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(8, 6, 8, 6);
    outer->setSpacing(4);

    m_whisper = new QLabel(this);
    m_whisper->setObjectName(QStringLiteral("queueWhisper"));
    m_whisper->setWordWrap(true);
    outer->addWidget(m_whisper);

    m_listLayout = new QVBoxLayout();
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(4);
    outer->addLayout(m_listLayout);

    hide();
}

void AcpPromptQueueStrip::clearRows()
{
    if (!m_listLayout)
        return;
    while (m_listLayout->count() > 0) {
        QLayoutItem *item = m_listLayout->takeAt(0);
        if (!item)
            break;
        if (QWidget *w = item->widget()) {
            w->hide();
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete item;
    }
}

void AcpPromptQueueStrip::setItems(const QStringList &previews)
{
    if (!m_whisper || !m_listLayout)
        return;
    clearRows();
    if (previews.isEmpty()) {
        hide();
        return;
    }

    if (previews.size() == 1) {
        m_whisper->setText(tr("Queued · sending when the agent finishes"));
    } else {
        m_whisper->setText(tr("%1 queued · sending next when the agent finishes")
                               .arg(previews.size()));
    }

    for (int i = 0; i < previews.size(); ++i) {
        auto *row = new QWidget(this);
        auto *hl = new QHBoxLayout(row);
        hl->setContentsMargins(0, 0, 0, 0);
        hl->setSpacing(4);

        auto *preview = new QLabel(previews.at(i), row);
        preview->setObjectName(QStringLiteral("queuePreview"));
        preview->setTextFormat(Qt::PlainText);
        preview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        preview->setMinimumWidth(0);
        preview->setWordWrap(false);
        hl->addWidget(preview, 1);

        auto *remove = new QToolButton(row);
        remove->setObjectName(QStringLiteral("queueRemove"));
        remove->setAutoRaise(true);
        remove->setText(QStringLiteral("×"));
        remove->setToolTip(tr("Remove from queue"));
        remove->setAccessibleName(tr("Remove from queue"));
        connect(remove, &QToolButton::clicked, this, [this, i]() {
            emit removeRequested(i);
        });
        hl->addWidget(remove, 0);

        m_listLayout->addWidget(row);
    }

    show();
}
