#include "tcpinterface.h"
#include <QDebug>

RemoteControlSocket::RemoteControlSocket(uint16_t port) : server() {
	server.listen(QHostAddress::Any, port);
	connect(&server, &QTcpServer::newConnection, this, &RemoteControlSocket::addClient);
}

void RemoteControlSocket::addClient() {
	auto *client = server.nextPendingConnection();
	clients.push_back(client);
	connect(client, &QTcpSocket::readyRead, this, [this, client]() {
		while(client->canReadLine())
			this->handleLine(client->readLine().trimmed(), client);
	});
}

void RemoteControlSocket::handleLine(QString s, QTcpSocket *sock) {
	qInfo() << s;
	if (s == "start")
		emit start();
	else if (s == "stop")
		emit stop();
	else if (s == "update")
			emit refresh_streams();
	else if (s.contains("filename")) {
		emit filename(s);
	} else if (s.contains("select")) {
		if (s.contains("all")) {
			emit select_all();
		} else if (s.contains("none")) {
			emit select_none();
		}
	}
	else if (s.contains("info")) {
		if (s.contains("clear"))
			emit clear_info();
		else
			emit add_info(s);
	}
	sock->write("OK\n");
	// TODO: select /deselect streams
	// TODO: send acknowledgement
	// TODO: get current state
	//
	// else this->sender()->sender("Whoops");
}

void RemoteControlSocket::onStateChanged(const QString& state)
{
	broadcast(state);
}

void RemoteControlSocket::broadcast(const QString& state)
{
	for (auto* client : clients) {
		if (client->state() == QAbstractSocket::ConnectedState) {
			client->write(state.toUtf8() + "\n");
		}
	}
}
