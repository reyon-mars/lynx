#pragma once
#include "net/socket_config.hpp"
#include <QObject>
#include <QString>
#include <atomic>
#include <thread>

namespace gui
{
	class server_controller : public QObject
	{
		Q_OBJECT
	public:
		explicit server_controller(QObject* parent = nullptr);
		~server_controller() override;

		void start(net::socket_config cfg);

		void stop();

	signals:
		void log_msg(const QString&);
		void client_connected(const QString&);

	private:
		void server_loop(net::socket_config cfg);

		std::thread worker_;
		std::atomic_bool running_{false};
	};
} // namespace gui
