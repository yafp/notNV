#include <QtGui>
#include <mainwindow.h>
#include <qfile.h>                              // handling files
#include <qtextstream.h>                        // handling files
#include <QtNetwork/QNetworkReply>              // update check / downloading files
#include <QtNetwork/QNetworkAccessManager>      // updatecheck / downloading files
#include <QDomDocument>

// *************************************
// START
// *************************************
MainWindow::MainWindow()
{
    // Define name, version etc
    QString appName = "notNV";
    QString appTodaysCounter = "05";
    QString appBuildDate = "20110731";
    QString appStableVersion = "0.4";                                                                   // next milestone:  0.5 - Initial contact

    // main app tabwidget
    mainTabWidget = new QTabWidget;
    mainTabWidget->setTabPosition(QTabWidget::South);
    mainTabWidget->setTabShape(QTabWidget::Rounded);

    noteChangeStatus = new QLabel;                                                                      // UGLY UGLY UGLY HACK - used to keep track of changes to note
    versionField = new QLineEdit;                                                                       // hidden field - another ugly one
    versionField->setText(appStableVersion+appBuildDate+appTodaysCounter);

    //
    // tab0 - notes-tab
    //
    QWidget *notesTab = new QWidget(mainTabWidget);
    mainTabWidget->addTab(notesTab, tr("notes"));

    directoryComboBox = createComboBox(QDir::currentPath());                                            // notes storage location Combobox
    directoryComboBox->setEnabled(false);
    directoryComboBox->setToolTip("Displays selected folder");

    magicField = new QLineEdit;                                                                         // magicField - used for search & create
    magicField->setAcceptDrops(true);                                                                   // configure magicField
    magicField->setDragEnabled(true);
    magicField->setFixedHeight(30);
    magicField->setFrame(true);                                                                         // true = round corners, eye-candy && false = simple
    magicField->setPlaceholderText("search or create");
    magicField->setToolTip("Search or create notes here");
    connect(magicField, SIGNAL(textChanged (const QString&) ), this, SLOT(magicChanged(const QString&)));
    connect(magicField, SIGNAL(returnPressed()), this, SLOT(createNewNote()));

    filesFoundLabel = new QLabel;                                                                       // label for footer aka filesFound
    filesFoundLabel->setAlignment (Qt::AlignCenter);

    QPixmap saveCurrentNotePixmap(":/images/icon_Save.ico");                                                     // create new note button icon
    QIcon saveCurrentNoteIcon;
    saveCurrentNoteIcon.addPixmap(saveCurrentNotePixmap, QIcon::Normal, QIcon::Off);
    saveCurrentNewNoteButton = new QPushButton();
    saveCurrentNewNoteButton->setIcon(saveCurrentNoteIcon);
    saveCurrentNewNoteButton->setText("Save");
    saveCurrentNewNoteButton->setIconSize(QSize(16,16));
    saveCurrentNewNoteButton->setFlat(false);
    saveCurrentNewNoteButton->setToolTip("Saves current note");
    saveCurrentNewNoteButton->setEnabled(false);
    saveCurrentNewNoteButton->setHidden(true);
    saveCurrentNewNoteButton->setShortcut(QKeySequence::Save);
    connect(saveCurrentNewNoteButton, SIGNAL(clicked()), SLOT(saveNote()));


    QFont fnt;
    fnt.setFamily("Helvetica");

    noteContent = new QTextEdit;                                                                        // note content
    noteContent->setAcceptRichText(true);
    noteContent->setText("No Note Selected");
    noteContent->setFont(fnt);
    noteContent->setAlignment (Qt::AlignCenter);
    noteContent->setEnabled(true);
    noteContent->setReadOnly(true);
    noteContent->setToolTip("Displays content of currently selected note.");
    connect(noteContent, SIGNAL(textChanged()), SLOT(noteContentChanged()));

    QPixmap newNotePixmap(":/images/icon_New.ico");                                                     // create new note button icon
    QIcon newNoteIcon;
    newNoteIcon.addPixmap(newNotePixmap, QIcon::Normal, QIcon::Off);
    createNewNoteButton = new QPushButton();
    createNewNoteButton->setIcon(newNoteIcon);
    createNewNoteButton->setText("New");
    createNewNoteButton->setIconSize(QSize(16,16));
    createNewNoteButton->setFlat(false);
    createNewNoteButton->setToolTip("Create a new note");
    createNewNoteButton->setEnabled(false);
    connect(createNewNoteButton, SIGNAL(clicked()), SLOT(createNewNote()));

    createFilesTable();                                                                                 // create files-table

    notesSplitter = new QSplitter;                                                                      // we need a splitter between notes-table and noteContent -dynamic shit ole ole
    notesSplitter->setOrientation(Qt::Vertical);
    notesSplitter->addWidget(filesTable);
    notesSplitter->addWidget(noteContent);

    QGridLayout *notesLayout = new QGridLayout;                                                         // Layout
    notesLayout->addWidget(magicField, 1, 0, 1, 4);                                                     // search & create field
    notesLayout->addWidget(createNewNoteButton, 1, 4);                                                  // create new note button
    //notesLayout->addWidget(directoryComboBox, 2, 0, 1, 5);                                            // display current directory & used in several other functions . hidden now from core gui
    notesLayout->addWidget(notesSplitter, 3, 0, 1, 5);                                                  // splitter (table & notecontent)
    notesLayout->addWidget(filesFoundLabel, 4, 2);                                                      // amount of notes
    notesLayout->addWidget(saveCurrentNewNoteButton, 4, 4);

    QVBoxLayout *notesTabLayout = new QVBoxLayout;
    notesTabLayout->addLayout(notesLayout);
    notesTab->setLayout(notesTabLayout);

    //
    // tab1 - conf-tab
    //
    QWidget *confTab = new QWidget(mainTabWidget);
    mainTabWidget->addTab(confTab, tr("conf"));

    // Add Groupbox for storage location
    QGroupBox *storageLocationGroupBox = new QGroupBox(tr("Notes location"));

    chooseStorageLocationButtonLabel = new QLabel(tr("Please select your notes folder:"));              // Label to show selected folder

    QPixmap folderPixmap(":/images/icon_Folder.ico");                                                   // notes folder button
    QIcon folderIcon;
    folderIcon.addPixmap(folderPixmap, QIcon::Normal, QIcon::Off);
    chooseStorageLocationButton = new QPushButton();
    chooseStorageLocationButton->setIcon(folderIcon);
    chooseStorageLocationButton->setIconSize(QSize(16,16));
    chooseStorageLocationButton->setText("Choose...");
    chooseStorageLocationButton->setToolTip("Opens a folder selection dialog");
    connect(chooseStorageLocationButton, SIGNAL(clicked()), this, SLOT(browse()));

    selectedStorageLocationLabel = new QLabel;                                                          // add Label - displays selected folder
    selectedStorageLocationLabel = new QLabel(tr("Not yet selected"));
    selectedStorageLocationLabel->setEnabled(false);
    selectedStorageLocationLabel->setAlignment( Qt::AlignCenter );

    QGroupBox *uiSettingsGroupBox = new QGroupBox(tr("Settings"));                                      // Add Groupbox for gui checkboxes

    showMenuCheckbox = new QCheckBox("Show menubar", this);                                             // Checkbox - hide/show menubar
    showMenuCheckbox->setToolTip("Enables the menu/menubar.");
    connect(showMenuCheckbox, SIGNAL(toggled(bool)),this, SLOT(showMenuBar()));

    smallFontsCheckbox = new QCheckBox("Use small font in file-list", this);                            // Checkbox - hide/show menubar
    smallFontsCheckbox->setToolTip("Enables small fonts in file-list.");
    connect(smallFontsCheckbox, SIGNAL(toggled(bool)),this, SLOT(smallFontForFileList()));

    enableConfirmOnDeleteCheckbox = new QCheckBox("Enable confirm notes-deletion dialog", this);
    enableConfirmOnDeleteCheckbox->setToolTip("Hides the menu.");

    enableInAppEditingCheckbox = new QCheckBox("Enable In-App editing (Manual Save!)", this);
    enableInAppEditingCheckbox->setToolTip("Enables notes editing inside notNV.");
    connect(enableInAppEditingCheckbox, SIGNAL(toggled(bool)),this, SLOT(find()));                      // check for inAppEditing is handled by loadFiletoNotecontent routine

    QPixmap confPixmap(":/images/icon_Settings.ico");                                                   // save conf button
    QIcon confIcon;
    confIcon.addPixmap(confPixmap, QIcon::Normal, QIcon::Off);
    saveButton = new QPushButton();
    saveButton->setIcon(confIcon);
    saveButton->setIconSize(QSize(16,16));
    saveButton->setFlat(false);
    saveButton->setText("Save configuration");
    saveButton->setToolTip("Saves your current notNV configuration.");
    saveButton->setEnabled(true);
    connect(saveButton, SIGNAL(clicked()), SLOT(writeSettings()));

    QHBoxLayout *vbox = new QHBoxLayout;                                                                // storageLocationCheckbox
    vbox->addWidget(chooseStorageLocationButtonLabel);
    vbox->addStretch(1);
    vbox->addWidget(chooseStorageLocationButton);
    QVBoxLayout *blabox = new QVBoxLayout;
    blabox->addLayout(vbox);
    blabox->addWidget(selectedStorageLocationLabel);
    storageLocationGroupBox->setLayout(blabox);
    QVBoxLayout *uiSettingsBox = new QVBoxLayout;                                                       // ui Settings Layout
    uiSettingsBox->addWidget(showMenuCheckbox);
    uiSettingsBox->addWidget(smallFontsCheckbox);
    uiSettingsBox->addWidget(enableConfirmOnDeleteCheckbox);
    uiSettingsBox->addWidget(enableInAppEditingCheckbox);
    uiSettingsGroupBox->setLayout(uiSettingsBox);
    QVBoxLayout *confTabLayout = new QVBoxLayout;                                                       // main Layout confTab
    confTabLayout->addWidget(storageLocationGroupBox);
    confTabLayout->addStretch(1);
    confTabLayout->addWidget(uiSettingsGroupBox);
    confTabLayout->addStretch(1);
    confTabLayout->addWidget(saveButton);
    confTab->setLayout(confTabLayout);

    //
    // shortcuts-tab
    //
    QWidget *shortcutsTab = new QWidget(mainTabWidget);
    mainTabWidget->addTab(shortcutsTab, tr("shortcuts"));

    QGroupBox *shortcutsGroupBox = new QGroupBox(tr("Keyboard shortcuts"));                                                                                             // Add Groupbox for storage location
    QPixmap keyESCPixmap(":/images/icon_KeyESC.png");
    QLabel *keyESCLabel = new QLabel;
    keyESCLabel->setPixmap(keyESCPixmap);
    escShortcutLabel = new QLabel(tr("<ul><li>resets search-field if focus in search-field</li><li>jumps to search-field if focus in file-list</li><li>jumps to file-list if focus in note-content</li></ul>"));            // add ESC description
    escShortcutLabel->setWordWrap(true);
    QPixmap keyENTERPixmap(":/images/icon_KeyENTER.png");
    QLabel *keyENTERLabel = new QLabel;
    keyENTERLabel->setPixmap(keyENTERPixmap);
    returnShortcutLabel = new QLabel(tr("<ul><li>opens selected note</li><li>creates new note if focus in search-field and name entered</li></ul>"));                   // add RETURN description
    returnShortcutLabel->setWordWrap(true);
    QPixmap keyDOWNPixmap(":/images/icon_KeyDOWN.png");
    QLabel *keyDOWNLabel = new QLabel;
    keyDOWNLabel->setPixmap(keyDOWNPixmap);
    arrowDownShortcutLabel = new QLabel(tr("<ul><li>jumps to file-list if focus in search-field</li><li>jumps to next note if focus in file-list</li></ul>"));          // add ARROWDOWN description
    arrowDownShortcutLabel->setWordWrap(true);
    QPixmap keyUPPixmap(":/images/icon_KeyUP.png");
    QLabel *keyUPLabel = new QLabel;
    keyUPLabel->setPixmap(keyUPPixmap);
    arrowUpShortcutLabel = new QLabel(tr("<ul><li>jumps to previous note if focus in file-list</li></ul>"));                                                            // add ARROWUP description
    arrowUpShortcutLabel->setWordWrap(true);
    QPixmap keyDELPixmap(":/images/icon_KeyDEL.png");
    QLabel *keyDELLabel = new QLabel;
    keyDELLabel->setPixmap(keyDELPixmap);
    delShortcutLabel = new QLabel(tr("<ul><li>deletes selected note</li></ul>"));                                                                                        // add DEL description
    delShortcutLabel->setWordWrap(true);
    QPixmap keyTABPixmap(":/images/icon_KeyTAB.png");
    QLabel *keyTABLabel = new QLabel;
    keyTABLabel->setPixmap(keyTABPixmap);
    tabShortcutLabel = new QLabel(tr("<ul><li>jump to next ui element</li></ul>"));                                                                                      // add TAB description
    tabShortcutLabel->setWordWrap(true);
    QPixmap keyF1Pixmap(":/images/icon_KeyF1.png");
    QLabel *keyF1Label = new QLabel;
    keyF1Label->setPixmap(keyF1Pixmap);
    f1ShortcutLabel = new QLabel(tr("<ul><li>opens help</li></ul>"));                                                                                                     // add F1 description
    f1ShortcutLabel->setWordWrap(true);
    QPixmap keyF2Pixmap(":/images/icon_KeyF2.png");
    QLabel *keyF2Label = new QLabel;
    keyF2Label->setPixmap(keyF2Pixmap);
    f2ShortcutLabel = new QLabel(tr("<ul><li>rename note</li></ul>"));                                                                                                     // add F2 description
    f2ShortcutLabel->setWordWrap(true);
    QPixmap keyF5Pixmap(":/images/icon_KeyF5.png");
    QLabel *keyF5Label = new QLabel;
    keyF5Label->setPixmap(keyF5Pixmap);
    f5ShortcutLabel = new QLabel(tr("<ul><li>reload file-list</li></ul>"));                                                                                                     // add F5 description
    f5ShortcutLabel->setWordWrap(true);

    QGridLayout *shortcutsLayout = new QGridLayout;                             // Layout
    shortcutsLayout->addWidget(keyESCLabel, 2,1);                               // ESC
    shortcutsLayout->addWidget(escShortcutLabel, 2, 2, 1, 4);
    shortcutsLayout->addWidget(keyENTERLabel, 3,1);                             // ENTER
    shortcutsLayout->addWidget(returnShortcutLabel, 3, 2, 1, 4);
    shortcutsLayout->addWidget(keyDOWNLabel, 4,1);                              // DOWN
    shortcutsLayout->addWidget(arrowDownShortcutLabel, 4,2,1,4);
    shortcutsLayout->addWidget(keyUPLabel, 5,1);                                // UP
    shortcutsLayout->addWidget(arrowUpShortcutLabel, 5,2,1,4);
    shortcutsLayout->addWidget(keyDELLabel, 6,1);                               // DEL
    shortcutsLayout->addWidget(delShortcutLabel, 6,2,1,4);
    shortcutsLayout->addWidget(keyTABLabel, 7,1);                               // TAB
    shortcutsLayout->addWidget(tabShortcutLabel, 7,2,1,4);
    shortcutsLayout->addWidget(keyF1Label, 8,1);                                // F1
    shortcutsLayout->addWidget(f1ShortcutLabel, 8,2,1,4);
    shortcutsLayout->addWidget(keyF2Label, 9,1);                                // F2
    shortcutsLayout->addWidget(f2ShortcutLabel, 9,2,1,4);
    shortcutsLayout->addWidget(keyF5Label, 10,1);                               // F2
    shortcutsLayout->addWidget(f5ShortcutLabel, 10,2,1,4);
    shortcutsGroupBox->setLayout(shortcutsLayout);

    QVBoxLayout *newShortcutsTabLayout = new QVBoxLayout;                       // main Layout confTab
    newShortcutsTabLayout->addWidget(shortcutsGroupBox);
    newShortcutsTabLayout->addStretch(1);
    shortcutsTab->setLayout(newShortcutsTabLayout);

    //
    // stats-tab
    //
    QWidget *statsTab = new QWidget(mainTabWidget);
    mainTabWidget->addTab(statsTab, tr("stats"));

    QGroupBox *statsGroupBox = new QGroupBox(tr("Statistics"));

    statsGenerationTimeLabel = new QLabel(tr("Stats generation timestamp: empty"));
    statsGenerationTimeLabel->setEnabled(false);

    statsSpacer = new QLabel(tr(""));

    appUsageCounterTitle = new QLabel(tr("notNV Launchcounter:"));
    appUsageCounterTitleResult = new QLabel(tr("1"));
    // note counter
    statsNotesCounterLabel = new QLabel(tr("Amount of notes:"));                                // amount of notes
    statsNotesCounterResultLabel = new QLabel(tr("0"));
    // more about note titles
    statsShortestTitle = new QLabel(tr("Shortest note title:"));                                // shortest title
    statsShortestResultTitle= new QLabel(tr("0"));
    statsAverageNoteTitleLengthLabel = new QLabel(tr("Average note title length:"));            // average title
    statsAverageNoteTitleLengthResultLabel= new QLabel(tr("0"));
    statsLargestTitleLabel = new QLabel(tr("Largest note title:"));                             // largest title
    statsLargestTitleResultLabel = new QLabel(tr("0"));
    // more about content? shortest note? longest note? average note? random note
    statsAverageLinesTitle = new QLabel(tr("Average Lines:"));                                  // largest title
    statsAverageLinesResultTitle = new QLabel(tr("0"));
    statsAverageCharsTitle = new QLabel(tr("Average Chars:"));                                  // average chars
    statsAverageCharsResultTitle = new QLabel(tr("0"));
    statsOverallLinesTitle = new QLabel(tr("Overall Lines:"));                                  // largest title
    statsOverallLinesResultTitle = new QLabel(tr("0"));
    statsOverallCharsTitle = new QLabel(tr("Overall Chars:"));                                  // average chars
    statsOverallCharsResultTitle = new QLabel(tr("0"));

    QPixmap statsPixmap(":/images/icon_Stats.ico");                                             // stats button
    QIcon statsIcon;
    statsIcon.addPixmap(statsPixmap, QIcon::Normal, QIcon::Off);
    statsButton = new QPushButton();
    statsButton->setText("Generate stats");
    statsButton->setIcon(statsIcon);
    statsButton->setIconSize(QSize(16,16));
    statsButton->setFlat(false);
    statsButton->setToolTip("Generates note stats");
    connect(statsButton, SIGNAL(clicked()), SLOT(generateStats()));

    // Layout
    QGridLayout *statsTabLayout = new QGridLayout;                                      // Layout
    statsTabLayout->addWidget(statsGenerationTimeLabel, 0, 1);
    statsTabLayout->addWidget(appUsageCounterTitle, 1, 1);
    statsTabLayout->addWidget(appUsageCounterTitleResult, 1, 3);
    statsTabLayout->addWidget(statsSpacer , 2, 1);
    statsTabLayout->addWidget(statsNotesCounterLabel, 3, 1);                            // Notescounter text
    statsTabLayout->addWidget(statsNotesCounterResultLabel, 3, 3);                      // result
    statsTabLayout->addWidget(statsSpacer , 4, 1);
    statsTabLayout->addWidget(statsOverallLinesTitle , 5, 1);                           // overallLines
    statsTabLayout->addWidget(statsOverallLinesResultTitle, 5, 3);                      // result
    statsTabLayout->addWidget(statsOverallCharsTitle , 6, 1);                           // overallChars
    statsTabLayout->addWidget(statsOverallCharsResultTitle, 6, 3);                      // result
    statsTabLayout->addWidget(statsSpacer , 7, 1);
    statsTabLayout->addWidget(statsAverageLinesTitle , 8, 1);                           // averageLines
    statsTabLayout->addWidget(statsAverageLinesResultTitle, 8, 3);                      // result
    statsTabLayout->addWidget(statsAverageCharsTitle , 9, 1);                           // averageCchars
    statsTabLayout->addWidget(statsAverageCharsResultTitle, 9, 3);                      // result
    statsTabLayout->addWidget(statsSpacer , 10, 1);
    statsTabLayout->addWidget(statsShortestTitle, 11, 1);                                // shortest title
    statsTabLayout->addWidget(statsShortestResultTitle, 11, 3);                          // result
    statsTabLayout->addWidget(statsAverageNoteTitleLengthLabel, 12, 1);                  // average title
    statsTabLayout->addWidget(statsAverageNoteTitleLengthResultLabel, 12, 3);            // result
    statsTabLayout->addWidget(statsLargestTitleLabel , 13, 1);                           // largest title
    statsTabLayout->addWidget(statsLargestTitleResultLabel, 13, 3);                      // result

    statsGroupBox->setLayout(statsTabLayout);

    QVBoxLayout *newStatsTabLayout = new QVBoxLayout;                                   // main Layout confTab
    newStatsTabLayout->addWidget(statsGroupBox);
    newStatsTabLayout->addStretch(1);
    newStatsTabLayout->addWidget(statsButton);
    statsTab->setLayout(newStatsTabLayout);

    //
    // about-tab
    //
    QWidget *aboutTab = new QWidget(mainTabWidget);
    mainTabWidget->addTab(aboutTab, tr("about"));

    QPixmap appPixmap(":/images/icon_notNV_big.png");
    QLabel *appIconBig = new QLabel;
    appIconBig->setPixmap(appPixmap);
    appIconBig->setAlignment(Qt::AlignCenter | Qt::AlignCenter);

    // add info about app - replacement for about window
    appInfoLabel = new QLabel(("<b>"+appName+"</b>     "+appStableVersion+"<br>" +appBuildDate+"."+appTodaysCounter+ "<br><br><small><font color=\"grey\">notNV is Free and Open Source Software. You are free to use, modify and redistribute it under the terms of the GNU General Public License Version 3 or later.<br><br>Copyright 2011 Florian Poeck</font></small>"));
    appInfoLabel->setWordWrap(true);

    QPixmap helpPixmap(":/images/icon_Help.ico");                      // help button
    QIcon helpIcon;
    helpIcon.addPixmap(helpPixmap, QIcon::Normal, QIcon::Off);
    helpButton = new QPushButton();
    helpButton->setText("Open Help");
    helpButton->setIcon(helpIcon);
    helpButton->setIconSize(QSize(16,16));
    helpButton->setFlat(false);
    helpButton->setToolTip("Opens online help in default browser");
    connect(helpButton, SIGNAL(clicked()), SLOT(openHelp()));

    QPixmap creditsPixmap(":/images/icon_Credits.ico");                      // credits button
    QIcon creditsIcon;
    creditsIcon.addPixmap(creditsPixmap, QIcon::Normal, QIcon::Off);
    showCreditsButton = new QPushButton();
    showCreditsButton->setIcon(creditsIcon);
    showCreditsButton->setIconSize(QSize(16,16));
    showCreditsButton->setText("Show Credits");
    showCreditsButton->setToolTip("Opens credits file in default browser");
    connect(showCreditsButton, SIGNAL(clicked()), SLOT(showCredits()));

    QPixmap websitePixmap(":/images/icon_Website.ico");                      // website button
    QIcon websiteIcon;
    websiteIcon.addPixmap(websitePixmap, QIcon::Normal, QIcon::Off);
    openWebsiteButton = new QPushButton();
    openWebsiteButton->setIcon(websiteIcon);
    openWebsiteButton->setIconSize(QSize(16,16));
    openWebsiteButton->setText("Visit Website");
    openWebsiteButton->setToolTip("Opens project page in default browser");
    connect(openWebsiteButton, SIGNAL(clicked()), SLOT(visitWebsite()));

    QPixmap updatePixmap(":/images/icon_Update.ico");                      // update button
    QIcon updateIcon;
    updateIcon.addPixmap(updatePixmap, QIcon::Normal, QIcon::Off);
    updateCheckButton = new QPushButton();
    updateCheckButton->setIcon(updateIcon);
    updateCheckButton->setIconSize(QSize(16,16));
    updateCheckButton->setText("Check for updates");
    updateCheckButton->setToolTip("Checks if you are running the latest notNV version");
    updateCheckButton->setEnabled(true);
    connect(updateCheckButton, SIGNAL(clicked()), SLOT(checkForUpdates()));

    QTextEdit *changelogTextEdit = new QTextEdit;
    changelogTextEdit->setReadOnly(true);
    changelogTextEdit->setFontPointSize(7);
    changelogTextEdit->setToolTip("Displays Changelog");

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    changelogTextEdit->setFont(font);

    QFile file(":/doc/CHANGELOG");
    if(file.open(QFile::ReadOnly))                              // read content of file into textedit if possible
    {
        QTextStream in(&file);
        QString text = in.readAll();
        changelogTextEdit->setPlainText(text);
    }
    else
    {
        changelogTextEdit->setPlainText("Changelog file is missing.");
    }
    file.close();

    infoLayout = new QHBoxLayout;                               // info/about Layout
    infoLayout->addWidget(appIconBig);
    infoLayout->addWidget(appInfoLabel);

    QVBoxLayout *aboutTabLayout = new QVBoxLayout;
    aboutTabLayout->addLayout(infoLayout);
    aboutTabLayout->addWidget(changelogTextEdit);
    aboutTabLayout->addWidget(showCreditsButton);
    aboutTabLayout->addWidget(openWebsiteButton);
    aboutTabLayout->addWidget(updateCheckButton);
    aboutTabLayout->addWidget(helpButton);
    aboutTab->setLayout(aboutTabLayout);

    setCentralWidget(mainTabWidget);                            // finish the layout *****

    // OTHER INIT FUNCTIONS
    setMinimumSize(300,400);                                    // set min-window size
    readSettings();                                             // read settings file
    find();                                                     // update file-table
    createActions();                                            // create our action
    createMenus();                                              // menu stuff
    resetNoteContent();                                         // disable noteContent
    generateStats();                                            // Gebnerate stats on app launch
    setUnifiedTitleAndToolBarOnMac(true);                       // test - Mac-specific ...well we dont have a toolbar right now but who cares
    magicField->setFocus();                                     // jump to most important gui-item after start
    statusBar()->showMessage("notNV finished init", 2000);      // init-update statusbar with first message
}


