#include "widget.h"
#include "ui_widget.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QStringList>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDebug>

#include <QProcessEnvironment>



#include <QHostAddress>
#include <QAbstractSocket>


namespace {

bool isValidIpAddress(const QString &ip)
{
    const QStringList parts = ip.split('.');

    if (parts.size() != 4) {
        return false;
    }

    for (const QString &part : parts) {
        bool ok = false;
        const int value = part.toInt(&ok);

        if (!ok || value < 0 || value > 255 ||
            part != QString::number(value)) {
            return false;
        }
    }

    return true;
}

bool isValidSubnetNumber(const QString &subnet)
{
    bool ok = false;
    const int value = subnet.toInt(&ok);
    return ok && value >= 1 && value <= 32;
}

QString shellSingleQuote(const QString &value)
{
    QString escaped = value;
    escaped.replace("'", "'\\''");
    return "'" + escaped + "'";
}

QString powershellSingleQuote(const QString &value)
{
    QString escaped = value;
    escaped.replace("'", "''");
    return "'" + escaped + "'";
}

QString powershellDoubleQuote(const QString &value)
{
    QString escaped = value;
    escaped.replace("`", "``");
    escaped.replace("\"", "`\"");
    return "\"" + escaped + "\"";
}

} // namespace

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
#if defined(Q_OS_LINUX)
    systemType = 1;
#else
    systemType = 0;
#endif
    rpcStatus = 1;
    ui->lineEdit_net_subnetNumber->setText("24");
    ui->lineEdit_net_DNS->setText("8.8.8.8");
    ui->pushButton_model_start_or_stop->setText("启动模型");

    ui->lineEdit_rpc_port->setText("50052");
    ui->label_rpc_status->setFixedSize(20, 20);
    rpc_display();

//    ui->pushButton_rpc->setText(tr("启动 RPC"));

//    ui->label_8->setStyleSheet(

//    )
}

Widget::~Widget()
{
    if (m_networkApplyProcess) {
        m_networkApplyProcess->deleteLater();
        m_networkApplyProcess = nullptr;
    }

    if (m_modelProcess) {
        m_modelProcess->terminate();
        if (!m_modelProcess->waitForFinished(2000)) {
            m_modelProcess->kill();
            m_modelProcess->waitForFinished();
        }
        m_modelProcess->deleteLater();
        m_modelProcess = nullptr;
    }

    if (m_rpcProcess &&
        m_rpcProcess->state() !=
            QProcess::NotRunning) {

        m_rpcProcess->terminate();

        if (!m_rpcProcess->waitForFinished(2000)) {
            m_rpcProcess->kill();
            m_rpcProcess->waitForFinished(1000);
        }
    }

    delete ui;
}

void Widget::rpc_display()
{
    if(rpcStatus == 0)
    {
        ui->label_rpc_status->setStyleSheet(
            "background-color:#22b455;"
            "border-radius:10px;"
        );
        ui->pushButton_rpc->setStyleSheet(
            "color: #16a34a;"
            "font-size: 20px;"
            "font-weight: bold;"
        );
        ui->pushButton_rpc->setText("已开启");
    }
    else if(rpcStatus != 0)
    {
        ui->label_rpc_status->setStyleSheet(
            "background-color:#ef4444;"
            "border-radius:10px;"
        );
        ui->pushButton_rpc->setStyleSheet(
            "color: #ef4444;"
            "font-size: 20px;"
            "font-weight: bold;"
        );
        ui->pushButton_rpc->setText("未开启");
    }
}

