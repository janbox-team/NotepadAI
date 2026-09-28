/*
 * This file is part of Notepad Next.
 * Copyright 2026 NotepadAI contributors
 *
 * SPDX short: GPL-3.0-or-later
 */

#include <QtTest>

#include "AcpPromptQueue.h"

class TestAcpPromptQueue : public QObject
{
    Q_OBJECT

private slots:
    void classifySend_processingWithContent_enqueues();
    void classifySend_idleWithContent_sendsNow();
    void classifySend_processingNativeGoal_sidePrompts();
    void classifySend_emptyContent_ignored();
    void fifo_dequeue_returns_oldest();
    void removeAt_drops_only_that_item();
    void takeNextIfIdle_flushes_one_only_when_idle();
};

void TestAcpPromptQueue::classifySend_processingWithContent_enqueues()
{
    QCOMPARE(AcpPromptQueue::classifySend(true, true, false),
             AcpPromptQueue::SendKind::Enqueue);
}

void TestAcpPromptQueue::classifySend_idleWithContent_sendsNow()
{
    QCOMPARE(AcpPromptQueue::classifySend(false, true, false),
             AcpPromptQueue::SendKind::SendNow);
}

void TestAcpPromptQueue::classifySend_processingNativeGoal_sidePrompts()
{
    QCOMPARE(AcpPromptQueue::classifySend(true, true, true),
             AcpPromptQueue::SendKind::SidePrompt);
}

void TestAcpPromptQueue::classifySend_emptyContent_ignored()
{
    QCOMPARE(AcpPromptQueue::classifySend(false, false, false),
             AcpPromptQueue::SendKind::Ignore);
    QCOMPARE(AcpPromptQueue::classifySend(true, false, false),
             AcpPromptQueue::SendKind::Ignore);
}

void TestAcpPromptQueue::fifo_dequeue_returns_oldest()
{
    AcpPromptQueue q;
    q.enqueue({QStringLiteral("first"), {}});
    q.enqueue({QStringLiteral("second"), {}});
    QCOMPARE(q.size(), 2);
    const auto a = q.dequeue();
    QVERIFY(a.has_value());
    QCOMPARE(a->text, QStringLiteral("first"));
    const auto b = q.dequeue();
    QVERIFY(b.has_value());
    QCOMPARE(b->text, QStringLiteral("second"));
    QVERIFY(!q.dequeue().has_value());
    QCOMPARE(q.size(), 0);
}

void TestAcpPromptQueue::removeAt_drops_only_that_item()
{
    AcpPromptQueue q;
    q.enqueue({QStringLiteral("a"), {}});
    q.enqueue({QStringLiteral("b"), {}});
    q.enqueue({QStringLiteral("c"), {}});
    QVERIFY(q.removeAt(1));
    QCOMPARE(q.size(), 2);
    QCOMPARE(q.dequeue()->text, QStringLiteral("a"));
    QCOMPARE(q.dequeue()->text, QStringLiteral("c"));
    QVERIFY(!q.removeAt(0));
    QVERIFY(!q.removeAt(-1));
}

void TestAcpPromptQueue::takeNextIfIdle_flushes_one_only_when_idle()
{
    AcpPromptQueue q;
    q.enqueue({QStringLiteral("a"), {}});
    q.enqueue({QStringLiteral("b"), {}});
    QVERIFY(!q.takeNextIfIdle(true).has_value());
    QCOMPARE(q.size(), 2);
    const auto a = q.takeNextIfIdle(false);
    QVERIFY(a.has_value());
    QCOMPARE(a->text, QStringLiteral("a"));
    QCOMPARE(q.size(), 1);
    const auto b = q.takeNextIfIdle(false);
    QVERIFY(b.has_value());
    QCOMPARE(b->text, QStringLiteral("b"));
    QVERIFY(!q.takeNextIfIdle(false).has_value());
}

QTEST_MAIN(TestAcpPromptQueue)
#include "test_acp_prompt_queue.moc"
