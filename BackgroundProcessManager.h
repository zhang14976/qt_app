#ifndef BACKGROUNDPROCESSMANAGER_H
#define BACKGROUNDPROCESSMANAGER_H

#include <QWidget>
#include <QObject>
#include <QHash>
#include <QProcess>
#include <QStringList>

class BackgroundProcessManager : public QWidget
{
    Q_OBJECT
public:
    explicit BackgroundProcessManager(QWidget *parent = nullptr);
    ~BackgroundProcessManager();

    bool start(const QString &name, const QString &command, const QString &workingDirectory = QString());
    bool stop(const QString &name, int timeoutMs = 3000);
    void stopAll(int timeoutMs = 3000);
    bool isRunning(const QString &name) const;
    QStringList runningProcessNames() const;
signals:
    void processStarted(const QString &name);
    void processStopped(const QString &name, int exitCode, QProcess::ExitStatus exitStatus);
    void processOutput(const QString &name, const QString &text);
    void processError(const QString &name, const QString &errorText);

private:
    QHash<QString, QProcess *> m_processes;
};

#endif // BACKGROUNDPROCESSMANAGER_H