void Widget::on_pushButton_model_start_or_stop_clicked()
{
    if (m_modelProcess &&
        m_modelProcess->state() != QProcess::NotRunning) {
        m_modelStopRequested = true;
        ui->pushButton_model_start_or_stop->setEnabled(false);
        ui->pushButton_model_start_or_stop->setText("停止中...");
        m_modelProcess->terminate();

        if (!m_modelProcess->waitForFinished(3000)) {
            m_modelProcess->kill();
        }

        return;
    }

    const QString llamaCppDirText =
        ui->lineEdit_llamacpp_path_text->text().trimmed();
    const QString modelPathText =
        ui->lineEdit_model_path_text->text().trimmed();
    const QString rpcIp =
        ui->lineEdit_net_staticIP->text().trimmed();

    if (llamaCppDirText.isEmpty()) {
        QMessageBox::warning(this, "模型启动", "请先选择 llama.cpp 路径。");
        return;
    }

    if (modelPathText.isEmpty()) {
        QMessageBox::warning(this, "模型启动", "请先选择模型文件。");
        return;
    }

    if (!isValidIpAddress(rpcIp)) {
        QMessageBox::warning(this, "模型启动", "请先填写有效的静态 IP，用于 --rpc 参数。");
        return;
    }

    const QFileInfo llamaCppDirInfo(llamaCppDirText);

    if (!llamaCppDirInfo.exists() || !llamaCppDirInfo.isDir()) {
        QMessageBox::warning(this, "模型启动", "llama.cpp 路径不是有效文件夹。");
        return;
    }

    const QFileInfo modelFileInfo(modelPathText);

    if (!modelFileInfo.exists() || !modelFileInfo.isFile()) {
        QMessageBox::warning(this, "模型启动", "模型文件不存在。");
        return;
    }

    QString executableName;

    if (systemType == 0) {
        executableName = "llama-server.exe";
    } else if (systemType == 1) {
        executableName = "llama-server";
    } else {
        QMessageBox::warning(this, "模型启动", "未知操作系统，无法判断 llama-server 文件名。");
        return;
    }

    const QString llamaCppDir =
        QDir::cleanPath(llamaCppDirInfo.absoluteFilePath());
    QFileInfo executableInfo(QDir(llamaCppDir).filePath(executableName));

    if (systemType == 0 && !executableInfo.exists()) {
        const QFileInfo fallbackInfo(QDir(llamaCppDir).filePath("llama-server"));

        if (fallbackInfo.exists()) {
            executableInfo = fallbackInfo;
        }
    }

    if (!executableInfo.exists() || !executableInfo.isFile()) {
        QMessageBox::warning(
            this,
            "模型启动",
            "未在 llama.cpp 路径下找到 " + executableName + "。");
        return;
    }

    if (systemType == 1 && !executableInfo.isExecutable()) {
        QMessageBox::warning(
            this,
            "模型启动",
            "llama-server 没有执行权限，请先执行：chmod +x llama-server");
        return;
    }

    const QString rpcAddress = rpcIp + ":50052";
    QStringList arguments;
    arguments << "-m"
              << modelFileInfo.absoluteFilePath()
              << "--rpc"
              << rpcAddress
              << "--host"
              << "0.0.0.0"
              << "-ngl"
              << "100";

    if (m_modelProcess) {
        m_modelProcess->deleteLater();
        m_modelProcess = nullptr;
    }

    m_modelStopRequested = false;
    m_modelProcess = new QProcess(this);
    m_modelProcess->setWorkingDirectory(llamaCppDir);

    connect(m_modelProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
                QProcess *process = qobject_cast<QProcess *>(sender());

                if (!process) {
                    return;
                }

                const QString output =
                    QString::fromLocal8Bit(process->readAllStandardOutput()).trimmed();
                const QString error =
                    QString::fromLocal8Bit(process->readAllStandardError()).trimmed();

                qDebug() << "llama-server 退出状态：" << exitStatus;
                qDebug() << "llama-server 退出代码：" << exitCode;
                qDebug() << "llama-server 输出：" << output;
                qDebug() << "llama-server 错误：" << error;

                ui->pushButton_model_start_or_stop->setEnabled(true);
                ui->pushButton_model_start_or_stop->setText("启动模型");

                if (!m_modelStopRequested &&
                    (exitStatus != QProcess::NormalExit || exitCode != 0)) {
                    QMessageBox::warning(
                        this,
                        "模型启动",
                        "llama-server 已退出。\n\n" + error);
                }

                m_modelStopRequested = false;

                if (m_modelProcess == process) {
                    m_modelProcess = nullptr;
                }

                process->deleteLater();
            });

    connect(m_modelProcess,
            &QProcess::errorOccurred,
            this,
            [this](QProcess::ProcessError) {
                QProcess *process = qobject_cast<QProcess *>(sender());

                if (!process) {
                    return;
                }

                ui->pushButton_model_start_or_stop->setEnabled(true);
                ui->pushButton_model_start_or_stop->setText("启动模型");
                m_modelStopRequested = false;
                QMessageBox::warning(
                    this,
                    "模型启动",
                    "llama-server 启动失败：\n" +
                    process->errorString());

                if (m_modelProcess == process) {
                    m_modelProcess = nullptr;
                }

                process->deleteLater();
            });

    qDebug() << "启动 llama-server：" << executableInfo.absoluteFilePath();
    qDebug() << "llama-server 工作目录：" << llamaCppDir;
    qDebug() << "llama-server 参数：" << arguments;

    ui->pushButton_model_start_or_stop->setText("停止模型");
    m_modelProcess->start(executableInfo.absoluteFilePath(), arguments);

    if (!m_modelProcess) {
        return;
    }

    if (!m_modelProcess->waitForStarted(3000)) {
        QProcess *process = m_modelProcess;
        const QString error = process->errorString();
        m_modelProcess = nullptr;
        m_modelStopRequested = false;
        ui->pushButton_model_start_or_stop->setText("启动模型");
        QMessageBox::warning(this, "模型启动", "llama-server 启动失败：\n" + error);
        process->deleteLater();
    }
}

