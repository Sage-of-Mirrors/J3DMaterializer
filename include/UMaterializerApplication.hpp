#pragma once

#include "UApplication.hpp"

class UMaterializerApplication : public UApplication {
	struct GLFWwindow* mWindow;
	class UMaterializerContext* mContext;

	virtual bool Execute(float deltaTime) override;

public:
	UMaterializerApplication();
	virtual ~UMaterializerApplication() {}

	virtual bool Setup() override;
	virtual bool Teardown() override;
};