// *************************************
// CONSTRUCTION AREA
// *************************************



// *************************************
// INIT RELATED
// *************************************

//  name:           createActions()
// function:        - create the menuactions
void MainWindow::createActions()
{
    // FILE ACTIONS
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    reloadAct = new QAction(tr("Reload files"), this);
    //reloadAct->setShortcuts(QKeySequence::Refresh);
    reloadAct->setStatusTip(tr("Reload files"));
    reloadAct->setEnabled(true);
    connect(reloadAct, SIGNAL(triggered()), this , SLOT(find()));
    // NOTE MENU ACTIONS
    deleteNoteAct = new QAction(tr("Delete"), this);
    deleteNoteAct->setStatusTip(tr("Deletes the selected note"));
    deleteNoteAct->setShortcuts(QKeySequence::Delete);
    deleteNoteAct->setEnabled(true);
    connect(deleteNoteAct, SIGNAL(triggered()), this, SLOT(deleteNote()));

    renameAct = new QAction(tr("Rename"), this);
    renameAct->setStatusTip(tr("Rename selected file"));
    renameAct->setEnabled(true);
    connect(renameAct, SIGNAL(triggered()), this, SLOT(renameNote()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("Save"), this);
    //saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(saveNote()));

    // HELP ACTIONS
    helpAct = new QAction(tr("Help"), this);
    helpAct->setStatusTip(tr("Opens notNV online help"));
    helpAct->setShortcut(QKeySequence::HelpContents);
    connect(helpAct, SIGNAL(triggered()), this, SLOT(openHelp()));

    updateAct = new QAction(tr("Update Check"), this);
    updateAct->setStatusTip(tr("Checks for software update"));
    connect(updateAct, SIGNAL(triggered()), this, SLOT(checkForUpdates()));
}

// name:            createMenus()
// function:        - create menubar items
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(reloadAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    notesMenu = menuBar()->addMenu(tr("&Note"));
    notesMenu->addAction(renameAct);
    notesMenu->addAction(saveAct);
    notesMenu->addAction(deleteNoteAct);


    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAct);
    helpMenu->addSeparator();
    helpMenu->addAction(updateAct);
}