void Widget::createRpcProcess()
{
    // 已经创建过就不要重复创建
    if (m_rpcProcess != nullptr) {
        return;
    }

    m_rpcProcess = new QProcess(this);

    m_rpcProcess->setProcessChannelMode(
        QProcess::MergedChannels
    );

    connect(
        m_rpcProcess,
        &QProcess::started,
        this,
        [this]()
        {
            rpcStatus = 0;
            rpc_display();

            qDebug() << "RPC 服务已经启动";
            qDebug() << "RPC PID："
                     << m_rpcProcess->processId();
        }
    );

    connect(
        m_rpcProcess,
        &QProcess::readyRead,
        this,
        [this]()
        {
            const QString output =
                QString::fromLocal8Bit(
                    m_rpcProcess->readAll()
                );

            qDebug().noquote()
                << output;

            // 有日志显示框时可以使用：
            /*
            ui->plainTextEdit_rpc_log
                ->appendPlainText(
                    output.trimmed()
                );
            */
        }
    );

    connect(
        m_rpcProcess,
        static_cast<void (QProcess::*)(
            int,
            QProcess::ExitStatus
        )>(&QProcess::finished),
        this,
        [this](
            int exitCode,
            QProcess::ExitStatus exitStatus
        )
        {
            rpcStatus = 1;
            rpc_display();

            qDebug() << "RPC 服务已经结束";
            qDebug() << "退出代码：" << exitCode;
            qDebug() << "退出状态：" << exitStatus;
        }
    );

    connect(
        m_rpcProcess,
        &QProcess::errorOccurred,
        this,
        [this](QProcess::ProcessError error)
        {
            qDebug() << "RPC 进程错误：" << error;
            qDebug() << "错误信息："
                     << m_rpcProcess->errorString();

            if (m_rpcProcess->state() ==
                QProcess::NotRunning) {

                rpcStatus = 1;
                rpc_display();
            }
        }
    );
}

bool Widget::startRpcServer()
{
//    const int systemType = getSystemType();
    if (systemType != 0 && systemType != 1)
    {
        QMessageBox::critical(this, tr("启动失败"), tr("当前操作系统不受支持。"));
        return false;
    }

    const QString rpcServerPath = getRpcServerPath(systemType);
    if (rpcServerPath.isEmpty())
    {
        QMessageBox::warning(this, tr("路径错误"), tr("请先选择包含 rpc-server 的目录。"));
        return false;
    }

    QFileInfo rpcFileInfo(rpcServerPath);
    if (!rpcFileInfo.exists() || !rpcFileInfo.isFile())
    {
        QMessageBox::critical(this, tr("启动失败"), tr("指定目录中不存在：\n%1").arg(systemType == 0 ? "rpc-server.exe" : "rpc-server"));
        return false;
    }

    if (systemType == 1 && !rpcFileInfo.isExecutable())
    {
        QFile::Permissions permissions = QFile::permissions(rpcServerPath);

        permissions |= QFileDevice::ExeOwner;
        permissions |= QFileDevice::ExeGroup;
        permissions |= QFileDevice::ExeOther;

        if (!QFile::setPermissions(rpcServerPath, permissions))
        {
            QMessageBox::critical(this, tr("启动失败"), tr("rpc-server 没有执行权限，" "并且无法自动添加权限。\n\n" "请执行：\n" "chmod +x \"%1\"").arg(rpcServerPath));
            return false;
        }

        rpcFileInfo.refresh();
    }

    const QString bindAddress = getRpcBindAddress();

    if (bindAddress.isEmpty())
    {
        QMessageBox::warning(this, tr("地址错误"), tr("静态 IP 地址格式不正确。"));
        return false;
    }

    const int rpcPort = 50052;

    const QStringList arguments = {
        "--host",
        bindAddress,
        "--port",
        QString::number(rpcPort)
    };

    const QString workingDirectory = rpcFileInfo.absolutePath();

    if (!startProgram(m_rpcProcess, rpcServerPath, arguments, workingDirectory))
        return false;

    QString displayAddress = bindAddress;

    if (bindAddress == "0.0.0.0")
        displayAddress = tr("所有本机 IPv4 接口");

    QMessageBox::information(
        this,
        tr("RPC 服务"),
        tr("RPC 服务已经启动。\n\n"
           "监听地址：%1\n"
           "端口：%2\n"
           "进程 ID：%3")
            .arg(displayAddress)
            .arg(rpcPort)
            .arg(m_rpcProcess->processId())
    );

    return true;
}

void Widget::stopRpcServer()
{
    if (!m_rpcProcess ||
        m_rpcProcess->state() ==
            QProcess::NotRunning) {

        rpcStatus = 1;
        rpc_display();

        return;
    }

    qDebug() << "正在停止 RPC 服务";

    // 先尝试正常关闭
    m_rpcProcess->terminate();

    if (!m_rpcProcess->waitForFinished(3000)) {
        qDebug() << "正常关闭超时，强制结束";

        m_rpcProcess->kill();
        m_rpcProcess->waitForFinished(1000);
    }

    rpcStatus = 1;
    rpc_display();

    QMessageBox::information(
        this,
        tr("RPC 服务"),
        tr("RPC 服务已经停止。")
                        );
}

