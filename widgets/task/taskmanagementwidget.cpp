/*
 *  This file is a part of KNOSSOS.
 *
 *  (C) Copyright 2007-2018
 *  Max-Planck-Gesellschaft zur Foerderung der Wissenschaften e.V.
 *
 *  KNOSSOS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 of
 *  the License as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  For further information, visit https://knossos.app
 *  or contact knossosteam@gmail.com
 */

#include "taskmanagementwidget.h"

#include "annotation/annotation.h"
#include "network.h"
#include "stateInfo.h"
#include "viewer.h"
#include "widgets/mainwindow.h"

#include <QApplication>
#include <QByteArray>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

#include <fstream>

TaskManagementWidget::TaskManagementWidget(QWidget *parent) : DialogVisibilityNotify(HEIDELBRAIN_INTEGRATION, parent), taskLoginWidget(parent) {//mainwindow as parent for login
    setWindowIcon(QIcon(":/resources/icons/tasks-management.png"));
    setWindowTitle("Task Management");

    baseUrl = taskLoginWidget.host.toString() + api;// loginwidget is child and therefore already alive

    for (auto && label : {&statusLabel, &categoryDescriptionLabel, &taskCommentLabel}) {
        label->setWordWrap(true);
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    }
    statusLabel.setTextInteractionFlags(Qt::TextBrowserInteraction);
    taskLabel.setTextInteractionFlags(Qt::TextBrowserInteraction);
    categoryDescriptionLabel.setTextInteractionFlags(Qt::TextBrowserInteraction);
    taskCommentLabel.setTextInteractionFlags(Qt::TextBrowserInteraction);

    formLayout.setSizeConstraint(QLayout::SetMinimumSize);
    formLayout.addRow("Current Task: ", &taskLabel);
    formLayout.addRow("Description: ", &categoryDescriptionLabel);
    formLayout.addRow("Comment: ", &taskCommentLabel);

    taskInfoGroupBox.setLayout(&formLayout);

    //first row is added in refresh
    gridLayout.addWidget(&submitCommentEdit, 1, 0, 1, 2);
    gridLayout.addWidget(&submitButton, 2, 0, 1, 1);
    gridLayout.addWidget(&submitFinalButton, 2, 1, 1, 1);

    separator.setFrameShape(QFrame::HLine);
    separator.setFrameShadow(QFrame::Sunken);
    gridLayout.addWidget(&separator, 3, 0, 1, 2);
    gridLayout.addWidget(&submitInvalidButton, 4, 0, 1, 1);
    gridLayout.addWidget(&rejectTaskButton, 4, 1, 1, 1);
    taskManagementGroupBox.setLayout(&gridLayout);

    vLayout.addWidget(&taskManagementGroupBox);
    vLayout.addStretch();
    vLayout.addWidget(&logoutButton);

    hLayout.addWidget(&taskInfoGroupBox);
    hLayout.addLayout(&vLayout);
    mainLayout.addWidget(&statusLabel);
    mainLayout.addLayout(&hLayout);
    setLayout(&mainLayout);

    submitCommentEdit.setPlaceholderText("submission comment (optional)");
    submitFinalButton.setToolTip("marks your work as finished.");
    submitInvalidButton.setToolTip("Flag task as invalid.");

    QObject::connect(&startNewTaskButton, &QPushButton::clicked, this, &TaskManagementWidget::startNewTaskButtonClicked);
    QObject::connect(&loadLastSubmitButton, &QPushButton::clicked, this, &TaskManagementWidget::loadLastSubmitButtonClicked);
    QObject::connect(&submitButton, &QPushButton::clicked, this, [this](const bool) { submit(); });
    QObject::connect(&submitFinalButton, &QPushButton::clicked, this, &TaskManagementWidget::submitFinal);
    QObject::connect(&submitInvalidButton, &QPushButton::clicked, this, &TaskManagementWidget::submitInvalid);
    QObject::connect(&rejectTaskButton, &QPushButton::clicked, this, &TaskManagementWidget::rejectTask);
    QObject::connect(&logoutButton, &QPushButton::clicked, this, &TaskManagementWidget::logoutButtonClicked);

    QObject::connect(&taskLoginWidget, &TaskLoginWidget::accepted, [this]{
        loginButtonClicked(taskLoginWidget.urlField.text(), taskLoginWidget.usernameField.text(), taskLoginWidget.passwordField.text());
    });
}

void TaskManagementWidget::setVisible(bool enable) {// showOrLoginOrHide
    if (enable) {
        updateAndRefreshWidget();
    } else {
        DialogVisibilityNotify::setVisible(false);
    }
}

bool TaskManagementWidget::handleError(const QPair<bool, QString> & res, const QString & successText) {
    if (res.first) {
        statusLabel.setText("<font color='green'>" + successText + "</font>");
        return true;
    } else {
        statusLabel.setText("<font color='red'>" + res.second + "</font>");
        return false;
    }
}

