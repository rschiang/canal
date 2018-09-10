#include "trayicon.h"

TrayIcon::TrayIcon()
{
    // Basic setup
    setToolTip(tr("Canal"));
    connect(this, &QSystemTrayIcon::activated, this, &TrayIcon::onActivated);

    // Initialize icon
    QIcon icon(":/res/mac/tray_icon.png");
    icon.setIsMask(true);
    setIcon(icon);

    // Create context menu
    QMenu *contextMenu = new QMenu();
    m_menu = contextMenu;
    setContextMenu(m_menu);

    // Initialize menu items
    this->accountMenuItem = contextMenu->addAction(tr("Not logged in"));
    accountMenuItem->setDisabled(true);

    contextMenu->addAction(tr("Plurk"));
    contextMenu->addAction(tr("Timeline"));
    contextMenu->addSeparator();

    this->notificationMenuItem = contextMenu->addAction(tr("Notification"));
    notificationMenuItem->setCheckable(true);
    notificationMenuItem->setChecked(true);

    contextMenu->addAction(tr("Settings"));
    contextMenu->addSeparator();
    contextMenu->addAction(tr("Quit"), this, &TrayIcon::quitRequested);
}

TrayIcon::~TrayIcon()
{
    delete m_menu;
}

void TrayIcon::updateProfile(Plurq::Profile *profile)
{
    QString text = tr("%1 · @%2").arg(profile->displayName()).arg(profile->nickName());
    accountMenuItem->setText(text);
}

void TrayIcon::onActivated(ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick)
        emit quitRequested();   // TODO: Quick posting
}