// *************************************
// UPDATE STUFF
// *************************************

//  name:           noteContentChanged()
// function:        - is triggered whenever content of noteContent is changed
//                  - checks if file was loaded & edited afterwards
//                  - if so : enables the save button
void MainWindow::noteContentChanged()
{
    if((noteChangeStatus->text() == "changed") & (enableInAppEditingCheckbox->isChecked()))
    {
        saveCurrentNewNoteButton->setHidden(false);                 // show it
        noteContent->setTextColor(QColor("red"));
        //noteContent->setTextBackgroundColor(QColor("lightGray"));
    }
}

//  name:           generateStats()
// function:        - update the stats-tab with stats-data of selected notes folder
void MainWindow::generateStats()
{
    QString path = directoryComboBox->currentText();                                                        // get combobox-text/path - current working dir
    currentDir = QDir(path);
    QString fileName = "*.txt";
    QStringList files;                                                                                      // get list of filesnames in directory
    files = currentDir.entryList(QStringList(fileName),QDir::Files | QDir::NoSymLinks);
    int overallFileNameLength = 0;
    int shortestFileName = 1000;
    int largestFileName = 0;
    int averageTitleLength = 0;
    int overallLines = 0;
    int averageLinesPerNote = 0;
    int overallChars = 0;
    int averageChars = 0;

    for (int i = 0; i < files.size(); ++i)                                                                  // for each file/filename in Stringlist 'files' do
    {
        int k = files[i].length() -4;   // count length of name  - 4 = .extension handling .txt

        if(k >= largestFileName)             // largest
        {
            largestFileName = k;
        }

        if(k <= shortestFileName)               // shortest
        {
            shortestFileName = k;
        }

        overallFileNameLength = overallFileNameLength + k;          // calculate overall title length

        QFile file(currentDir.absoluteFilePath(files[i]));                                              // define file
        if (file.open(QIODevice::ReadOnly))                                                             // open file and start digging checking each line
        {
            QString line;
            QTextStream in(&file);
            while (!in.atEnd())
            {
                line = in.readLine();
                overallLines = overallLines +1;                     // lines + 1
                overallChars = overallChars + line.length();        // count chars
            }
        }
    }

    // calculate the things we need to calculate
    averageChars = overallChars/files.size();
    averageLinesPerNote = overallLines/files.size();
    averageTitleLength = overallFileNameLength / files.size();

    // Update the stats-tab
    statsNotesCounterResultLabel->setText(QString::number(files.size()));
    statsAverageNoteTitleLengthResultLabel->setText(QString::number(averageTitleLength));
    statsLargestTitleResultLabel->setText(QString::number(largestFileName));
    statsShortestResultTitle->setText(QString::number(shortestFileName));
    statsAverageCharsResultTitle->setText(QString::number(averageChars));
    statsAverageLinesResultTitle->setText(QString::number(averageLinesPerNote));
    statsOverallLinesResultTitle->setText(QString::number(overallLines));
    statsOverallCharsResultTitle->setText(QString::number(overallChars));

    //update timestamp at stats tab
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString();
    //statsGenerationTimeLabel->setText("Generated at: " + dateTimeString);
    statsGenerationTimeLabel->setText(dateTimeString);

    updateStatusBar("Finished calculating.");
}


