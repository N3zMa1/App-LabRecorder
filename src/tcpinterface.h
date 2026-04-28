#pragma once

#include <cstdint>

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QDataStream>
#include <qregularexpression.h>

class RemoteControlSocket : public QObject {
	Q_OBJECT
	QTcpServer server;
	QList<QTcpSocket*> clients;
public:
	RemoteControlSocket(uint16_t port);

signals:
	void refresh_streams();
	void start();
	void stop();
	void filename(QString s);
	void select_all();
	void select_none();
	void add_info(QString s);
	void clear_info();

public slots:
	void addClient();
	void handleLine(QString s, QTcpSocket* sock);
	void onStateChanged(const QString& state);

private:
	void broadcast(const QString& state);
};
