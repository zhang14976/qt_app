#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void on_pushButton_rpc_clicked();

    void on_pushButton_net_autoGateway_clicked();

    void on_pushButton_net_apply_clicked();

    void on_pushButton_model_choice_clicked();

    void on_pushButton_llamacpp_choice_clicked();

    void on_pushButton_model_start_or_stop_clicked();

private:
    Ui::Widget *ui;
    int systemType;
    int rpcStatus;
    // 用于异步执行提权后的网络配置脚本
    QProcess *m_networkApplyProcess = nullptr;

    // 临时脚本和日志路径
    QString m_networkScriptPath;
    QString m_networkLogPath;
    QProcess *m_modelProcess = nullptr;
    // RPC 进程，必须是成员变量
    QProcess *m_rpcProcess = nullptr;

    void createRpcProcess();

    // 启动 RPC
    bool startRpcServer();

    // 停止 RPC
    void stopRpcServer();

    // 获取 rpc-server 的完整路径
    QString getRpcServerPath(int systemType) const;

    // 获取 RPC 绑定地址
    QString getRpcBindAddress() const;

    // 通用的进程启动函数
    bool startProgram(
        QProcess *process,
        const QString &program,
        const QStringList &arguments,
        const QString &workingDirectory
    );

    bool m_modelStopRequested = false;
    void rpc_display();
};
#endif // WIDGET_H