QString Widget::getRpcServerPath(int systemType) const
{
    QString directory = ui->lineEdit_llamacpp_path_text->text().trimmed();

    if (directory.isEmpty()) {
        return {};
    }

    directory =
        QDir::fromNativeSeparators(directory);

    const QFileInfo directoryInfo(directory);

    if (!directoryInfo.exists() ||
        !directoryInfo.isDir()) {
        return {};
    }

    QString executableName;

    if (systemType == 0) {
        executableName = "rpc-server.exe";
    } else if (systemType == 1) {
        executableName = "rpc-server";
    } else {
        return {};
    }

    return QDir(directory).filePath(
        executableName
                        );
    }

QString Widget::getRpcBindAddress() const
{
    /*
     * 优先使用用户填写的静态 IP。
     *
     * 如果没有填写，则监听所有 IPv4 接口。
     */
    const QString staticIP =
        ui->lineEdit_net_staticIP
            ->text()
            .trimmed();

    if (staticIP.isEmpty()) {
        return "0.0.0.0";
    }

    QHostAddress address;

    if (!address.setAddress(staticIP) ||
        address.protocol() !=
            QAbstractSocket::IPv4Protocol) {

        return {};
    }

    return staticIP;
}

bool Widget::startProgram(QProcess *process, const QString &program, const QStringList &arguments, const QString &workingDirectory)
{
    if (!process) {
        return false;
    }

    if (process->state() !=
        QProcess::NotRunning) {
        return false;
    }

    const QFileInfo programInfo(program);

    if (!programInfo.exists() ||
        !programInfo.isFile()) {

        QMessageBox::critical(
            this,
            tr("启动失败"),
            tr("找不到程序：\n%1")
                .arg(
                    QDir::toNativeSeparators(
                        program
                    )
                )
        );

        return false;
    }

    QProcessEnvironment environment =
        QProcessEnvironment::systemEnvironment();

//    const int systemType =
//        getSystemType();

    if (systemType == 0) {
        /*
         * Windows：
         * 让程序优先从当前目录寻找 DLL。
         */
        const QString oldPath =
            environment.value("PATH");

        environment.insert(
            "PATH",
            workingDirectory +
                QDir::listSeparator() +
                oldPath
        );
    } else if (systemType == 1) {
        /*
         * Ubuntu：
         * 让 rpc-server 找到同目录中的：
         * libggml.so
         * libggml-vulkan.so
         * libggml-rpc.so
         */
        const QString oldLibraryPath =
            environment.value(
                "LD_LIBRARY_PATH"
            );

        QString newLibraryPath =
            workingDirectory;

        if (!oldLibraryPath.isEmpty()) {
            newLibraryPath +=
                ":" + oldLibraryPath;
        }

        environment.insert(
            "LD_LIBRARY_PATH",
            newLibraryPath
        );
    }

    process->setWorkingDirectory(
        workingDirectory
    );

    process->setProcessEnvironment(
        environment
    );

    qDebug() << "启动程序：" << program;
    qDebug() << "启动参数：" << arguments;
    qDebug() << "工作目录：" << workingDirectory;

    process->start(
        program,
        arguments
    );

    if (!process->waitForStarted(3000)) {
        QMessageBox::critical(
            this,
            tr("启动失败"),
            tr("无法启动程序。\n\n"
               "程序：%1\n"
               "错误：%2")
                .arg(
                    QDir::toNativeSeparators(
                        program
                    )
                )
                .arg(process->errorString())
        );

        return false;
    }

    return true;
}

void Widget::on_pushButton_rpc_clicked()
{
    /*
     * 第一次点击时，m_rpcProcess 是空指针，
     * 此时才创建进程对象。
     */
    if (m_rpcProcess == nullptr) {
        createRpcProcess();
    }

    if (m_rpcProcess == nullptr) {
        QMessageBox::critical(
            this,
            tr("RPC 服务"),
            tr("无法创建 RPC 进程对象。")
        );

        return;
    }

    if (m_rpcProcess->state() ==
        QProcess::NotRunning) {

        // 当前没有运行，点击后启动
        startRpcServer();

    } else {
        // 当前正在运行，点击后关闭
        stopRpcServer();
    }
}