// name:            resetNoteContent
// function:        - resets noteContent back to defaults
void MainWindow::resetNoteContent()
{
    noteContent->clear();
    noteContent->setTextColor(QColor("gray"));
    noteContent->setText("No Note Selected");
    noteContent->setAlignment (Qt::AlignCenter);
    noteContent->setEnabled(false);
    saveCurrentNewNoteButton->setHidden(true);             // hide save button again
}

// name:            showMenuBar()
// function         show or hides menubar
void MainWindow::showMenuBar()
{
    if(showMenuCheckbox->isChecked())               // check if checked or unchecked
    {
        menuBar()->show();
        updateStatusBar("Menubar displayed");
    }
    else
    {
        menuBar()->hide();
        updateStatusBar("Menubar hidden");
    }
}

// name:            Window::showFiles
// function:        - is called by 'find()'
//                  - gets file-selection matching current search from 'findFiles()'
//                  - updates the file table with fileselection
//                  - updates the file-counter at app-bottom
void MainWindow::showFiles(const QStringList &files)
{    
    for (int i = 0; i < files.size(); ++i)                                                                  // for all files
    {
        //qDebug() << files[i];
        QRegExp sep(",");
        //qDebug() << files[i].section(sep, 0, 0);        // filename
        //qDebug() << files[i].section(sep, 1);           // relevancy

        QFile file(currentDir.absoluteFilePath(files[i].section(sep, 0, 0)));
        qint64 size = QFileInfo(file).size();
        QDateTime dateModified = QFileInfo(file).lastModified();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i].section(sep, 0, 0));                                    // filename
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));      // filesize
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem *dateItem = new QTableWidgetItem(tr("%1").arg(dateModified.toString("yyyyMMdd.HHmm")));           // date-modified
        dateItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        dateItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem *relevancyItem = new QTableWidgetItem(files[i].section(sep, 1));           // date-modified
        relevancyItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        relevancyItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = filesTable->rowCount();

        // update table with entry
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);

        QStringList labels;
        if(files[i].section(sep, 1) ==  "")                 // showing all files ->date based
        {
            filesTable->setItem(row, 1, dateItem);
            labels << tr("Name") << tr("Last modified");
            filesTable->setHorizontalHeaderLabels(labels);
        }
        else                                                // showing filtered list of files including relevancy info
        {
            filesTable->setItem(row, 1, relevancyItem);
            labels << tr("Name") << tr("Relevancy");
            filesTable->setHorizontalHeaderLabels(labels);
        }
        filesTable->sortItems(1, Qt::DescendingOrder );
    }

    // just 1 note - we should select it right away & load the note to noteContent
    if (files.size() == 1)
    {
        filesTable->selectRow(0);                                                                           // select row0
        loadFileToNotesView();                                                                              // load note to noteContent
    }
    filesFoundLabel->setText(tr("%1 file(s) found").arg(files.size()));                                     // Update number of files we found at bottom of app window
}



