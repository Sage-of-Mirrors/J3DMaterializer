#include "UMaterializerContext.hpp"
#include "ui/UMaterializerUIPanel.hpp"
#include "ui/UMaterializerStagePanel.hpp"

#include <J3D/J3DModelLoader.hpp>
#include <J3D/J3DModelData.hpp>
#include <J3D/J3DUniformBufferObject.hpp>
#include <J3D/J3DLight.hpp>
#include <J3D/J3DModelInstance.hpp>

#include <ImGuiFileDialog.h>
#include <glad/glad.h>
#include <imgui.h>
#include <bstream.h>

bool UMaterializerContext::Update(float deltaTime) {
	mCamera.Update(deltaTime);

	if (mMaterials.size() != 0)
		mMaterials[mCurrentMaterialIndex].get()->GenerateShaders();

	return true;
}

void UMaterializerContext::Render(float deltaTime) {
	RenderPanels(deltaTime);
	RenderMenuBar();

	J3DUniformBufferObject::SetProjAndViewMatrices(&mCamera.GetProjectionMatrix(), &mCamera.GetViewMatrix());

	if (mInstance.get() != nullptr)
		mInstance->Render(deltaTime);
}

void UMaterializerContext::RenderPanels(float deltaTime) {
	if (mStagePanel.get() != nullptr) {
		mStagePanel->Render(deltaTime);
	}
}

void UMaterializerContext::RenderMenuBar() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Open...")) {
			OpenModelCB();
		}

		ImGui::Separator();
		ImGui::MenuItem("Close");

		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit")) {
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("About")) {
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	if (bIsFileDialogOpen)
		ImGuiFileDialog::Instance()->OpenDialog("OpenFileDialog", "Choose File", "J3D Models (*.bmd *.bdl){.bmd,.bdl}", ".");

	if (ImGuiFileDialog::Instance()->Display("OpenFileDialog")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string FilePath = ImGuiFileDialog::Instance()->GetFilePathName();

			try {
				LoadModelFromPath(FilePath);
			}
			catch (std::exception e) {
				std::cout << "Failed to load model " << FilePath << "!\n";
			}

			bIsFileDialogOpen = false;
		}

		ImGuiFileDialog::Instance()->Close();
	}
}

void UMaterializerContext::OpenModelCB() {
	bIsFileDialogOpen = true;
}

void UMaterializerContext::LoadModel(bStream::CStream* stream) {
	J3DModelLoader Loader;

	mData = Loader.Load(stream, NULL);
	mInstance = mData->GetInstance();

	mMaterials = mData->GetMaterials();

	J3DLight lights[8];

	//lights[0].Position = glm::vec4(00000, 00000, 00000, 1);
	//lights[0].AngleAtten = glm::vec4(1.0, 0.0, 0.0, 1);
	//lights[0].DistAtten = glm::vec4(1.0, 0.0, 0.0, 1);

	//lights[1].Position = glm::vec4(-5654.32, 561.264, 5105.44, 1);
	//lights[1].AngleAtten = glm::vec4(1.0, 0.0, 0.0, 1);
	//lights[1].DistAtten = glm::vec4(1000.0, 0.5, 0.0, 1);

	//lights[2].Position = glm::vec4(200000, 500000, 200000, 1);
	//lights[2].AngleAtten = glm::vec4(0, 0, 1, 1);
	//lights[2].DistAtten = glm::vec4(25.0, 0.0, -24.0, 1);
	//lights[2].Direction = glm::vec4(0.434196, -0.868448, 0.239316, 1);

	for (int i = 0; i < 8; i++)
		lights[i].Color = glm::vec4(1, 1, 1, 1);

	J3DUniformBufferObject::SetLights(lights);
}

void UMaterializerContext::LoadModelFromPath(std::filesystem::path filePath) {
	bStream::CFileStream* stream = new bStream::CFileStream(
		filePath.generic_u8string().c_str(),
		bStream::Endianess::Big,
		bStream::OpenMode::In
	);
	
	LoadModel(stream);

	mStagePanel = std::make_shared<UMaterializerStagePanel>("TEV Stage");
	mStagePanel->SetStageList(mMaterials[mCurrentMaterialIndex].get()->TevBlock.mTevStages);

	delete stream;
}