void Widget::on_pushButton_net_autoGateway_clicked()
{
    QString program;
    QStringList arguments;

    if (systemType == 0) {
        /*
         * Windows：
         * 获取所有 IPv4 默认路由；
         * 按路由跃点和接口跃点排序；
         * 取优先级最高的默认网关。
         */
        program = QStandardPaths::findExecutable("powershell.exe");

        if (program.isEmpty()) {
            program = "C:/Windows/System32/WindowsPowerShell/v1.0/powershell.exe";
        }

        arguments << "-NoProfile"
                  << "-NonInteractive"
                  << "-Command"
                  << "(Get-NetRoute "
                     "-AddressFamily IPv4 "
                     "-DestinationPrefix '0.0.0.0/0' "
                     "-ErrorAction SilentlyContinue | "
                     "Where-Object {$_.NextHop -ne '0.0.0.0'} | "
                     "Sort-Object RouteMetric,InterfaceMetric | "
                     "Select-Object -First 1 "
                     "-ExpandProperty NextHop)";

    } else if (systemType == 1) {
        /*
         * Ubuntu：
         * 使用 ip 命令查询 IPv4 默认路由。
         */
        program = QStandardPaths::findExecutable("ip");

        if (program.isEmpty() && QFileInfo::exists("/usr/sbin/ip")) {
            program = "/usr/sbin/ip";
        }

        if (program.isEmpty() && QFileInfo::exists("/usr/bin/ip")) {
            program = "/usr/bin/ip";
        }

        if (program.isEmpty() && QFileInfo::exists("/sbin/ip")) {
            program = "/sbin/ip";
        }

        arguments << "-4"
                  << "route"
                  << "show"
                  << "default";

    } else {
        ui->lineEdit_net_gateway->setText("未知操作系统");
        qDebug() << "未知的系统类型：" << systemType;
        return;
    }

    if (program.isEmpty()) {
        ui->lineEdit_net_gateway->setText("找不到系统命令");
        qDebug() << "找不到对应的系统命令";
        return;
    }

    QProcess process;

    qDebug() << "执行程序：" << program;
    qDebug() << "执行参数：" << arguments;

    process.start(program, arguments);

    // 先判断进程是否成功启动
    if (!process.waitForStarted(2000)) {
        ui->lineEdit_net_gateway->setText("命令启动失败");

        qDebug() << "命令启动失败："
                 << process.errorString();

        return;
    }

    // 等待进程执行结束
    if (!process.waitForFinished(5000)) {
        process.kill();
        process.waitForFinished();

        ui->lineEdit_net_gateway->setText("获取网关超时");

        qDebug() << "命令执行超时："
                 << process.errorString();

        return;
    }

    const QString standardOutput =
        QString::fromLocal8Bit(
            process.readAllStandardOutput()
        ).trimmed();

    const QString standardError =
        QString::fromLocal8Bit(
            process.readAllStandardError()
        ).trimmed();

    qDebug() << "命令输出：" << standardOutput;
    qDebug() << "错误输出：" << standardError;
    qDebug() << "退出代码：" << process.exitCode();

    if (process.exitStatus() != QProcess::NormalExit ||
        process.exitCode() != 0) {
        ui->lineEdit_net_gateway->setText("获取网关失败");
        return;
    }

    QString gateway;

    if (systemType == 0) {
        /*
         * Windows PowerShell 已经只输出网关地址，例如：
         * 192.168.1.1
         */
        const QStringList lines =
            standardOutput.split(
                QRegularExpression("[\\r\\n]+"),
                Qt::SkipEmptyParts
            );

        if (!lines.isEmpty()) {
            gateway = lines.first().trimmed();
        }

    } else if (systemType == 1) {
        /*
         * Ubuntu 输出示例：
         * default via 192.168.1.1 dev eno1 proto dhcp metric 100
         */
        const QStringList lines =
            standardOutput.split(
                QRegularExpression("[\\r\\n]+"),
                Qt::SkipEmptyParts
            );

        for (const QString &line : lines) {
            const QStringList parts =
                line.split(
                    QRegularExpression("\\s+"),
                    Qt::SkipEmptyParts
                );

            const int viaIndex = parts.indexOf("via");

            if (viaIndex >= 0 &&
                viaIndex + 1 < parts.size()) {
                gateway = parts.at(viaIndex + 1);
                break;
            }
        }
    }

    if (gateway.isEmpty()) {
        ui->lineEdit_net_gateway->setText("未检测到默认网关");
        qDebug() << "没有获取到默认网关";
        return;
    }

    ui->lineEdit_net_gateway->setText(gateway);
    qDebug() << "默认网关：" << gateway;
}