// name:            Window::openFileOfItem(int row, int column)
// function:        - opens a selected file in default editor
void MainWindow::openFileOfItem(int row, int /* column */)
{
    QTableWidgetItem *item = filesTable->item(row, 0);                                                      // get item
    QDesktopServices::openUrl(QUrl::fromLocalFile(currentDir.absoluteFilePath(item->text())));              // open file
}

// name:                magicChanged(const QString&)
// function:            - is executed if user inputs something into searchfield
//                      - enables & disables the new-note button
void MainWindow::magicChanged(const QString&)
{
    find();                 // execute find to update file-table
    QString tempO = magicField->text();
    if(tempO.length() > 0)
    {
        createNewNoteButton->setEnabled(true);
    }
    else
    {
        createNewNoteButton->setEnabled(false);
    }
}

// name:            loadFiletoNotesView
// function:        - is executed if user select a row in our table
//                  - loads the content of the selected file to noteContent
// todo:            maybe we do include somekind of notes-editing - in such acase we would need to enable the notecontent field ;)
void MainWindow::loadFileToNotesView()
{
    noteChangeStatus->setText("ok");

    noteContent->setEnabled(true);                                  // enable notecontent - handling read/write comes later
    saveCurrentNewNoteButton->setHidden(true);                     // show save button - as we loaded a note

    int curRow = filesTable->currentRow();
    if(curRow != -1)                                                // only continue if we got a selected row
    {
        QString value = filesTable->item(curRow,0)->text();
        QFile f(currentDir.absoluteFilePath(value));
        noteContent->setText("");                                   // reset noteContent
        noteContent->setTextColor(QColor("black"));

        if (f.open(QIODevice::ReadOnly | QIODevice::Text))          // open file
        {
            QTextStream in(&f);
            QString line = in.readLine();
            while (!line.isNull())
            {
                noteContent->append(line);                          // append to our notecontent field
                line = in.readLine();
            }

            noteChangeStatus->setText("changed");                   // UGLY UGLY ... its all about realizing when a note changed

            QTextCursor tc = noteContent->textCursor();
            tc.setPosition(0,QTextCursor::MoveAnchor);
            tc.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor,2);
            noteContent->setTextCursor(tc);                         // set cursor to line 3

            noteContent->verticalScrollBar()->setValue(0);          // scroll up
            noteContent->ensureCursorVisible();                     // ensure the cursor is visble - right now at last line might conflict with line above
        }

        if(enableInAppEditingCheckbox->isChecked())                 // enable or disable inAppEditing
        {
            noteContent->setReadOnly(false);                        // enable inAppEditing
            saveCurrentNewNoteButton->setEnabled(true);             // enable save-button if it makes sense
        }
        else
        {
            noteContent->setReadOnly(true);                         // disable inAppEditing
        }
        f.close();
    }
    else
    {
        resetNoteContent();                                         // start: resetNoteContent()
    }
}

// name:            updateComboBox
// function:        -
static void updateComboBox(QComboBox *comboBox)
{
    if (comboBox->findText(comboBox->currentText()) == -1)
        comboBox->addItem(comboBox->currentText());
}


// *************************************
// USER INPUT
// *************************************

// name:            smallFontForFileList()
// function         - enables small font in file-list
void MainWindow::smallFontForFileList()
{
    QFont fnt;
    fnt.setFamily("Helvetica");
    if(smallFontsCheckbox->isChecked())               // check if checked or unchecked
    {
        smallFontsCheckbox->setChecked(true);
        fnt.setPointSize(8);
        updateStatusBar("Small fonts activated");
    }
    else
    {
        smallFontsCheckbox->setChecked(false);
        fnt.setPointSize(11);
        updateStatusBar("Small fonts disabled");
    }
    filesTable->setFont(fnt);
}

