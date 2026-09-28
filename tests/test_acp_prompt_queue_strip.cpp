/*
 * This file is part of Notepad Next.
 * Copyright 2026 NotepadAI contributors
 *
 * SPDX short: GPL-3.0-or-later
 */

#include <QtTest>
#include <QLabel>
#include <QSignalSpy>
#include <QToolButton>

#include "AcpPromptQueueStrip.h"

class TestAcpPromptQueueStrip : public QObject
{
    Q_OBJECT

private slots:
    void hidden_when_empty();
    void one_item_shows_preview();
    void remove_emits_index();
    void setItems_replaces_previous_rows();
};

void TestAcpPromptQueueStrip::hidden_when_empty()
{
    AcpPromptQueueStrip w;
    w.show();
    w.setItems({QStringLiteral("hello")});
    QVERIFY(!w.isHidden());
    w.setItems({});
    QVERIFY(w.isHidden());
}

void TestAcpPromptQueueStrip::one_item_shows_preview()
{
    AcpPromptQueueStrip w;
    w.show();
    w.setItems({QStringLiteral("hello")});
    auto *whisper = w.findChild<QLabel *>(QStringLiteral("queueWhisper"));
    QVERIFY(whisper);
    QVERIFY(whisper->text().contains(QStringLiteral("Queued")));
    auto previews = w.findChildren<QLabel *>(QStringLiteral("queuePreview"));
    QCOMPARE(previews.size(), 1);
    QCOMPARE(previews.at(0)->text(), QStringLiteral("hello"));
}

void TestAcpPromptQueueStrip::remove_emits_index()
{
    AcpPromptQueueStrip w;
    w.show();
    QSignalSpy spy(&w, &AcpPromptQueueStrip::removeRequested);
    w.setItems({QStringLiteral("a"), QStringLiteral("b")});
    const auto buttons = w.findChildren<QToolButton *>(QStringLiteral("queueRemove"));
    QCOMPARE(buttons.size(), 2);
    QTest::mouseClick(buttons.at(1), Qt::LeftButton);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toInt(), 1);
}

void TestAcpPromptQueueStrip::setItems_replaces_previous_rows()
{
    AcpPromptQueueStrip w;
    w.show();
    w.setItems({QStringLiteral("old")});
    w.setItems({QStringLiteral("new")});
    const auto previews = w.findChildren<QLabel *>(QStringLiteral("queuePreview"));
    QCOMPARE(previews.size(), 1);
    QCOMPARE(previews.at(0)->text(), QStringLiteral("new"));
}

QTEST_MAIN(TestAcpPromptQueueStrip)
#include "test_acp_prompt_queue_strip.moc"
