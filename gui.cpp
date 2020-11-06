/*
 * Copyright (C) 2020 Computer Graphics Group, University of Siegen
 * Written by Martin Lambers <martin.lambers@uni-siegen.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <QMenu>
#include <QMenuBar>

#include "gui.hpp"


Gui::Gui(Set& set) : QMainWindow(),
    _qv(new QV(set, this))
{
    // TODO: where to put "Copy current view", "Copy 1:1 view"?

    QMenu* fileMenu = menuBar()->addMenu("&File");
    QAction* fileOpenAction = new QAction("&Open...", this);
    fileOpenAction->setShortcut(QKeySequence::Open);
    connect(fileOpenAction, SIGNAL(triggered()), this, SLOT(fileOpen()));
    fileMenu->addAction(fileOpenAction);
    QAction* fileReloadAction = new QAction("&Reload", this);
    fileReloadAction->setShortcut(QKeySequence::Refresh);
    connect(fileReloadAction, SIGNAL(triggered()), this, SLOT(fileReload()));
    fileMenu->addAction(fileReloadAction);
    QAction* fileCloseAction = new QAction("&Close", this);
    fileCloseAction->setShortcut(QKeySequence::Close);
    connect(fileCloseAction, SIGNAL(triggered()), this, SLOT(fileClose()));
    fileMenu->addAction(fileCloseAction);
    fileMenu->addSeparator();
    QAction* fileSaveCurrentViewAction = new QAction("Save current view...", this);
    //fileSaveCurrentViewAction->setShortcut(QKeySequence::Save);
    connect(fileSaveCurrentViewAction, SIGNAL(triggered()), this, SLOT(fileSaveCurrentView()));
    fileMenu->addAction(fileSaveCurrentViewAction);
    QAction* fileSaveViewAction = new QAction("&Save 1:1 view...", this);
    fileSaveViewAction->setShortcut(QKeySequence::Save);
    connect(fileSaveViewAction, SIGNAL(triggered()), this, SLOT(fileSaveView()));
    fileMenu->addAction(fileSaveViewAction);
    fileMenu->addSeparator();
    QAction* fileNextAction = new QAction("Jump to next file", this);
    connect(fileNextAction, SIGNAL(triggered()), this, SLOT(fileNext()));
    fileMenu->addAction(fileNextAction);
    QAction* filePrevAction = new QAction("Jump to previous file", this);
    connect(filePrevAction, SIGNAL(triggered()), this, SLOT(filePrev()));
    fileMenu->addAction(filePrevAction);
    QAction* fileNext10Action = new QAction("Jump 10 files forward", this);
    connect(fileNext10Action, SIGNAL(triggered()), this, SLOT(fileNext10()));
    fileMenu->addAction(fileNext10Action);
    QAction* filePrev10Action = new QAction("Jump 10 files backward", this);
    connect(filePrev10Action, SIGNAL(triggered()), this, SLOT(filePrev10()));
    fileMenu->addAction(filePrev10Action);
    QAction* fileNext100Action = new QAction("Jump 100 files forward", this);
    connect(fileNext100Action, SIGNAL(triggered()), this, SLOT(fileNext100()));
    fileMenu->addAction(fileNext100Action);
    QAction* filePrev100Action = new QAction("Jump 100 files backward", this);
    connect(filePrev100Action, SIGNAL(triggered()), this, SLOT(filePrev100()));
    fileMenu->addAction(filePrev100Action);
    fileMenu->addSeparator();
    QAction *fileQuitAction = new QAction("&Quit", this);
    fileQuitAction->setShortcut(QKeySequence::Quit);
    connect(fileQuitAction, SIGNAL(triggered()), this, SLOT(close()));
    fileMenu->addAction(fileQuitAction);

    setCentralWidget(_qv);
    _qv->setFocus();
}

void Gui::fileOpen()
{
    _qv->openFile();
}

void Gui::fileClose()
{
    _qv->closeFile();
}

void Gui::fileReload()
{
    _qv->reloadFile();
}

void Gui::fileSaveCurrentView()
{
    _qv->saveView(false);
}

void Gui::fileSaveView()
{
    _qv->saveView(true);
}

void Gui::fileNext()
{
    _qv->adjustFileIndex(+1);
}

void Gui::filePrev()
{
    _qv->adjustFileIndex(-1);
}

void Gui::fileNext10()
{
    _qv->adjustFileIndex(+10);
}

void Gui::filePrev10()
{
    _qv->adjustFileIndex(-10);
}

void Gui::fileNext100()
{
    _qv->adjustFileIndex(+100);
}

void Gui::filePrev100()
{
    _qv->adjustFileIndex(-100);
}
