#ifndef WINDOW_H
#define WINDOW_H

#include <QDialog>
#include <QDir>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QTextEdit;
class QSplitter;
class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QLineEdit;
class QHBoxLayout;
class QCheckBox;
class QHeaderView;
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void browse();
    void find();
    void openFileOfItem(int row, int column);
    void keyPressEvent(QKeyEvent *event);
    void magicChanged(const QString&);
    void loadFileToNotesView();
    void createNewNote();
    void writeSettings();
    void openHelp();
    void updateStatusBar(QString);
    void showMenuBar();
    void smallFontForFileList();
    void renameNote();
    void deleteNote();
    void generateStats();
    void saveNote();
    void noteContentChanged();
    void showCredits();
    void visitWebsite();
    void checkForUpdates();

private:
    QStringList findFiles(const QStringList &files, const QString &text);
    void showFiles(const QStringList &files);
    QComboBox *createComboBox(const QString &text = QString());
    void createFilesTable();
    void readSettings(); 
    void resetNoteContent();
    void createActions();
    void createMenus();

    QNetworkAccessManager *m_NetworkMngr;
    QString *appStableVersion;

    // monitors notestatus - most likely the ugliest hack in this project so far
    QLabel *noteChangeStatus;

    // Menus
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *notesMenu;
    QMenu *helpMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *helpAct;
    QAction *reloadAct;
    QAction *deleteNoteAct;
    QAction *renameAct;
    QAction *updateAct;

    QTabWidget *mainTabWidget;

    // Tab 0 - notes
    QLineEdit *versionField;
    QLineEdit *magicField;
    QPushButton *createNewNoteButton;
    QComboBox *directoryComboBox;
    QTableWidget *filesTable;
    QHeaderView *headerView;
    QTextEdit *noteContent;
    QSplitter *notesSplitter;
    QDir currentDir;
    QLabel *filesFoundLabel;
    QPushButton *saveCurrentNewNoteButton;
    // Tab 1 - conf/settings
    QLabel *chooseStorageLocationButtonLabel;
    QLabel *selectedStorageLocationLabel;
    QPushButton *chooseStorageLocationButton;
    QCheckBox *showMenuCheckbox;
    QCheckBox *smallFontsCheckbox;
    QCheckBox *enableConfirmOnDeleteCheckbox;
    QCheckBox *enableInAppEditingCheckbox;
    QPushButton *saveButton;
    // Tab2 - Shortcuts
    QLabel *escShortcutLabel;
    QLabel *returnShortcutLabel;
    QLabel *arrowDownShortcutLabel;
    QLabel *arrowUpShortcutLabel;
    QLabel *delShortcutLabel;
    QLabel *tabShortcutLabel;
    QLabel *f1ShortcutLabel;
    QLabel *f2ShortcutLabel;
    QLabel *f5ShortcutLabel;
    // tab3 - stats
    QPushButton *statsButton;
    QLabel *statsNotesCounterLabel;
    QLabel *statsNotesCounterResultLabel;
    QLabel *statsAverageNoteTitleLengthLabel;
    QLabel *statsAverageNoteTitleLengthResultLabel;
    QLabel *statsShortestTitle;
    QLabel *statsShortestResultTitle;
    QLabel *statsLargestTitleLabel;
    QLabel *statsLargestTitleResultLabel;
    QLabel *statsAverageLinesTitle;
    QLabel *statsAverageLinesResultTitle;
    QLabel *statsAverageCharsTitle;
    QLabel *statsAverageCharsResultTitle;
    QLabel *statsOverallLinesTitle;
    QLabel *statsOverallLinesResultTitle;
    QLabel *statsOverallCharsTitle;
    QLabel *statsOverallCharsResultTitle;
    QLabel *statsGenerationTimeLabel;
    QLabel *appUsageCounterTitle;
    QLabel *appUsageCounterTitleResult;
    QLabel *statsSpacer;
    // Tab4 - about/Changelog
    QLabel *appInfoLabel;
    QHBoxLayout *infoLayout;
    QPushButton *helpButton;
    QPushButton *showCreditsButton;
    QPushButton *openWebsiteButton;
    QPushButton *updateCheckButton;
};

#endif
