#include "UMaterializerContext.hpp"
#include "ui/UMaterializerUIPanel.hpp"
#include "ui/UMaterializerStagePanel.hpp"
#include "ui/UMaterializerTexGenPanel.hpp"
#include "ui/UMaterializerTexMatrixpanel.hpp"
#include "ui/UMaterializerColorChannelPanel.hpp"

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
		ImGui::Indent();

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

		ImGui::Unindent();
	}
}

void UMaterializerContext::RenderMaterialList() {
	if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Indent();

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("materialList", ImVec2(0, 200), true, 0);

		for (int i = 0; i < mMaterials.size(); i++) {
			if (ImGui::Selectable(mMaterials[i]->Name.c_str(), mCurrentMaterialIndex == i)) {
				mCurrentMaterialIndex = i;
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::Unindent();
	}
}

void UMaterializerContext::RenderTevStageList() {
	ImGui::Indent();

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("tevStages", ImVec2(0, 150), true, 0);

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

	if (ImGui::Button("Add TEV Stage", ImVec2(200, 0)) && mMaterials[mCurrentMaterialIndex]->TevBlock->mTevStages.size() < 16) {
		std::shared_ptr<J3DTevStageInfo> newTevStage = std::make_shared<J3DTevStageInfo>();
		std::shared_ptr<J3DTevOrderInfo> newTevOrder = std::make_shared<J3DTevOrderInfo>();

		mMaterials[mCurrentMaterialIndex]->TevBlock->mTevStages.push_back(newTevStage);
		mMaterials[mCurrentMaterialIndex]->TevBlock->mTevOrders.push_back(newTevOrder);
	}

	ImGui::SameLine();

	if (ImGui::Button("Remove TEV Stage", ImVec2(200, 0)) && mMaterials[mCurrentMaterialIndex]->TevBlock->mTevStages.size() > 1) {
		mMaterials[mCurrentMaterialIndex]->TevBlock->mTevStages.erase(mMaterials[mCurrentMaterialIndex]->TevBlock->mTevStages.end() - 1);
		mMaterials[mCurrentMaterialIndex]->TevBlock->mTevOrders.erase(mMaterials[mCurrentMaterialIndex]->TevBlock->mTevOrders.end() - 1);
	}

	ImGui::Unindent();
}

void UMaterializerContext::RenderTexGenList() {
	ImGui::Indent();

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("texGens1", ImVec2(0, 150), true, 0);

	for (int i = 0; i < mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexCoordInfo.size(); i++) {
		char windowName[128];

		char stageName[16];
		std::snprintf(stageName, 16, "TexCoordGen %i", i);

		if (ImGui::Selectable(stageName)) {
			std::snprintf(windowName, 128, "%s - TexCoordGen %i", mMaterials[mCurrentMaterialIndex]->Name.data(), i);

			bool bAlreadyOpen = false;
			for (std::shared_ptr<UMaterializerUIPanel> panel : mPanels) {
				if (std::strcmp(panel->GetName().data(), windowName) == 0) {
					bAlreadyOpen = true;
					break;
				}
			}

			if (!bAlreadyOpen) {
				mPanels.push_back(std::make_shared<UMaterializerTexGenPanel>(windowName, mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexCoordInfo[i]));
			}
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();

	if (ImGui::Button("Add Tex Gen", ImVec2(200, 0)) && mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexCoordInfo.size() < 8) {
		std::shared_ptr<J3DTexCoordInfo> newTexGen = std::make_shared<J3DTexCoordInfo>();
		mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexCoordInfo.push_back(newTexGen);
	}

	ImGui::SameLine();

	if (ImGui::Button("Remove Tex Gen", ImVec2(200, 0)) && mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexCoordInfo.size() > 0) {
		mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexCoordInfo.erase(mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexCoordInfo.end() - 1);
	}

	ImGui::Unindent();
}

void UMaterializerContext::RenderTexMatrixList() {
	ImGui::Indent();

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("texMtxs", ImVec2(0, 150), true, 0);

	for (int i = 0; i < mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexMatrix.size(); i++) {
		char windowName[128];

		char stageName[16];
		std::snprintf(stageName, 16, "TexMatrix %i", i);

		if (ImGui::Selectable(stageName)) {
			std::snprintf(windowName, 128, "%s - TexMatrix %i", mMaterials[mCurrentMaterialIndex]->Name.data(), i);

			bool bAlreadyOpen = false;
			for (std::shared_ptr<UMaterializerUIPanel> panel : mPanels) {
				if (std::strcmp(panel->GetName().data(), windowName) == 0) {
					bAlreadyOpen = true;
					break;
				}
			}

			if (!bAlreadyOpen) {
				mPanels.push_back(std::make_shared<UMaterializerTexMatrixPanel>(windowName, mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexMatrix[i]));
			}
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();

	if (ImGui::Button("Add Tex Matrix", ImVec2(200, 0)) && mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexMatrix.size() < 8) {
		std::shared_ptr<J3DTexMatrixInfo> newTexMtx = std::make_shared<J3DTexMatrixInfo>();
		mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexMatrix.push_back(newTexMtx);
	}

	ImGui::SameLine();

	if (ImGui::Button("Remove Tex Matrix", ImVec2(200, 0)) && mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexMatrix.size() > 0) {
		mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexMatrix.erase(mMaterials[mCurrentMaterialIndex]->TexGenBlock.mTexMatrix.end() - 1);
	}

	ImGui::Unindent();
}

void UMaterializerContext::RenderColorChannelList() {
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("colorChans", ImVec2(0, 46), true, 0);

	for (int i = 0; i < mMaterials[mCurrentMaterialIndex]->LightBlock.mColorChannels.size(); i++) {
		char windowName[128];

		char stageName[16];
		std::snprintf(stageName, 16, "Color Channel %i", i);

		if (ImGui::Selectable(stageName)) {
			std::snprintf(windowName, 128, "%s - Color Channel %i", mMaterials[mCurrentMaterialIndex]->Name.data(), i);

			bool bAlreadyOpen = false;
			for (std::shared_ptr<UMaterializerUIPanel> panel : mPanels) {
				if (std::strcmp(panel->GetName().data(), windowName) == 0) {
					bAlreadyOpen = true;
					break;
				}
			}

			if (!bAlreadyOpen) {
				mPanels.push_back(std::make_shared<UMaterializerColorChannelPanel>(windowName, mMaterials[mCurrentMaterialIndex]->LightBlock.mColorChannels[i]));
			}
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();

	if (ImGui::Button("Add Color Channel", ImVec2(200, 0)) && mMaterials[mCurrentMaterialIndex]->LightBlock.mColorChannels.size() < 2) {
		std::shared_ptr<J3DColorChannel> newChannel = std::make_shared<J3DColorChannel>();
		mMaterials[mCurrentMaterialIndex]->LightBlock.mColorChannels.push_back(newChannel);
	}

	ImGui::SameLine();

	if (ImGui::Button("Remove Color Channel", ImVec2(200, 0)) && mMaterials[mCurrentMaterialIndex]->LightBlock.mColorChannels.size() > 0) {
		mMaterials[mCurrentMaterialIndex]->LightBlock.mColorChannels.erase(mMaterials[mCurrentMaterialIndex]->LightBlock.mColorChannels.end() - 1);
	}
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
			ImGui::Indent();

			ImGui::Text("Name: %s", mMaterials[mCurrentMaterialIndex]->Name.data());

			ImGui::Spacing();

			ImGui::PushID("TevBlock");
			if(ImGui::CollapsingHeader("TEV Block")) {
				ImGui::Indent();
				if (ImGui::CollapsingHeader("TEV Stages")) {
					RenderTevStageList();
				}
				ImGui::Unindent();

				ImGui::Spacing();

				/*
				ImGui::Indent();
				if (ImGui::CollapsingHeader("TEV Colors")) {
					ImGui::PushID("tevColor");

					ImGui::Indent();
					ImGui::ColorEdit4("Color 0", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevColors[0].r, ImGuiColorEditFlags_Float);
					ImGui::ColorEdit4("Color 1", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevColors[1].r, ImGuiColorEditFlags_Float);
					ImGui::ColorEdit4("Color 2", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevColors[2].r, ImGuiColorEditFlags_Float);
					ImGui::ColorEdit4("Color 3", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevColors[3].r, ImGuiColorEditFlags_Float);
					ImGui::Unindent();

					ImGui::PopID();
				}
				ImGui::Unindent();
				
				ImGui::Spacing();
				*/

				ImGui::Indent();
				if (ImGui::CollapsingHeader("Konst Colors")) {
					ImGui::PushID("konstColor");

					ImGui::Indent();
					ImGui::ColorEdit4("Color 0", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[0].r, ImGuiColorEditFlags_Float);
					ImGui::ColorEdit4("Color 1", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[1].r, ImGuiColorEditFlags_Float);
					ImGui::ColorEdit4("Color 2", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[2].r, ImGuiColorEditFlags_Float);
					ImGui::ColorEdit4("Color 3", &mMaterials[mCurrentMaterialIndex]->TevBlock->mTevKonstColors[3].r, ImGuiColorEditFlags_Float);
					ImGui::Unindent();

					ImGui::PopID();
				}
				ImGui::Unindent();
			}
			ImGui::PopID();

			ImGui::Spacing();

			ImGui::PushID("TexGenBlock");
			if (ImGui::CollapsingHeader("TexGen Block")) {
				ImGui::Indent();
				if (ImGui::CollapsingHeader("Tex Coord Generators")) {
					RenderTexGenList();
				}
				ImGui::Unindent();

				ImGui::Spacing();

				ImGui::Indent();
				if (ImGui::CollapsingHeader("Tex Matrices")) {
					RenderTexMatrixList();
				}
				ImGui::Unindent();
			}
			ImGui::PopID();

			ImGui::Spacing();

			ImGui::PushID("LightBlock");
			if (ImGui::CollapsingHeader("Light Block")) {
				ImGui::Spacing();
				ImGui::Indent();

				if (ImGui::CollapsingHeader("Color Channels")) {
					ImGui::Indent();

					RenderColorChannelList();

					ImGui::Unindent();
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader("Material Colors")) {
					ImGui::Indent();

					ImGui::ColorEdit4("Material Color 0", &mMaterials[mCurrentMaterialIndex]->LightBlock.mMatteColor[0].x, ImGuiColorEditFlags_Float);
					ImGui::ColorEdit4("Material Color 1", &mMaterials[mCurrentMaterialIndex]->LightBlock.mMatteColor[1].x, ImGuiColorEditFlags_Float);

					ImGui::Unindent();
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader("Ambient Colors")) {
					ImGui::Indent();

					ImGui::ColorEdit4("Ambient Color 0", &mMaterials[mCurrentMaterialIndex]->LightBlock.mAmbientColor[0].x, ImGuiColorEditFlags_Float);
					ImGui::ColorEdit4("Ambient Color 1", &mMaterials[mCurrentMaterialIndex]->LightBlock.mAmbientColor[1].x, ImGuiColorEditFlags_Float);

					ImGui::Unindent();
				}

				ImGui::Unindent();
			}
			ImGui::PopID();

			ImGui::Spacing();

			ImGui::PushID("PixelEngine");
			if (ImGui::CollapsingHeader("Pixel Engine")) {
				J3DPixelEngineBlock* pixelEngine = &mMaterials[mCurrentMaterialIndex]->PEBlock;

				ImGui::Indent();

				ImGui::Checkbox("Dithering Enabled", &pixelEngine->mDither);
				ImGui::Checkbox("ZCompLoc", &pixelEngine->mZCompLoc);

				ImGui::Spacing();
				ImGui::Spacing();

				if (ImGui::CollapsingHeader("Alpha Compare")) {
					ImGui::PushID("alphaCompare");
					
					ImGui::Indent();

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

					ImGui::Unindent();

					ImGui::PopID();
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader("Blend Mode")) {
					ImGui::PushID("blendMode");

					ImGui::Indent();

					UIUtil::RenderComboEnum<EGXBlendMode>("Type", pixelEngine->mBlendMode.Type);

					ImGui::Spacing();

					UIUtil::RenderComboEnum<EGXBlendModeControl>("Source Factor", pixelEngine->mBlendMode.SourceFactor);
					UIUtil::RenderComboEnum<EGXBlendModeControl>("Dest Factor", pixelEngine->mBlendMode.DestinationFactor);

					ImGui::Spacing();

					UIUtil::RenderComboEnum<EGXLogicOp>("Operation", pixelEngine->mBlendMode.Operation);

					ImGui::Spacing();

					ImGui::Unindent();

					ImGui::PopID();
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader("Fog")) {
					ImGui::PushID("fog");

					ImGui::Indent();

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

					ImGui::Unindent();

					ImGui::PopID();
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader("Z-Mode")) {
					ImGui::PushID("zMode");

					ImGui::Indent();

					ImGui::Checkbox("Enabled", &pixelEngine->mZMode.Enable);

					ImGui::Spacing();

					if (pixelEngine->mZMode.Enable) {
						UIUtil::RenderComboEnum<EGXCompareType>("Compare Type", pixelEngine->mZMode.Function);
						ImGui::Checkbox("Updates Enabled", &pixelEngine->mZMode.UpdateEnable);
					}

					ImGui::Spacing();

					ImGui::Unindent();

					ImGui::PopID();
				}

				ImGui::Unindent();
			}
			ImGui::PopID();

			ImGui::Unindent();
		}
	}

	ImGui::End();
}

void UMaterializerContext::RenderPanels(float deltaTime) {
	SetUpDocking();

	RenderMainWindow(deltaTime);

	for (int i = 0; i < mPanels.size(); i++) {
		if (!mPanels[i]->IsOpen()) {
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
