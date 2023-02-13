#include "UMaterializerContext.hpp"
#include "ui/UMaterializerUIPanel.hpp"
#include "ui/UMaterializerStagePanel.hpp"
#include "util/UUIUtil.hpp"
#include "UMaterialLayer.hpp"

#include <J3D/J3DModelLoader.hpp>
#include <J3D/J3DModelData.hpp>
#include <J3D/J3DUniformBufferObject.hpp>
#include <J3D/J3DLight.hpp>
#include <J3D/J3DModelInstance.hpp>

#include <ImGuiFileDialog.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_internal.h>
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

void UMaterializerContext::SetUpDocking() {
	const ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoDockingInCentralNode;
	mMainDockSpaceID = ImGui::DockSpaceOverViewport(mainViewport, dockFlags);
	
	if (!bIsDockingSetUp) {
		ImGui::DockBuilderRemoveNode(mMainDockSpaceID); // clear any previous layout
		ImGui::DockBuilderAddNode(mMainDockSpaceID, dockFlags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(mMainDockSpaceID, mainViewport->Size);

		mDockNodeTopID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Up, 0.5f, nullptr, &mMainDockSpaceID);
		mDockNodeRightID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Right, 0.5f, nullptr, &mMainDockSpaceID);
		mDockNodeDownID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Down, 0.5f, nullptr, &mMainDockSpaceID);
		mDockNodeLeftID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Left, 0.5f, nullptr, &mMainDockSpaceID);

		ImGui::DockBuilderDockWindow("mainWindow", mDockNodeLeftID);

		ImGui::DockBuilderFinish(mMainDockSpaceID);

		bIsDockingSetUp = true;
	}
}