void Widget::on_pushButton_net_apply_clicked()
{
    const QString gateway = ui->lineEdit_net_gateway->text().trimmed();
    const QString ipAddress = ui->lineEdit_net_staticIP->text().trimmed();
    QString subnetNumber = ui->lineEdit_net_subnetNumber->text().trimmed();
    QString dnsServer = ui->lineEdit_net_DNS->text().trimmed();

    if (subnetNumber.isEmpty()) {
        subnetNumber = "24";
    }

    if (dnsServer.isEmpty()) {
        dnsServer = "8.8.8.8";
    }

    if (!isValidIpAddress(ipAddress)) {
        QMessageBox::warning(this, "网络配置", "静态 IP 地址格式无效。");
        return;
    }

    if (!isValidSubnetNumber(subnetNumber)) {
        QMessageBox::warning(this, "网络配置", "子网掩码位数必须是 1-32。");
        return;
    }

    if (!isValidIpAddress(gateway)) {
        QMessageBox::warning(this, "网络配置", "网关地址格式无效。");
        return;
    }

    if (!isValidIpAddress(dnsServer)) {
        QMessageBox::warning(this, "网络配置", "DNS 地址格式无效。");
        return;
    }

    const QString summary =
        QString("即将应用以下网络配置：\n\n"
                "IP 地址：%1/%2\n"
                "网关：%3\n"
                "DNS：%4\n\n"
                "该操作需要管理员/root 权限，并可能临时断开网络。是否继续？")
            .arg(ipAddress, subnetNumber, gateway, dnsServer);

    if (QMessageBox::question(this, "确认应用网络配置", summary) !=
        QMessageBox::Yes) {
        return;
    }

    const QString timestamp =
        QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");

    QString program;
    QStringList arguments;

    if (m_networkApplyProcess) {
        m_networkApplyProcess->kill();
        m_networkApplyProcess->deleteLater();
        m_networkApplyProcess = nullptr;
    }

    if (systemType == 1) {
        m_networkScriptPath =
            QDir::temp().filePath("qt_network_apply_" + timestamp + ".sh");
        m_networkLogPath =
            QDir::temp().filePath("qt_network_apply_" + timestamp + ".log");

        QFile scriptFile(m_networkScriptPath);

        if (!scriptFile.open(QIODevice::WriteOnly |
                             QIODevice::Text |
                             QIODevice::Truncate)) {
            QMessageBox::warning(this, "网络配置", "无法创建 Ubuntu 网络配置脚本。");
            return;
        }

        const QString script =
            QString(
                "#!/bin/bash\n"
                "set -u\n"
                "LOG_FILE=%1\n"
                "exec > >(tee -a \"$LOG_FILE\") 2>&1\n"
                "IP_ADDRESS=%2\n"
                "SUBNET_MASK=%3\n"
                "GATEWAY=%4\n"
                "DNS_SERVER=%5\n"
                "echo \"开始应用网络配置...\"\n"
                "systemctl enable NetworkManager\n"
                "systemctl start NetworkManager\n"
                "nmcli connection delete bridge >/dev/null 2>&1 || true\n"
                "nmcli connection delete bridge-eth >/dev/null 2>&1 || true\n"
                "nmcli connection delete bridge-thunderbolt0 >/dev/null 2>&1 || true\n"
                "nmcli connection delete bridge-thunderbolt1 >/dev/null 2>&1 || true\n"
                "nmcli connection add type bridge con-name bridge ifname nm-bridge bridge.stp yes ipv4.method auto ipv6.method ignore\n"
                "nmcli connection modify bridge ipv4.method manual ipv4.addresses \"$IP_ADDRESS/$SUBNET_MASK\" ipv4.gateway \"$GATEWAY\" ipv4.dns \"$DNS_SERVER\"\n"
                "nmcli connection add type ethernet con-name bridge-eth ifname eno1 master bridge bridge-port.path-cost 1000 || true\n"
                "nmcli connection add type ethernet con-name bridge-thunderbolt0 ifname thunderbolt0 master bridge ethernet.mtu 65520 bridge-port.path-cost 1 || true\n"
                "nmcli connection add type ethernet con-name bridge-thunderbolt1 ifname thunderbolt1 master bridge ethernet.mtu 65520 bridge-port.path-cost 1 || true\n"
                "nmcli connection up bridge\n"
                "nmcli connection show --active\n"
                "ip addr show bridge || true\n"
                "echo \"网络配置完成。建议重启系统使配置完全生效。\"\n")
                .arg(shellSingleQuote(m_networkLogPath),
                     shellSingleQuote(ipAddress),
                     shellSingleQuote(subnetNumber),
                     shellSingleQuote(gateway),
                     shellSingleQuote(dnsServer));

        scriptFile.write(script.toUtf8());
        scriptFile.close();
        QFile::setPermissions(m_networkScriptPath,
                              QFileDevice::ReadOwner |
                              QFileDevice::WriteOwner |
                              QFileDevice::ExeOwner |
                              QFileDevice::ReadGroup |
                              QFileDevice::ExeGroup |
                              QFileDevice::ReadOther |
                              QFileDevice::ExeOther);

        program = QStandardPaths::findExecutable("sh");

        if (program.isEmpty()) {
            program = "/bin/sh";
        }

        arguments << "-c"
                  << QString("if [ \"$(id -u)\" -eq 0 ]; then "
                             "bash %1; "
                             "elif command -v pkexec >/dev/null 2>&1; then "
                             "pkexec bash %1; "
                             "else "
                             "x-terminal-emulator -e sudo bash %1; "
                             "fi")
                         .arg(shellSingleQuote(m_networkScriptPath));
    } else if (systemType == 0) {
        m_networkScriptPath =
            QDir::temp().filePath("qt_network_apply_" + timestamp + ".ps1");
        m_networkLogPath =
            QDir::temp().filePath("qt_network_apply_" + timestamp + ".log");

        QFile scriptFile(m_networkScriptPath);

        if (!scriptFile.open(QIODevice::WriteOnly |
                             QIODevice::Text |
                             QIODevice::Truncate)) {
            QMessageBox::warning(this, "网络配置", "无法创建 Windows 网络配置脚本。");
            return;
        }

        const QString script =
            QString(
                "$ErrorActionPreference = 'Stop'\r\n"
                "$LogFile = %1\r\n"
                "Start-Transcript -Path $LogFile -Append | Out-Null\r\n"
                "$IpAddress = %2\r\n"
                "$SubnetMask = %3\r\n"
                "$Gateway = %4\r\n"
                "$DnsServer = %5\r\n"
                "try {\r\n"
                "  $route = Get-NetRoute -AddressFamily IPv4 -DestinationPrefix '0.0.0.0/0' -ErrorAction SilentlyContinue | Where-Object { $_.NextHop -ne '0.0.0.0' } | Sort-Object RouteMetric, InterfaceMetric | Select-Object -First 1\r\n"
                "  if (-not $route) { throw '未检测到默认网络接口。' }\r\n"
                "  $ifIndex = $route.InterfaceIndex\r\n"
                "  Set-NetIPInterface -InterfaceIndex $ifIndex -Dhcp Disabled\r\n"
                "  Get-NetIPAddress -InterfaceIndex $ifIndex -AddressFamily IPv4 -ErrorAction SilentlyContinue | Remove-NetIPAddress -Confirm:$false -ErrorAction SilentlyContinue\r\n"
                "  Get-NetRoute -InterfaceIndex $ifIndex -AddressFamily IPv4 -DestinationPrefix '0.0.0.0/0' -ErrorAction SilentlyContinue | Remove-NetRoute -Confirm:$false -ErrorAction SilentlyContinue\r\n"
                "  New-NetIPAddress -InterfaceIndex $ifIndex -IPAddress $IpAddress -PrefixLength ([int]$SubnetMask) -DefaultGateway $Gateway\r\n"
                "  Set-DnsClientServerAddress -InterfaceIndex $ifIndex -ServerAddresses $DnsServer\r\n"
                "  Write-Host '网络配置完成。'\r\n"
                "} finally {\r\n"
                "  Stop-Transcript | Out-Null\r\n"
                "}\r\n")
                .arg(powershellSingleQuote(m_networkLogPath),
                     powershellSingleQuote(ipAddress),
                     powershellSingleQuote(subnetNumber),
                     powershellSingleQuote(gateway),
                     powershellSingleQuote(dnsServer));

        scriptFile.write(script.toUtf8());
        scriptFile.close();

        program = QStandardPaths::findExecutable("powershell.exe");

        if (program.isEmpty()) {
            program = "C:/Windows/System32/WindowsPowerShell/v1.0/powershell.exe";
        }

        const QString elevatedArguments =
            QString("-NoProfile -ExecutionPolicy Bypass -File %1")
                .arg(powershellDoubleQuote(m_networkScriptPath));

        arguments << "-NoProfile"
                  << "-ExecutionPolicy"
                  << "Bypass"
                  << "-Command"
                  << QString("Start-Process -FilePath powershell.exe "
                             "-Verb RunAs "
                             "-ArgumentList %1")
                         .arg(powershellSingleQuote(elevatedArguments));
    } else {
        QMessageBox::warning(this, "网络配置", "未知操作系统，无法应用配置。");
        return;
    }

    if (program.isEmpty()) {
        QMessageBox::warning(this, "网络配置", "找不到执行网络配置所需的系统命令。");
        return;
    }

    m_networkApplyProcess = new QProcess(this);

    connect(m_networkApplyProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
                ui->pushButton_net_apply->setEnabled(true);
                ui->pushButton_net_apply->setText("应用配置");

                const QString output =
                    QString::fromLocal8Bit(m_networkApplyProcess->readAllStandardOutput()).trimmed();
                const QString error =
                    QString::fromLocal8Bit(m_networkApplyProcess->readAllStandardError()).trimmed();

                qDebug() << "网络配置启动器退出状态：" << exitStatus;
                qDebug() << "网络配置启动器退出代码：" << exitCode;
                qDebug() << "网络配置启动器输出：" << output;
                qDebug() << "网络配置启动器错误：" << error;
                qDebug() << "网络配置日志：" << m_networkLogPath;

                if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                    QMessageBox::information(
                        this,
                        "网络配置",
                        "网络配置命令已启动。\n\n"
                        "如果系统弹出权限确认，请允许执行。\n"
                        "执行日志：\n" + m_networkLogPath);
                } else {
                    QMessageBox::warning(
                        this,
                        "网络配置",
                        "网络配置命令启动失败。\n\n" + error);
                }

                m_networkApplyProcess->deleteLater();
                m_networkApplyProcess = nullptr;
            });

    connect(m_networkApplyProcess,
            &QProcess::errorOccurred,
            this,
            [this](QProcess::ProcessError) {
                ui->pushButton_net_apply->setEnabled(true);
                ui->pushButton_net_apply->setText("应用配置");
                QMessageBox::warning(
                    this,
                    "网络配置",
                    "网络配置命令启动失败：\n" +
                    m_networkApplyProcess->errorString());
                m_networkApplyProcess->deleteLater();
                m_networkApplyProcess = nullptr;
            });

    ui->pushButton_net_apply->setEnabled(false);
    ui->pushButton_net_apply->setText("应用中...");

    qDebug() << "网络配置脚本：" << m_networkScriptPath;
    qDebug() << "网络配置日志：" << m_networkLogPath;
    qDebug() << "网络配置启动程序：" << program;
    qDebug() << "网络配置启动参数：" << arguments;

    m_networkApplyProcess->start(program, arguments);
}

