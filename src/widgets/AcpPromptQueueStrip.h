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

#ifndef ACP_PROMPT_QUEUE_STRIP_H
#define ACP_PROMPT_QUEUE_STRIP_H

#include <QStringList>
#include <QWidget>

class QLabel;
class QVBoxLayout;

// Compact composer-adjacent list of prompts waiting for the agent to go idle.
// Hidden when empty. Remove is per-row; the owner owns queue mutation.
class AcpPromptQueueStrip : public QWidget
{
    Q_OBJECT

public:
    explicit AcpPromptQueueStrip(QWidget *parent = nullptr);

    void setItems(const QStringList &previews);

signals:
    void removeRequested(int index);

private:
    void clearRows();

    QLabel *m_whisper = nullptr;
    QVBoxLayout *m_listLayout = nullptr;
};

#endif // ACP_PROMPT_QUEUE_STRIP_H