// name:            keyPressedEvents
// function:        - catching keyPressedEvents
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat() )
    {
        //qDebug() << "keyPressEvent ignore";
        event->ignore();
    }
    else
    {
        //qDebug() << "keyPressEvent accept";
        event->accept();

        // we might need to know which widget has focus
        QWidget *widget = focusWidget();
        //qDebug() << focusWidget();

        switch (event->key())
        {
            case Qt::Key_F1:                    // KeyPress F1 - open notnv wiki & dialog that there is no help
            {
                openHelp();
                break;
            }
            case Qt::Key_F2:                    // KeyPress F2 - rename note
            {
                renameNote();
                break;
            }
            case Qt::Key_F5:                    // KeyPress F5 - reloading files
            {
                find();
                break;
            }
            case Qt::Key_Delete:                // KeyPress DEL - delete selected note
            {
                deleteNote();
                break;
            }
            case Qt::Key_Down:                // KeyPress Down - function: jumps from magicField to Table & redefined focus
            {
                if (widget && widget->inherits("QLineEdit"))    // pressed in magicField
                {
                    magicField->setPlaceholderText("search or create magic");   // set placeholder for magic field
                    createNewNoteButton->setEnabled(false);                     // disable new note button
                    filesTable->selectRow(0);                                   // select row in file list
                    filesTable->setFocus();                                     // change focus to file-list
                    qDebug() << "the Mac test";
                }
                break;
            }
            case Qt::Key_Escape:                // function: unselects selected row, cleans magicField & noteContent
            {
                if (widget && widget->inherits("QLineEdit"))    // pressed in magicField
                {
                    magicField->clear();
                    magicField->setFocus();
                    find();
                }

                else if (widget && widget->inherits("QTableWidget")) // pressed in file-list/table
                {
                    filesTable->clearSelection();           // clear table selection
                    filesTable->clearFocus();               // still not really cleared somehow - try deleting shortcut
                    resetNoteContent();                     // modify noteContent
                    magicField->setFocus();                 // jump mouse curosr to search field
                }


                else if (widget && widget->inherits("QTextEdit")) // pressed in noteContent - jump bakc to filelist
                {
                    filesTable->setFocus();                 // jump mouse curosr to search field
                }

                else
                {
                    magicField->setFocus();
                }

                break;
            }
        }
    }
}


//  name:           checkForUpdates()
// function:        - checks for app updates
// todo:            - everything
void MainWindow::checkForUpdates()
{
    QUrl url("http://macfidelity.github.com/notNV/current_notNV.xml");
    QString filename = QFileInfo(url.path()).fileName();

    // check if local file exists already
    if(QFile::exists(filename))
    {
      if(QMessageBox::question(this, tr("Update Check"),
       tr("There already exists a file called %1 in "
        "the current directory. Overwrite?").arg(filename),QMessageBox::Yes|QMessageBox::No, QMessageBox::No)== QMessageBox::No)
        return;
      QFile::remove(filename);
    }

    // download file using QNetworkAccessManager
    QNetworkAccessManager* m_NetworkMngr = new QNetworkAccessManager(this);
    QNetworkReply *reply= m_NetworkMngr->get(QNetworkRequest(url));
    QEventLoop loop;
    connect(reply, SIGNAL(finished()),&loop, SLOT(quit()));
    loop.exec();
    QUrl aUrl(url);
    QFileInfo fileInfo=aUrl.path();
    QFile file(fileInfo.fileName());
    file.open(QIODevice::WriteOnly);
    file.write(reply->readAll());
    file.close();
    delete reply;

    // we need to parse the xml file now and see if our version is up-to-date
    QDomDocument doc( "notNV_version" );
    if( !file.open(QIODevice::ReadOnly ) )
    {
      //
    }
    if( !doc.setContent( &file ) )
    {
      file.close();
    }
    //file.close();

    QDomElement root = doc.documentElement();
    if( root.tagName() != "current" )
    {
        //
    }

    QString foobar;
    QString foobar2;
    QString foobar3;
    QString foobar4;

    QDomNode n = root.firstChild();
    while( !n.isNull() )
    {
        QDomElement e = n.toElement();
      if( !e.isNull() )
      {
        if( e.tagName() == "stable" )
        {
            foobar = e.attribute( "name", "" );
            foobar2 = e.attribute( "version", "" );
            foobar3 = e.attribute( "builddate", "" );
            foobar4 = e.attribute( "counter", "" );
        }
      }
      n = n.nextSibling();
    }

    QFile::remove(filename);                        // remove the xml file

    QString localVersion = versionField->text();
    QString latestStable = foobar2+foobar3+foobar4;

    QMessageBox currentStableMessageBox;

    if(localVersion == latestStable)                // current stable
    {
        currentStableMessageBox.setText("You are running the latest stable milestone");
    }
    if(localVersion < latestStable)                 // old version
    {
        currentStableMessageBox.setText("There is a new milestone of notNV available");
    }
    if(localVersion > latestStable)                 // version > latest stable milestone
    {
        currentStableMessageBox.setText("You are running a developer version");
    }
    currentStableMessageBox.setIcon(QMessageBox::Information);
    currentStableMessageBox.exec();

    updateStatusBar("Finished update check.");
}



// name:            browse()
// function:        - is executed on 'Browse..' button
//                  - opens a dialog to select a folder
void MainWindow::browse()
{
    QSettings settings("notNV", "notNV");                                                                     // Settings
    QString directory;                                                                                          // new string for our setting

    // display dialog to user to select a folder - starting points depends on the fact if user had selected one folder before or not
    if (settings.contains("storageLocationFolder"))                                                             // storage location
    {
        QString defPath =settings.value("storageLocationFolder").toString();                                    // get stored Location folder from config file
        directory = QFileDialog::getExistingDirectory(this, tr("notNV - select notes folder"), defPath);        // open file dialog at current place
    }
    else
    {
        directory = QFileDialog::getExistingDirectory(this, tr("notNV - select notes folder"), QDir::currentPath());    // ther is no folder stored in our settings file - so lets start from current dir
    }

    // check input
    if (!directory.isEmpty())                                                                                   // if user selected a folder
    {
        if (directoryComboBox->findText(directory) == -1)
        {
            directoryComboBox->addItem(directory);
        }
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
        writeSettings();                                                                                        // call write settings hack
        selectedStorageLocationLabel->setText(directory);
        updateStatusBar("Notes folder selected.");
    }
    else
    {
        updateStatusBar("Notice: you canceled the notes folder selection.");
    }
    find();                                                                                                     // get updated table/filelist
}


// name:        Window::openHelp()
// function:    - opens  github wiki in default browser
void MainWindow::openHelp()
{
    QDesktopServices::openUrl(QUrl("https://github.com/macfidelity/notNV/wiki", QUrl::TolerantMode));
    updateStatusBar("Launched online help.");
}

//  name:           showCredits()
// function:        - opens credits file at github
void MainWindow::showCredits()
{
    QDesktopServices::openUrl(QUrl("https://github.com/macfidelity/notNV/blob/master/doc/CREDITS", QUrl::TolerantMode));
    updateStatusBar("Opened CREDITS file in default browser.");
}

//  name:           visitWebsite()
// function:        - opens project page in default browser
void MainWindow::visitWebsite()
{
    QDesktopServices::openUrl(QUrl("http://macfidelity.github.com/notNV/", QUrl::TolerantMode));
    updateStatusBar("Opened project page in default browser.");
}



// *************************************
// INIT-RELATED THINGS
// *************************************

// name:        createComboBox()
// function:    - creates a combobox
QComboBox *MainWindow::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}