bool loginGuardMsg() {
    QMessageBox overwriteMsg(QApplication::activeWindow());
    overwriteMsg.setIcon(QMessageBox::Question);
    overwriteMsg.setText(QObject::tr("You have unsaved changes that will be lost on login."));
    const auto  * discard = overwriteMsg.addButton(QObject::tr("Discard current changes and login."), QMessageBox::AcceptRole);
    overwriteMsg.addButton(QObject::tr("Cancel"), QMessageBox::RejectRole);
    overwriteMsg.exec();
    return overwriteMsg.clickedButton() == discard;
}

void TaskManagementWidget::updateAndRefreshWidget() {
    WidgetDisabler d{*this};// don’t allow widget interaction while Network has an event loop running
    LoadingCursor loadingcursor;
    const auto res = Network::singleton().refresh(baseUrl + "/session/");

    const auto jmap = QJsonDocument::fromJson(res.second.toUtf8()).object();
    if (res.first && !jmap.isEmpty()) {
//        auto username = jmap["username"].toString();// username is not used
        auto & task = Annotation::singleton().activeTask;
        auto fullName = jmap["first_name"].toString() + ' ' + jmap["last_name"].toString();
        auto isAdmin = jmap["is_admin"].toBool();
        task = {jmap["task_project_name"].toString(""), jmap["task_category_name"].toString(""), jmap["task_name"].toString("")};

        if (!Annotation::singleton().isEmpty() && (Annotation::singleton().fileTask != Annotation::singleton().activeTask)) {
            if (!loginGuardMsg()) {
                return;
            }
            Annotation::singleton().clearAnnotation();
        }

        auto taskComment = jmap["task_comment"].toString();
        auto categoryDescription = jmap["task_category_description"].toString();
        const bool hasTask = !task.name.isEmpty();

        auto statusText = tr("Hello ") + fullName + tr("!");
        statusLabel.setText(statusText);
        logoutButton.setText("Logout");

        //last submit/new task are mutually exclusive, show only one of both
        gridLayout.removeWidget(&loadLastSubmitButton);
        gridLayout.removeWidget(&startNewTaskButton);
        if (hasTask) {
            taskLabel.setText(tr("“%1” (%2, %3)").arg(task.name, task.project, task.category));
            gridLayout.addWidget(&loadLastSubmitButton, 0, 0, 1, 2);
        } else {
            taskLabel.setText(tr("None"));
            gridLayout.addWidget(&startNewTaskButton, 0, 0, 1, 2);
        }
        categoryDescriptionLabel.setText(categoryDescription);
        taskCommentLabel.setText(taskComment);
        //buttons would still be shown after removing them from the layout
        loadLastSubmitButton.setVisible(hasTask);
        startNewTaskButton.setVisible(!hasTask);

        submitCommentEdit.setEnabled(hasTask);
        submitButton.setEnabled(hasTask);
        submitFinalButton.setEnabled(hasTask);
        submitInvalidButton.setEnabled(hasTask);
        rejectTaskButton.setVisible(isAdmin);
        rejectTaskButton.setEnabled(hasTask);
        DialogVisibilityNotify::setVisible(true);
    } else {
        emit visibilityChanged(false);
        taskLoginWidget.setResponse("Please login.");
    }
}

void TaskManagementWidget::loginButtonClicked(QUrl host, const QString & username, const QString & password) {
    if (Annotation::singleton().unsavedChanges) {
        if (!loginGuardMsg()) {
            return;
        }
        Annotation::singleton().clearAnnotation();
    }
    if (host.scheme().isEmpty()) {// qnam cannot handle url without protocol
        host = "https://" + taskLoginWidget.urlField.text();
    }
    baseUrl = host.toString() + api;// host changed, update url
    WidgetDisabler d{*this};// don’t allow widget interaction while Network has an event loop running
    LoadingCursor loadingcursor;
    const auto res = Network::singleton().login(baseUrl + "/login/", username, password);

    if (res.first) {
        taskLoginWidget.host = host;//login was successful → save host
        updateAndRefreshWidget();//task infos changed
    } else {
        hide();
        taskLoginWidget.setResponse(res.second);
    }
}

void TaskManagementWidget::logout() {
    WidgetDisabler d{*this};// don’t allow widget interaction while Network has an event loop running
    LoadingCursor loadingcursor;
    const auto res = Network::singleton().refresh(baseUrl + "/logout/");

    if (handleError(res)) {
        hide();
        taskLoginWidget.setResponse("<font color='green'>Logged out successfully.</font>");
    }
}

void TaskManagementWidget::logoutButtonClicked() {
    updateAndRefreshWidget();
    if (isVisible()) {// logout only in valid session
        logout();
    }
}

