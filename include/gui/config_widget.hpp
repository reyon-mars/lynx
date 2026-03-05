#pragma once
#include "net/socket_config.hpp"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace gui
{
	class config_widget : public QWidget
	{
		Q_OBJECT
	public:
		explicit config_widget(QWidget* parent = nullptr);
	signals:
		void start_server(net::socket_config cfg);

	private slots:
		void on_start_clicked();

	private:
		void setup_ui();

		QLineEdit* ip_input_;
		QLineEdit* port_input_;
		QComboBox* type_combo_;
		QPushButton* start_button_;
	};
} // namespace gui