// name:        Window::createFilesTable()
// function:    - define the file-table
void MainWindow::createFilesTable()
{
    // header v1
    QStringList labels;
    labels << tr("Name") << tr("Last modified");

    filesTable = new QTableWidget(0, 2);                                                                    // configure table
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);                                        // always select entire row
    filesTable->setSelectionMode(QTableView::SingleSelection);                                              // allow only single selection
    filesTable->setHorizontalHeaderLabels(labels);                                                          // header
    filesTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);                                 // all the columns are sized to show all contents & column 0 is stretched - fills remaining space
    filesTable->verticalHeader()->hide();
    filesTable->setAlternatingRowColors(true);                                                              // alt table-row colors
    filesTable->setShowGrid(true);                                                                          // show grid
    filesTable->setToolTip("Displays list of filtered notes.");    
    filesTable->setWhatsThis("The file-table");
    filesTable->setSortingEnabled(true);
    filesTable->setTabKeyNavigation(false);                                                                 // we dont want to tab from cell to cell - strange default value
    //filesTable->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(filesTable, SIGNAL(cellActivated(int,int)),this, SLOT(openFileOfItem(int,int)));                // connect openFile for external usage
    connect(filesTable, SIGNAL(itemSelectionChanged()),this, SLOT(loadFileToNotesView()));                  // connect openFile to display it inside notNV
}

// *************************************
// USER WORKS WITH NOTES
// *************************************

//  name:           saveNote()
// function:        - saves current selected note
void MainWindow::saveNote()
{
    int curRow = filesTable->currentRow();
    if(curRow != -1)                                                // only continue if we got a selected row
    {
        QString value = filesTable->item(curRow,0)->text();
        QFile f(currentDir.absoluteFilePath(value));

        // get current text from noteContent
        QString fubar = noteContent->toPlainText();

        if (f.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            // reset current content of file
            QTextStream out(&f);
            out<<fubar;
            f.close();

            saveCurrentNewNoteButton->setHidden(true);             // hide save button

            loadFileToNotesView();                              // reload note to fix font color

            updateStatusBar("Note "+value+ " saved and reloaded.");
        }
        else
        {
            qDebug() << "was not able to save changes";
        }
    }
}


// name:            renameNote()
// function:        - displays dialog to user to rename selected note
//void MainWindow::renameNote(int row, int /* column */)
void MainWindow::renameNote()
{
    int curRow = -1;
    curRow = filesTable->currentRow();

    if (curRow > -1)                        // if note is selected - try to rename it
    {
        QString fileName = filesTable->item(curRow,0)->text();         // get filename
        QFile file(currentDir.absoluteFilePath(fileName));          // define file

        bool ok;
        QString text = QInputDialog::getText(this, tr("Rename note"),tr("Filename:"), QLineEdit::Normal,fileName, &ok);
        if (ok && !text.isEmpty())
        {
            QString newFileName = text;
            qDebug() << newFileName;

            QFile newFile(currentDir.absoluteFilePath(newFileName));          // define file
            if (newFile.exists())
            {
                updateStatusBar("Warning: Filename is already in use.");
            }
            else
            {
                bool rename = file.rename(currentDir.absoluteFilePath(newFileName));
                if(rename == true)
                {
                    updateStatusBar("Note " +fileName+ " renamed to " +newFileName+ ".");
                }
                else
                {
                    updateStatusBar("Renaming note " +fileName+ " failed.");
                }
                find();
            }
        }
        else
        {
            updateStatusBar("Notice: Invalid input.");
        }
    }
}

// name:            deleteNote()
// function:        - displays dialog if user is sure to delete selected note
//                  - tries to delete the selected note
void MainWindow::deleteNote()
{
    int curRow = -1;
    curRow = filesTable->currentRow();

    if (curRow > -1)                        // if note is selected - try to rename it
    {
        // check status of ui-dialog
        if(enableConfirmOnDeleteCheckbox->isChecked())
        {
            int ret = QMessageBox::question(this, tr("Delete Note"),tr("Do you really want to delete this note?"),QMessageBox::No,QMessageBox::Yes);        // ask if user is sure
            if(ret == QMessageBox::Yes)                                                                                                                     // if answer = yes
            {
                QString fileName = filesTable->item(curRow,0)->text();                                                                                         // need filename - so content of column 1
                QFile file(currentDir.absoluteFilePath(fileName));                                                                                          // define file

                bool delProc =  file.remove();                                                                                                              // delete file
                if(delProc == false)
                {
                    updateStatusBar("Could not delete selected note.");
                }
                else
                {
                    updateStatusBar("Note " +fileName+ " deleted.");
                    find();                                                                                                                                 // update filesTable

                    if(filesTable->rowCount() > curRow)                                                                                                        // select new row after having deleted one
                    {
                        filesTable->selectRow(curRow);                                                                                                         // select new row after deleting one note
                    }
                    // CONSTRUCTION AREA
                    else
                    {
                        if(filesTable->rowCount() == 0)
                        {
                            // select nothing
                        }
                    }
                }
            }
        }
        else
        {
            QString fileName = filesTable->item(curRow,0)->text();                                                                                         // need filename - so content of column 1
            QFile file(currentDir.absoluteFilePath(fileName));                                                                                          // define file

            bool delProc =  file.remove();                                                                                                              // delete file
            if(delProc == false)
            {
                updateStatusBar("Could not delete selected note.");
            }
            else
            {
                updateStatusBar("Note " +fileName+ " deleted.");
                find();                                                                                                                                 // update filesTable

                if(filesTable->rowCount() > curRow)                                                                                                        // select new row after having deleted one
                {
                    filesTable->selectRow(curRow);                                                                                                         // select new row after deleting one note
                }
            }
        }
    }
}

// name:            createNewNote()
// function:        - creates a new note
// todo             cleaning the bloody mess
void MainWindow::createNewNote()
{
    QString newNoteName = magicField->text();                                                               // get string entered by user
    if(newNoteName.length()>0)
    {
        newNoteName = newNoteName+".txt";
        QSettings settings("notNV", "notNV");
        if (settings.contains("storageLocationFolder"))
        {        
            QDir notNVStoragePath;                                                                          // Get storage path
            notNVStoragePath.setPath(settings.value("storageLocationFolder").toString());

            QStringList files;                                                                              // get list of filesnames in directory
            files = currentDir.entryList(QStringList(newNoteName),QDir::Files | QDir::NoSymLinks);
            if(files.contains(newNoteName))                                                                 // check if filename exists already ;)
            {     
                updateStatusBar("Notice: There is already a note with that name.");
            }
            else
            {
                QFile f(newNoteName);                                                                       // try to create it
                QDir currentDir = QDir::current();
                currentDir.setCurrent(settings.value("storageLocationFolder").toString());

                if (f.open(QIODevice::WriteOnly | QIODevice::Text))                                         // write to file if possible
                {
                    QTextStream streamToWrite(&f);
                    QString newContent = newNoteName+"\n################################";
                    streamToWrite<<newContent;
                    f.close();

                    updateStatusBar("New note "+newNoteName+ " created.");
                }
                else
                {
                    updateStatusBar("Could not create new note.");
                }
                find();                                                                                     // update table
            }
        }
    }
    else
    {
        updateStatusBar("Can't' create a new note without name.");
    }
}


// *************************************
// FIND STUFF
// *************************************

// name:        Window::find()
// function:    -
void MainWindow::find()
{
    filesTable->setRowCount(0);                                                                             // resetTable rowcount
    QString path = directoryComboBox->currentText();                                                        // get combobox-text
    QString newText = magicField->text();
    updateComboBox(directoryComboBox);                                                                      // run updateComboBox
    currentDir = QDir(path);
    QString fileName = "*.txt";
    QStringList files;                                                                                      // get list of filesnames in directory
    files = currentDir.entryList(QStringList(fileName),QDir::Files | QDir::NoSymLinks);

    if (!newText.isEmpty())                                                                                 // is empty on app launch
    {
        files = findFiles(files, newText);                                                                  // call findFiles to check for files containing our searchword
    }
    showFiles(files);                                                                                       // update table

    updateStatusBar("Updated file list.");
}


