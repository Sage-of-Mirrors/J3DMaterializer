#pragma once

#include "UCamera.hpp"

#include <vector>
#include <filesystem>
#include <memory>

class UMaterializerUIPanel;
class J3DMaterial;
class UMaterialLayer;

namespace bStream { class CStream; }
class J3DModelData;
class J3DModelInstance;

class UMaterializerContext {
	std::shared_ptr<J3DModelData> mData;
	std::shared_ptr<J3DModelInstance> mInstance;

	std::vector<std::shared_ptr<J3DMaterial>> mMaterials;
	std::vector<std::shared_ptr<UMaterialLayer>> mLayers;

	uint32_t mCurrentMaterialIndex = 0;
	uint32_t mCurrentLayerIndex = 0;

	std::vector<std::shared_ptr<UMaterializerUIPanel>> mPanels;

	USceneCamera mCamera;

	bool bIsDockingSetUp { false };
	uint32_t mMainDockSpaceID;
	uint32_t mDockNodeTopID;
	uint32_t mDockNodeRightID;
	uint32_t mDockNodeDownID;
	uint32_t mDockNodeLeftID;

	bool bIsFileDialogOpen { false };

	void SetUpDocking();

	void RenderLayerList();
	void RenderMaterialList();

	void RenderTevStageList();
	void RenderTexGenList();
	void RenderTexMatrixList();
	void RenderColorChannelList();
	void RenderMainWindow(float deltaTime);
	void RenderPanels(float deltaTime);
	void RenderMenuBar();

	void OpenModelCB();
	void LoadModel(bStream::CStream* stream);
	void LoadModelFromPath(std::filesystem::path filePath);
public:
	UMaterializerContext() {}
	~UMaterializerContext() {}

	bool Update(float deltaTime);
	void Render(float deltaTime);
};
