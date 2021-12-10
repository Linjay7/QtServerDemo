#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    
    //服务器状态：一开始是关闭
    m_state = false;
    
    //客户端收到连接申请时，会发送newConnection，就会调用OnNewConnection
    connect(&m_server, &QTcpServer::newConnection, this, &Dialog::OnNewConnection);
    //定时结束后，会发送timeout，就会调用OnTimeOut
    connect(&m_timer, &QTimer::timeout, this, &Dialog::OnTimeOut);
}

Dialog::~Dialog()
{
    delete ui;
}

//创建服务器响应函数
void Dialog::on_btn_createServer_clicked()
{
    if (m_state) {
        //关闭服务器
        m_server.close();
        ui->edit_port->setEnabled(true);
        ui->btn_createServer->setText("创建服务器");
        m_state = false;
        //关闭计时器
        m_timer.stop();
        QMessageBox::information(this, "提示", "服务器已关闭");
    }
    else {
        //获取端口信息
        m_port = ui->edit_port->text().toShort();
        
        //创建服务器
        if (m_server.listen(QHostAddress::Any, m_port)) {
            ui->edit_port->setEnabled(false);
            ui->btn_createServer->setText("关闭服务器");
            m_state = true;
            
            //设置定时器
            m_timer.start(3000);
            QMessageBox::information(this, "提示", "创建服务器成功！");
            
        }
        else {
            QMessageBox::critical(this, "错误", m_server.errorString());
            return;
        }
    }
}

//收到客户端连接申请时响应函数
void Dialog::OnNewConnection()
{
    if (m_server.hasPendingConnections()) {
        //保存连接的套接字
        auto clientSocket = m_server.nextPendingConnection();
        m_clientSockets.append(clientSocket);
        connect(clientSocket, &QTcpSocket::readyRead, this, &Dialog::OnReadReady);
    }
}
//接受客户端消息处理函数
void Dialog::OnReadReady()
{
    //遍历客户端，获得发送消息的客户端
    for (int i = 0; i != m_clientSockets.size(); ++i) {
        if (m_clientSockets.at(i)->bytesAvailable()) {
            //保存消息并显示在界面上
            auto socketContent = m_clientSockets.at(i)->readAll();
            ui->listWidget->addItem(socketContent);
            //发送给其他客户端
            SendMessage(socketContent);
        }
    }
}
//转发客户端消息到其他客户端
void Dialog::SendMessage(const QByteArray& content)
{
    for (int i = 0; i != m_clientSockets.size(); ++i) {
        m_clientSockets.at(i)->write(content);
    }
}
//定时结束后响应函数
void Dialog::OnTimeOut()
{
    //循环遍历所有客户端，并清理无效客户端
    for (int i = m_clientSockets.size() - 1; i != -1; --i) {
        if (!m_clientSockets.at(i)->isValid()) {
            m_clientSockets.removeAt(i);
        }
    }
}

