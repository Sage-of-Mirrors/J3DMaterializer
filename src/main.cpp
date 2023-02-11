#include "UMaterializerApplication.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
	UMaterializerApplication app;

	if (!app.Setup()) {
		std::cout << "Failed to set up J3DMaterializer, please contact Gamma!" << std::endl;
		return 0;
	}

	app.Run();

	if (!app.Teardown()) {
		std::cout << "Something went wrong on teardown, please contact Gamma!" << std::endl;
		return 0;
	}
}