void TaskManagementWidget::saveAndLoadFile(const QString & filename, const QByteArray & content) {
    QDir taskDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/tasks");
    taskDir.mkpath("./task-files");//filenames of new tasks contain subfolder

    //Retrieve the filename from response header and rename the previously created tmp.nml
    if (!filename.isEmpty()) {
        const QString actualFilename = taskDir.absolutePath() + "/" + filename;

        QFile tmpFile(actualFilename);
        const auto success = tmpFile.open(QIODevice::WriteOnly) && (tmpFile.write(content) != -1);
        tmpFile.close();//it’s reopened by openFileDispatch

        if (success && state->viewer->window->openFileDispatch({actualFilename})) {
            statusLabel.setText("<font color='green'>Loaded annotation successfully.</font>");
        } else {
            statusLabel.setText("<font color='red'>Failed to load »" + actualFilename + "«.</font>");
        }
    }
}

void TaskManagementWidget::loadLastSubmitButtonClicked() {
    if (Annotation::singleton().unsavedChanges) {
        QMessageBox overwriteMsg(this);
        overwriteMsg.setIcon(QMessageBox::Question);
        overwriteMsg.setText(tr("You have unsaved changes that will be overwritten when loading the last submission."));
        overwriteMsg.addButton(tr("Load and lose current changes"), QMessageBox::AcceptRole);
        const auto * cancel = overwriteMsg.addButton(tr("Cancel"), QMessageBox::RejectRole);
        overwriteMsg.exec();
        if (overwriteMsg.clickedButton() == cancel) {
            return;
        }
    }
    Annotation::singleton().clearAnnotation();
    WidgetDisabler d{*this};// don’t allow widget interaction while Network has an event loop running
    LoadingCursor loadingcursor;
    const auto res = Network::singleton().getFile(baseUrl + "/current_file/");

    if (handleError({res.first, res.second.second})) {
        saveAndLoadFile(res.second.first, res.second.second);
    }
}

void TaskManagementWidget::startNewTaskButtonClicked() {
    WidgetDisabler d{*this};// don’t allow widget interaction while Network has an event loop running
    LoadingCursor loadingcursor;
    const auto res = Network::singleton().getPost(baseUrl + "/new_task/");

    if (handleError({res.first, res.second.second})) {
        updateAndRefreshWidget();//task infos changed
        saveAndLoadFile(res.second.first, res.second.second);
    }
}

void TaskManagementWidget::submitFinal() {
    QMessageBox box{QApplication::activeWindow()};
    box.setIcon(QMessageBox::Question);
    box.setText("You are confident the task is done and you want to submit?");
    const auto * accept = box.addButton(tr("Finish task"), QMessageBox::AcceptRole);
    box.addButton(tr("Cancel"), QMessageBox::RejectRole);
    box.exec();
    if (box.clickedButton() == accept) {
        submit(true);
    }
}

void TaskManagementWidget::rejectTask() {
    QMessageBox box{QApplication::activeWindow()};
    box.setIcon(QMessageBox::Question);
    box.setText("Do you want to reject this task to be able to pull a new one?");
    const auto * accept = box.addButton(tr("Reject task"), QMessageBox::AcceptRole);
    box.addButton(tr("Cancel"), QMessageBox::RejectRole);
    box.exec();
    if (box.clickedButton() == accept) {
        const auto & task = Annotation::singleton().activeTask;
        auto res = Network::singleton().rejectTask(baseUrl + "/work/" + task.project + "/" + task.category + "/" + task.name);
        if (handleError(res, res.second)) {
            updateAndRefreshWidget();//task infos changed
            statusLabel.setText("<font color='green'>Rejected task successfully.</font>");
        }
    }
}

void TaskManagementWidget::submitInvalid() {
    QMessageBox box{QApplication::activeWindow()};
    box.setIcon(QMessageBox::Question);
    box.setText("Do you really want to flag the task as invalid?");
    const auto * accept = box.addButton(tr("Flag invalid"), QMessageBox::AcceptRole);
    box.addButton(tr("Cancel"), QMessageBox::RejectRole);
    box.exec();
    if (box.clickedButton() == accept) {
        submit(true, false);
    }
}

bool TaskManagementWidget::submit(const bool final, const bool valid) {
    state->viewer->window->save();//save file to submit

    WidgetDisabler d{*this};// don’t allow widget interaction while Network has an event loop running
    LoadingCursor loadingcursor;
    auto res = Network::singleton().submitHeidelbrain(baseUrl + "/submit/", Annotation::singleton().annotationFilename, submitCommentEdit.text(), final, valid);

    if (handleError(res, res.second)) {
        submitCommentEdit.clear();//clean comment if submit was successful
        if (final) {
            state->viewer->window->newAnnotationSlot();//clear the annotation to ease starting a new one
            updateAndRefreshWidget();//task infos changed. Do this after clearing annotation, else a mismatching task will be detected.
        }
        return true;
    }
    return false;
}
