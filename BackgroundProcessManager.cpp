#include "BackgroundProcessManager.h"

#include <QPointer>
#include <QTimer>

#include <signal.h>
#include <unistd.h>

BackgroundProcessManager::BackgroundProcessManager(QObject *parent) : QObject(parent)
{

}

BackgroundProcessManager::~BackgroundProcessManager()
{
    const QList<QProcess *> processes = m_processes.values();

    for (QProcess *process : processes)
    {
        if (process == nullptr)
        {
            continue;
        }

        if (process->state() == QProcess::NotRunning)
        {
            continue;
        }

        qint64 processId = process->processId();

        if (processId > 0)
        {
            ::kill(-static_cast<pid_t>(processId), SIGKILL);
        }

        process->kill();
        process->waitForFinished(1000);
    }

    m_processes.clear();
}

bool BackgroundProcessManager::start(const QString &name, const QString &command, const QString &workingDirectory)
{
    if (name.trimmed().isEmpty())
    {
        emit processError(name, QStringLiteral("进程名称不能为空"));
        return false;
    }

    if (command.trimmed().isEmpty())
    {
        emit processError(name, QStringLiteral("运行命令不能为空"));
        return false;
    }

    if (isRunning(name))
    {
        emit processError(name, QStringLiteral("该后台进程已经在运行"));
        return false;
    }

    if (m_processes.contains(name))
    {
        QProcess *oldProcess = m_processes.take(name);

        if (oldProcess != nullptr)
        {
            oldProcess->deleteLater();
        }
    }

    QProcess *process = new QProcess(this);

    process->setProcessChannelMode(QProcess::MergedChannels);

    if (!workingDirectory.trimmed().isEmpty())
    {
        process->setWorkingDirectory(workingDirectory);
    }

    connect(process, &QProcess::started, this, [this, name]()
    {
        emit processStarted(name);
    });

    connect(process, &QProcess::readyReadStandardOutput, this, [this, name, process]()
    {
        QByteArray data = process->readAllStandardOutput();

        if (!data.isEmpty())
        {
            emit processOutput(name, QString::fromLocal8Bit(data));
        }
    });

    connect(process, &QProcess::errorOccurred, this, [this, name, process](QProcess::ProcessError)
    {
        emit processError(name, process->errorString());
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, name, process](int exitCode, QProcess::ExitStatus exitStatus)
    {
        QByteArray remainingData = process->readAllStandardOutput();

        if (!remainingData.isEmpty())
        {
            emit processOutput(name, QString::fromLocal8Bit(remainingData));
        }

        if (m_processes.value(name) == process)
        {
            m_processes.remove(name);
        }

        emit processStopped(name, exitCode, exitStatus);
        process->deleteLater();
    });

    m_processes.insert(name, process);

    QStringList arguments;
    arguments << QStringLiteral("bash");
    arguments << QStringLiteral("-lc");
    arguments << command;

    process->start(QStringLiteral("setsid"), arguments);

    if (!process->waitForStarted(3000))
    {
        QString errorText = process->errorString();

        if (m_processes.value(name) == process)
        {
            m_processes.remove(name);
        }

        process->deleteLater();

        emit processError(name, errorText);
        return false;
    }

    return true;
}

bool BackgroundProcessManager::stop(const QString &name, int timeoutMs)
{
    QProcess *process = m_processes.value(name, nullptr);

    if (process == nullptr)
    {
        emit processError(name, QStringLiteral("没有找到对应的后台进程"));
        return false;
    }

    if (process->state() == QProcess::NotRunning)
    {
        return false;
    }

    qint64 processId = process->processId();

    if (processId > 0)
    {
        int result = ::kill(-static_cast<pid_t>(processId), SIGTERM);

        if (result != 0)
        {
            process->terminate();
        }
    }
    else
    {
        process->terminate();
    }

    QPointer<QProcess> safeProcess = process;

    QTimer::singleShot(timeoutMs, this, [safeProcess, processId]()
    {
        if (safeProcess.isNull())
        {
            return;
        }

        if (safeProcess->state() == QProcess::NotRunning)
        {
            return;
        }

        if (processId > 0)
        {
            ::kill(-static_cast<pid_t>(processId), SIGKILL);
        }

        safeProcess->kill();
    });

    return true;
}

void BackgroundProcessManager::stopAll(int timeoutMs)
{
    QStringList names = m_processes.keys();

    for (const QString &name : names)
    {
        stop(name, timeoutMs);
    }
}

bool BackgroundProcessManager::isRunning(const QString &name) const
{
    QProcess *process = m_processes.value(name, nullptr);

    if (process == nullptr)
    {
        return false;
    }

    return process->state() != QProcess::NotRunning;
}

QStringList BackgroundProcessManager::runningProcessNames() const
{
    QStringList names;

    for (auto iterator = m_processes.constBegin(); iterator != m_processes.constEnd(); ++iterator)
    {
        QProcess *process = iterator.value();

        if (process != nullptr && process->state() != QProcess::NotRunning)
        {
            names.append(iterator.key());
        }
    }

    return names;
}
