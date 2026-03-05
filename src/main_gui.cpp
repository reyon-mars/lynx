#include "gui/main_window.hpp"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	gui::main_window window;
	window.show();

	return app.exec();
}
