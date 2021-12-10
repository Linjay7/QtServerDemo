#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
    
private slots:
    //创建服务器
    void on_btn_createServer_clicked();
    //收到客户端连接申请时响应函数
    void OnNewConnection();
    //接受客户端消息处理函数
    void OnReadReady();
    //转发客户端消息到其他客户端
    void SendMessage(const QByteArray& content);
    
    //定时结束后响应函数
    void OnTimeOut();
    
private:
    Ui::Dialog *ui;
    
    QTcpServer m_server;
    quint16 m_port;
    QList<QTcpSocket*> m_clientSockets;
    bool m_state;
    QTimer m_timer;
    
};
#endif // DIALOG_H
