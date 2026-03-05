#include "gui/main_window.hpp"
#include "QVBoxLayout"
#include "QWidget"
#include "gui/config_widget.hpp"
#include "gui/server_controller.hpp"

namespace gui
{
	main_window::main_window(QWidget* parent) : QMainWindow(parent)
	{
		setWindowTitle("Mirror Stream Server");
		resize(800, 600);

		auto* central = new QWidget;
		auto* layout = new QVBoxLayout(central);

		config_widget_ = new config_widget(this);
		layout->addWidget(config_widget_);

		log_view_ = new QTextEdit;

		log_view_->setReadOnly(true);
		layout->addWidget(log_view_);

		setCentralWidget(central);
		server_ = new server_controller(this);

		connect(config_widget_, &config_widget::start_server, server_, &server_controller::start);

		connect(server_, &server_controller::log_msg, log_view_, &QTextEdit::append);
	}

} // namespace gui