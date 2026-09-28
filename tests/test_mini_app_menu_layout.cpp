/*
 * This file is part of NotepadAI.
 * Copyright 2026 NotepadAI contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QtTest>

#include "MiniAppDefinition.h"
#include "MiniAppMenuLayout.h"

class TestMiniAppMenuLayout : public QObject
{
    Q_OBJECT

private:
    static MiniAppDefinition app(const QString &name, const QString &group = {})
    {
        MiniAppDefinition def;
        def.name = name;
        def.url = QStringLiteral("https://example.com");
        def.group = group;
        return def;
    }

private slots:
    void unique_skipsEmpty_sortsCaseInsensitive();
    void unique_includesPathPrefixes();
    void split_dropsEmptySegments();
    void layout_groupsAzThenUngrouped();
    void layout_oneAppGroupStillNests();
    void layout_caseDistinctGroupsDoNotMerge();
    void layout_keepsMergedOrderInsideGroup();
    void layout_nestedSlashPath();
    void layout_siblingAppAndNestedGroup();
    void layout_deepPathIsCapped();
};

void TestMiniAppMenuLayout::unique_skipsEmpty_sortsCaseInsensitive()
{
    const QStringList names = uniqueMiniAppGroups({
        app(QStringLiteral("a"), QStringLiteral("Chat")),
        app(QStringLiteral("b"), QStringLiteral("  ")),
        app(QStringLiteral("c"), QStringLiteral("AWS")),
        app(QStringLiteral("d"), QStringLiteral("Chat")),
        app(QStringLiteral("e")),
    });
    QCOMPARE(names, QStringList({QStringLiteral("AWS"), QStringLiteral("Chat")}));
}

void TestMiniAppMenuLayout::unique_includesPathPrefixes()
{
    const QStringList names = uniqueMiniAppGroups({
        app(QStringLiteral("s"), QStringLiteral("Chat/Work")),
        app(QStringLiteral("c"), QStringLiteral("AWS/prod")),
    });
    QCOMPARE(names, QStringList({
        QStringLiteral("AWS"),
        QStringLiteral("AWS/prod"),
        QStringLiteral("Chat"),
        QStringLiteral("Chat/Work"),
    }));
}

void TestMiniAppMenuLayout::split_dropsEmptySegments()
{
    QCOMPARE(splitMiniAppGroupPath(QStringLiteral("gr1//gr2/")),
             QStringList({QStringLiteral("gr1"), QStringLiteral("gr2")}));
    QCOMPARE(splitMiniAppGroupPath(QStringLiteral(" /a/ b / ")),
             QStringList({QStringLiteral("a"), QStringLiteral("b")}));
    QVERIFY(splitMiniAppGroupPath(QString()).isEmpty());
}

void TestMiniAppMenuLayout::layout_groupsAzThenUngrouped()
{
    const MiniAppMenuNode root = buildMiniAppMenuTree({
        app(QStringLiteral("Grok")),
        app(QStringLiteral("Discord"), QStringLiteral("Chat")),
        app(QStringLiteral("aws"), QStringLiteral("AWS")),
        app(QStringLiteral("Zalo"), QStringLiteral("Chat")),
        app(QStringLiteral("Context Engine")),
    });
    QCOMPARE(root.children.size(), 2);
    QCOMPARE(root.children[0].name, QStringLiteral("AWS"));
    QCOMPARE(root.children[0].apps.size(), 1);
    QCOMPARE(root.children[0].apps[0].name, QStringLiteral("aws"));
    QCOMPARE(root.children[1].name, QStringLiteral("Chat"));
    QCOMPARE(root.children[1].apps.size(), 2);
    QCOMPARE(root.children[1].apps[0].name, QStringLiteral("Discord"));
    QCOMPARE(root.children[1].apps[1].name, QStringLiteral("Zalo"));
    QCOMPARE(root.apps.size(), 2);
    QCOMPARE(root.apps[0].name, QStringLiteral("Grok"));
    QCOMPARE(root.apps[1].name, QStringLiteral("Context Engine"));
}

void TestMiniAppMenuLayout::layout_oneAppGroupStillNests()
{
    const MiniAppMenuNode root = buildMiniAppMenuTree({
        app(QStringLiteral("Mailchimp"), QStringLiteral("Marketing")),
    });
    QCOMPARE(root.children.size(), 1);
    QCOMPARE(root.children[0].name, QStringLiteral("Marketing"));
    QCOMPARE(root.children[0].apps[0].name, QStringLiteral("Mailchimp"));
    QVERIFY(root.apps.isEmpty());
}

void TestMiniAppMenuLayout::layout_caseDistinctGroupsDoNotMerge()
{
    const MiniAppMenuNode root = buildMiniAppMenuTree({
        app(QStringLiteral("A"), QStringLiteral("Social")),
        app(QStringLiteral("B"), QStringLiteral("social")),
    });
    QCOMPARE(root.children.size(), 2);
    QCOMPARE(root.children[0].name, QStringLiteral("Social"));
    QCOMPARE(root.children[1].name, QStringLiteral("social"));
    QCOMPARE(root.children[0].apps[0].name, QStringLiteral("A"));
    QCOMPARE(root.children[1].apps[0].name, QStringLiteral("B"));
}

void TestMiniAppMenuLayout::layout_keepsMergedOrderInsideGroup()
{
    const MiniAppMenuNode root = buildMiniAppMenuTree({
        app(QStringLiteral("zeta"), QStringLiteral("G")),
        app(QStringLiteral("alpha"), QStringLiteral("G")),
    });
    QCOMPARE(root.children.size(), 1);
    QCOMPARE(root.children[0].apps[0].name, QStringLiteral("zeta"));
    QCOMPARE(root.children[0].apps[1].name, QStringLiteral("alpha"));
}

void TestMiniAppMenuLayout::layout_nestedSlashPath()
{
    const MiniAppMenuNode root = buildMiniAppMenuTree({
        app(QStringLiteral("App"), QStringLiteral("gr1/gr2")),
    });
    QCOMPARE(root.children.size(), 1);
    QCOMPARE(root.children[0].name, QStringLiteral("gr1"));
    QVERIFY(root.children[0].apps.isEmpty());
    QCOMPARE(root.children[0].children.size(), 1);
    QCOMPARE(root.children[0].children[0].name, QStringLiteral("gr2"));
    QCOMPARE(root.children[0].children[0].apps[0].name, QStringLiteral("App"));
}

void TestMiniAppMenuLayout::layout_siblingAppAndNestedGroup()
{
    const MiniAppMenuNode root = buildMiniAppMenuTree({
        app(QStringLiteral("Discord"), QStringLiteral("Chat")),
        app(QStringLiteral("Slack"), QStringLiteral("Chat/Work")),
    });
    QCOMPARE(root.children.size(), 1);
    const MiniAppMenuNode &chat = root.children[0];
    QCOMPARE(chat.name, QStringLiteral("Chat"));
    QCOMPARE(chat.children.size(), 1);
    QCOMPARE(chat.children[0].name, QStringLiteral("Work"));
    QCOMPARE(chat.children[0].apps[0].name, QStringLiteral("Slack"));
    QCOMPARE(chat.apps.size(), 1);
    QCOMPARE(chat.apps[0].name, QStringLiteral("Discord"));
}

void TestMiniAppMenuLayout::layout_deepPathIsCapped()
{
    QString path;
    for (int i = 0; i < kMaxMiniAppGroupDepth + 4; ++i) {
        if (!path.isEmpty())
            path += QLatin1Char('/');
        path += QStringLiteral("g%1").arg(i);
    }
    const MiniAppMenuNode root = buildMiniAppMenuTree({
        app(QStringLiteral("Deep"), path),
    });
    int depth = 0;
    const MiniAppMenuNode *node = &root;
    while (!node->children.isEmpty()) {
        ++depth;
        node = &node->children[0];
    }
    QCOMPARE(depth, kMaxMiniAppGroupDepth);
    QCOMPARE(node->apps[0].name, QStringLiteral("Deep"));
}

QTEST_MAIN(TestMiniAppMenuLayout)

#include "test_mini_app_menu_layout.moc"