void Widget::on_pushButton_model_choice_clicked()
{
    QString startDir = ui->lineEdit_model_path_text->text().trimmed();

    if (!startDir.isEmpty()) {
        const QFileInfo currentPathInfo(startDir);

        if (currentPathInfo.isFile()) {
            startDir = currentPathInfo.absolutePath();
        } else if (!currentPathInfo.isDir()) {
            startDir.clear();
        }
    }

    if (startDir.isEmpty()) {
        if (systemType == 0) {
            startDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        } else if (systemType == 1) {
            startDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        } else {
            startDir = QDir::homePath();
        }
    }

    const QString filter =
        "模型文件 (*.gguf);;"
        "所有文件 (*)";

    const QString selectedPath =
        QFileDialog::getOpenFileName(
            this,
            "选择模型文件",
            startDir,
            filter);

    if (selectedPath.isEmpty()) {
        return;
    }

    const QString absolutePath = QFileInfo(selectedPath).absoluteFilePath();

    if (systemType == 0) {
        ui->lineEdit_model_path_text->setText(
            QDir::toNativeSeparators(absolutePath));
    } else if (systemType == 1) {
        ui->lineEdit_model_path_text->setText(
            QDir::fromNativeSeparators(QDir::cleanPath(absolutePath)));
    } else {
        ui->lineEdit_model_path_text->setText(
            QDir::cleanPath(absolutePath));
    }
}

