#pragma once
#include "gui/config_widget.hpp"
#include "server_controller.hpp"
#include <QMainWindow>
#include <QTextEdit>

namespace gui
{
	class main_window : public QMainWindow
	{
		Q_OBJECT
	public:
		explicit main_window(QWidget* parent = nullptr);

	private:
		config_widget* config_widget_;
		QTextEdit* log_view_;
		server_controller* server_;
	};
} // namespace gui