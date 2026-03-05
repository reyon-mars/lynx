#include "gui/config_widget.hpp"
#include "net/socket_config.hpp"
#include "net/socket_type.hpp"
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QVBoxLayout>

namespace gui
{
	config_widget::config_widget(QWidget* parent) : QWidget(parent)
	{
		setup_ui();
	}

	void config_widget::setup_ui()
	{
		auto* layout = new QVBoxLayout(this);

		auto* ip_layout = new QHBoxLayout;
		ip_layout->addWidget(new QLabel("IP: "));
		ip_input_ = new QLineEdit("0.0.0.0");
		ip_layout->addWidget(ip_input_);
		layout->addLayout(ip_layout);

		auto* port_layout = new QHBoxLayout;
		port_layout->addWidget(new QLabel("Port: "));
		port_input_ = new QLineEdit("8080");
		port_layout->addWidget(port_input_);
		layout->addLayout(port_layout);

		type_combo_ = new QComboBox;
		type_combo_->addItem("TCP");
		type_combo_->addItem("UDP");
		layout->addWidget(new QLabel("Type: "));
		layout->addWidget(type_combo_);

		start_button_ = new QPushButton("Start Server");
		layout->addWidget(start_button_);

		connect(start_button_, &QPushButton::clicked, this, &config_widget::on_start_clicked);
	}

	void config_widget::on_start_clicked()
	{
		net::socket_config cfg{};
		cfg.sock_addr.ip = ip_input_->text().toStdString();
		cfg.sock_addr.port = static_cast<uint16_t>(port_input_->text().toUInt());
		cfg.typ = (type_combo_->currentIndex() == 0 ? net::Type::Sstream : net::Type::Datagram);

		emit start_server(cfg);
	}

} // namespace gui