void Widget::on_pushButton_llamacpp_choice_clicked()
{
    QString startDir =
        ui->lineEdit_llamacpp_path_text->text().trimmed();

    if (!startDir.isEmpty()) {
        const QFileInfo currentPathInfo(startDir);

        if (currentPathInfo.isFile()) {
            // 当前填写的是文件时，从该文件所在目录开始
            startDir = currentPathInfo.absolutePath();
        } else if (currentPathInfo.isDir()) {
            // 转换成绝对目录路径
            startDir = currentPathInfo.absoluteFilePath();
        } else {
            // 路径不存在
            startDir.clear();
        }
    }

    // 当前输入的路径不可用时，设置默认起始目录
    if (startDir.isEmpty()) {
        if (systemType == 0) {
            // Windows：默认打开“文档”目录
            startDir = QStandardPaths::writableLocation(
                QStandardPaths::DocumentsLocation
            );
        } else if (systemType == 1) {
            // Ubuntu：默认打开用户主目录
            startDir = QStandardPaths::writableLocation(
                QStandardPaths::HomeLocation
            );
        } else {
            startDir = QDir::homePath();
        }
    }

    // 弹出文件夹选择窗口
    const QString selectedDir =
        QFileDialog::getExistingDirectory(
            this,
            tr("选择 llama.cpp 文件夹"),
            startDir,
            QFileDialog::ShowDirsOnly |
            QFileDialog::DontResolveSymlinks
        );

    // 用户取消选择
    if (selectedDir.isEmpty()) {
        return;
    }

    const QString absolutePath =
        QFileInfo(selectedDir).absoluteFilePath();

    if (systemType == 0) {
        // Windows 显示为 C:\xxx\xxx
        ui->lineEdit_llamacpp_path_text->setText(
            QDir::toNativeSeparators(
                QDir::cleanPath(absolutePath)
            )
        );
    } else {
        // Ubuntu 显示为 /home/xxx/xxx
        ui->lineEdit_llamacpp_path_text->setText(
            QDir::fromNativeSeparators(
                QDir::cleanPath(absolutePath)
            )
        );
    }
}