void UMaterializerContext::RenderLayerList() {
	if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("layerList", ImVec2(0, 200), true, 0);

		if (ImGui::Selectable("Main", mCurrentLayerIndex == 0)) {
			mCurrentLayerIndex = 0;
			mMaterials = mLayers[mCurrentLayerIndex]->GetMaterials();
		}

		for (int i = 1; i < mLayers.size(); i++) {
			char buf[16];
			std::snprintf(buf, 16, "Layer %i", i);

			if (ImGui::Selectable(buf, mCurrentLayerIndex == i)) {
				mCurrentLayerIndex = i;
				mMaterials = mLayers[mCurrentLayerIndex]->GetMaterials();
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
}

void UMaterializerContext::RenderMaterialList() {
	if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("materialList", ImVec2(0, 200), true, 0);

		for (int i = 0; i < mMaterials.size(); i++) {
			if (ImGui::Selectable(mMaterials[i]->Name.c_str(), mCurrentMaterialIndex == i)) {
				mCurrentMaterialIndex = i;
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
}

void UMaterializerContext::RenderTevStageTree() {
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("tevStages", ImVec2(0, 200), true, 0);

	for (int i = 0; i < mMaterials[mCurrentMaterialIndex]->TevBlock->mTevStages.size(); i++) {
		char windowName[128];

		char stageName[16];
		std::snprintf(stageName, 16, "Stage %i", i);

		if (ImGui::Selectable(stageName)) {
			std::snprintf(windowName, 128, "%s - TEV Stage %i", mMaterials[mCurrentMaterialIndex]->Name.data(), i);

			bool bAlreadyOpen = false;
			for (std::shared_ptr<UMaterializerUIPanel> panel : mPanels) {
				if (std::strcmp(panel->GetName().data(), windowName) == 0) {
					bAlreadyOpen = true;
					break;
				}
			}

			if (!bAlreadyOpen) {
				mPanels.push_back(std::make_shared<UMaterializerStagePanel>(windowName, mMaterials[mCurrentMaterialIndex]->TevBlock, i));
			}
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void UMaterializerContext::RenderMainWindow(float deltaTime) {
	// Disable the tab bar for the main window.
	ImGuiWindowClass mainWindowOverride;
	mainWindowOverride.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
	ImGui::SetNextWindowClass(&mainWindowOverride);

	ImGui::Begin("mainWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	
	if (mData.get() != nullptr) {
		RenderLayerList();
		RenderMaterialList();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Current Material")) {
			ImGui::Text("Name: %s", mMaterials[mCurrentMaterialIndex]->Name.data());

			ImGui::Spacing();

			if (ImGui::CollapsingHeader("TEV Stages")) {
				RenderTevStageTree();
			}

			ImGui::Spacing();

			if (ImGui::CollapsingHeader("TEV Colors")) {
				ImGui::PushID("tevColor");

				ImGui::ColorEdit4("Color 0", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevColors[0].r, ImGuiColorEditFlags_Float);
				ImGui::ColorEdit4("Color 1", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevColors[1].r, ImGuiColorEditFlags_Float);
				ImGui::ColorEdit4("Color 2", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevColors[2].r, ImGuiColorEditFlags_Float);
				ImGui::ColorEdit4("Color 3", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevColors[3].r, ImGuiColorEditFlags_Float);

				ImGui::PopID();
			}

			ImGui::Spacing();
			ImGui::Spacing();

			if (ImGui::CollapsingHeader("Konst Colors")) {
				ImGui::PushID("konstColor");

				float c[4]{
					mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[1].r,
					mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[1].g,
					mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[1].b,
					mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[1].a
				};

				if (ImGui::ColorEdit4("Color 1", c, ImGuiColorEditFlags_Float)) {
					mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[1].r = c[0];
					mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[1].g = c[1];
					mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[1].b = c[2];
					mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[1].a = c[3];
				}

				ImGui::ColorEdit4("Color 0", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[0].r, ImGuiColorEditFlags_Float);
				//ImGui::ColorEdit4("Color 1", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[1].r, ImGuiColorEditFlags_Float);
				ImGui::ColorEdit4("Color 2", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[2].r, ImGuiColorEditFlags_Float);
				ImGui::ColorEdit4("Color 3", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[3].r, ImGuiColorEditFlags_Float);

				ImGui::PopID();
			}

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Spacing();

			if (ImGui::CollapsingHeader("Pixel Engine")) {
				J3DPixelEngineBlock* pixelEngine = &mMaterials[mCurrentMaterialIndex]->PEBlock;

				ImGui::Checkbox("Dithering Enabled", &pixelEngine->mDither);
				ImGui::Checkbox("ZCompLoc", &pixelEngine->mZCompLoc);

				ImGui::Spacing();
				ImGui::Spacing();

				if (ImGui::CollapsingHeader("Alpha Compare")) {
					ImGui::PushID("alphaCompare");

					UIUtil::RenderComboEnum<EGXCompareType>("Compare Function A", pixelEngine->mAlphaCompare.CompareFunc0);
					int refA = pixelEngine->mAlphaCompare.Reference0;
					if (ImGui::InputInt("Reference A", &refA)) {
						pixelEngine->mAlphaCompare.Reference0 = refA;
					}

					ImGui::Spacing();
					UIUtil::RenderComboEnum<EGXAlphaOp>("Operation", pixelEngine->mAlphaCompare.Operation);
					ImGui::Spacing();

					UIUtil::RenderComboEnum<EGXCompareType>("Compare Function B", pixelEngine->mAlphaCompare.CompareFunc1);
					int refB = pixelEngine->mAlphaCompare.Reference1;
					if (ImGui::InputInt("Reference B", &refB)) {
						pixelEngine->mAlphaCompare.Reference1 = refB;
					}

					ImGui::Spacing();

					ImGui::PopID();
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader("Blend Mode")) {
					ImGui::PushID("blendMode");

					UIUtil::RenderComboEnum<EGXBlendMode>("Type", pixelEngine->mBlendMode.Type);

					ImGui::Spacing();

					UIUtil::RenderComboEnum<EGXBlendModeControl>("Source Factor", pixelEngine->mBlendMode.SourceFactor);
					UIUtil::RenderComboEnum<EGXBlendModeControl>("Dest Factor", pixelEngine->mBlendMode.DestinationFactor);

					ImGui::Spacing();

					UIUtil::RenderComboEnum<EGXLogicOp>("Operation", pixelEngine->mBlendMode.Operation);

					ImGui::Spacing();

					ImGui::PopID();
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader("Fog")) {
					ImGui::PushID("fog");

					ImGui::Checkbox("Enabled", &pixelEngine->mFog.Enable);

					ImGui::Spacing();

					if (pixelEngine->mFog.Enable) {
						UIUtil::RenderComboEnum<EGXFogType>("Type", pixelEngine->mFog.Type);

						ImGui::Spacing();

						int center = pixelEngine->mFog.Center;
						if (ImGui::InputInt("Center", &center)) {
							pixelEngine->mFog.Center = center;
						}

						ImGui::Spacing();

						ImGui::DragFloat("Start Z", &pixelEngine->mFog.StartZ);
						ImGui::DragFloat("End Z", &pixelEngine->mFog.EndZ);
						ImGui::DragFloat("Near Z", &pixelEngine->mFog.NearZ);
						ImGui::DragFloat("Far Z", &pixelEngine->mFog.FarZ);

						ImGui::Spacing();

						ImGui::ColorEdit4("Color", &pixelEngine->mFog.Color.r, ImGuiColorEditFlags_Float);
					}

					ImGui::Spacing();

					ImGui::PopID();
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader("Z-Mode")) {
					ImGui::PushID("zMode");

					ImGui::Checkbox("Enabled", &pixelEngine->mZMode.Enable);

					ImGui::Spacing();

					if (pixelEngine->mZMode.Enable) {
						UIUtil::RenderComboEnum<EGXCompareType>("Compare Type", pixelEngine->mZMode.Function);
						ImGui::Checkbox("Updates Enabled", &pixelEngine->mZMode.UpdateEnable);
					}

					ImGui::Spacing();

					ImGui::PopID();
				}
			}
		}
	}

	ImGui::End();
}

void UMaterializerContext::RenderPanels(float deltaTime) {
	SetUpDocking();

	RenderMainWindow(deltaTime);

	for (int i = 0; i < mPanels.size(); i++) {
		if (mPanels[i]->GetClosingStatus()) {
			mPanels.erase(mPanels.begin() + i);
			i--;

			continue;
		}

		mPanels[i]->Render(deltaTime);
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
	mLayers.push_back(std::make_shared<UMaterialLayer>(mMaterials));

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

	delete stream;
}