// name:        Window::findFiles
// function:    - is called by 'find' and
//              - searches if searchword is can be found inside filenames
//              - searches if searchword is contained inside a our files
//              - returns a list of filesnames which do contain the searchword in name or content
QStringList MainWindow::findFiles(const QStringList &files, const QString &text)
{
    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("notNV is searching"));

    QStringList foundFiles;                                                                                 // define new list for hits
    QStringList relevancyList;
    int fileRelevancy = 0;

    for (int i = 0; i < files.size(); ++i)                                                                  // for each file/filename in Stringlist 'files' do
    {
        fileRelevancy = 0;

        progressDialog.setValue(i);                                                                         // progress dialog
        progressDialog.setLabelText(tr("Searching file number %1 of %2...").arg(i).arg(files.size()));
        qApp->processEvents();

        if (progressDialog.wasCanceled())
            break;

        // check if filename contains or search already
        // case sensitivity is not needed here - we need an uppercase hack for text
        QString bla = text;
        bla = bla.toUpper();
        if(files[i].toUpper().contains(bla))
        {
            // we got a hit in title
            foundFiles << files[i];                                                                         // if so - add filename to hit-list
            //relevancyList << fileRelevancy[1];
            fileRelevancy = fileRelevancy +2;

            // search inside note as well
            QFile file(currentDir.absoluteFilePath(files[i]));                                              // define file

            if (file.open(QIODevice::ReadOnly))                                                             // open file and start digging checking each line
            {
                QString line;
                QTextStream in(&file);
                while (!in.atEnd())
                {
                    if (progressDialog.wasCanceled())
                        break;
                    line = in.readLine();

                    if (line.toUpper().contains(bla))                                                       // check if current line contains or searchword
                    {
                        fileRelevancy = fileRelevancy +1;
                    }
                }
                file.close();
            }
        }
        // filename didnt contain our search - so check inside the file to be sure
        else
        {
            QFile file(currentDir.absoluteFilePath(files[i]));                                              // define file

            if (file.open(QIODevice::ReadOnly))                                                             // open file and start digging checking each line
            {
                QString line;
                QTextStream in(&file);
                while (!in.atEnd())
                {
                    if (progressDialog.wasCanceled())
                        break;
                    line = in.readLine();

                    if (line.toUpper().contains(bla))                                                       // check if current line contains or searchword
                    {
                        if(foundFiles.contains(files[i]))
                        {
                           fileRelevancy = fileRelevancy +1;
                           break;
                        }
                        else
                        {
                            foundFiles << files[i];
                            fileRelevancy = fileRelevancy +1;
                        }
                    }
                }
                file.close();
            }
        }
        // output if relevant
        if(fileRelevancy > 0)
        {
            //qDebug() << fileRelevancy;
            QString blablub = (QString::number(fileRelevancy));
            relevancyList << blablub;
        }
    }

    QStringList damn;
    for (int i = 0; i < relevancyList.size(); ++i)                                                                  // debug output of relevancylist - we need return that shit and use it to fillthe tablecolumn
    {
        damn << foundFiles[i]+ ","+relevancyList[i];
    }

    //return foundFiles;                                                                                      // return result
    return damn;
}



// *************************************
// STATUSBAR-RELATED
// *************************************

// name:            updateStatusBar()
// function:        - add text to statusbar for a limited time
void MainWindow::updateStatusBar(QString updateMessage)
{
    statusBar()->showMessage(updateMessage, 2000);                                                                // update the text of statusbar
}

// *************************************
// SETTINGS
// *************************************

// name:        readSettings
// function:    - read values from config file (pos, size, storageLocationFolder)
void MainWindow::readSettings()
{
    QSettings settings("notNV", "notNV");                                                 // Settings
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();                         // Window-Position
    QSize size = settings.value("size", QSize(450, 600)).toSize();                          // Window-Size
    resize(size);                                                                           // resize window
    move(pos);                                                                              // move window

    // storageLocationFolder value?
    if (settings.contains("storageLocationFolder"))                                         // storage location
    {
        directoryComboBox->clear();                                                         // clear box
        directoryComboBox->addItem(settings.value("storageLocationFolder").toString());     // set folder from settings to comboxbox
        selectedStorageLocationLabel->setText(settings.value("storageLocationFolder").toString());
    }
    else
    {
        QMessageBox undefiniedStorageFolderBox;
        undefiniedStorageFolderBox.setText("Seems like you are using notNV the first time.<br><br>"
                                           "Before you can start using notNV<br>"
                                           "you have to define a storage folder.<br><br>");
        undefiniedStorageFolderBox.setIcon(QMessageBox::Critical);
        undefiniedStorageFolderBox.exec();

        browse();
    }

    // launchCounter
    int newLaunchCounter = 0;
    if (settings.contains("launchCounter"))                                    // Setting: showMenu
    {
        newLaunchCounter = settings.value("launchCounter").toInt() +1;
        settings.setValue("launchCounter", newLaunchCounter);
    }
    else
    {
        newLaunchCounter = 1;
        settings.setValue("launchCounter", 1);                                  // first launch - add it with a starting value of 1
    }
    settings.setValue("launchCounter", newLaunchCounter);
    appUsageCounterTitleResult->setText(QString::number(newLaunchCounter));


    // showMenu
    if (settings.value("showMenu") == 0)
    {
        showMenuCheckbox->setCheckState(Qt::Unchecked);
    }
    else
    {
        showMenuCheckbox->setCheckState(Qt::Checked);
    }
    showMenuBar();


    // Setting: smallFont
    QFont fnt;
    fnt.setFamily("Helvetica");
    if (settings.value("smallFont") == 0)
    {
        smallFontsCheckbox->setChecked(false);
        fnt.setPointSize(11);
    }
    if (settings.value("smallFont") == 1)
    {
        smallFontsCheckbox->setChecked(true);
        fnt.setPointSize(8);
    }
    filesTable->setFont(fnt);

    // Setting: enableDeleteConfirmDialog
    if (settings.value("enableDeleteConfirmDialog") == 0)
    {
        enableConfirmOnDeleteCheckbox->setChecked(false);
    }
    if (settings.value("enableDeleteConfirmDialog") == 1)
    {
        enableConfirmOnDeleteCheckbox->setChecked(true);
    }

    // Settings: enableInAppEditing
    if(settings.value("enableInAppEditing") == 0)
    {
        enableInAppEditingCheckbox->setChecked(false);
    }
    if(settings.value("enableInAppEditing") == 1)
    {
        enableInAppEditingCheckbox->setChecked(true);
    }

    updateStatusBar("Settings read.");
}


// name:            writeSettings
// function:        - write settings to config file (pos, size, storageLocationFolder)
void MainWindow::writeSettings()
{
    QSettings settings("notNV", "notNV");
    settings.setValue("pos", pos());                                                                    // position
    settings.setValue("size", size());                                                                  // size
    settings.setValue("storageLocationFolder", directoryComboBox->currentText());                       // storageLocation

    if(showMenuCheckbox->isChecked())                                                                   // get showMenu checkbox state
    {
        settings.setValue("showMenu", 1);
    }
    else
    {
        settings.setValue("showMenu", 0);
    }

    if (smallFontsCheckbox->isChecked())                                                                // small fonts
    {
        settings.setValue("smallFont", 1);
    }
    else
    {
        settings.setValue("smallFont", 0);
    }

    if(enableConfirmOnDeleteCheckbox->isChecked())                                                      // enableDeleteConfirmDialog
    {
        settings.setValue("enableDeleteConfirmDialog", 1);
    }
    else
    {
        settings.setValue("enableDeleteConfirmDialog", 0);
    }

    if(enableInAppEditingCheckbox->isChecked())                                                      // enableInAppEditing
    {
        settings.setValue("enableInAppEditing", 1);
    }
    else
    {
        settings.setValue("enableInAppEditing", 0);
    }

    updateStatusBar("Settings written.");
}
