#include "erm/loaders/fbx/FBXModelLoader.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-pedantic-errors"
#pragma clang diagnostic ignored "-pedantic"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wextra-semi"
#include <fbxsdk.h>
#pragma clang diagnostic pop

namespace {

	int numTabs = 0;

}

namespace erm {

	void PrintNode(FbxNode* pNode);
	void PrintTabs();
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
	void PrintAttribute(FbxNodeAttribute* pAttribute);

	void ParseFBXModel(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		Materials& materials)
	{
		(void)mutex;
		(void)stop;
		(void)model;
		(void)materials;

		FbxManager* lSdkManager = FbxManager::Create();

		FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);

		FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
		auto t = static_cast<FbxIOBase*>(lImporter)->GetStatus();

		if (!lImporter->Initialize(path, -1, lSdkManager->GetIOSettings()))
		{
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		}

		FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
		lImporter->Import(lScene);
		lImporter->Destroy();

		FbxNode* lRootNode = lScene->GetRootNode();
		if (lRootNode)
		{
			for (int i = 0; i < lRootNode->GetChildCount(); ++i)
				PrintNode(lRootNode->GetChild(i));
		}

		lSdkManager->Destroy();
	}

	void PrintNode(FbxNode* pNode)
	{
		PrintTabs();
		const char* nodeName = pNode->GetName();
		FbxDouble3 translation = pNode->LclTranslation.Get();
		FbxDouble3 rotation = pNode->LclRotation.Get();
		FbxDouble3 scaling = pNode->LclScaling.Get();

		// Print the contents of the node.
		printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n", nodeName, translation[0], translation[1], translation[2], rotation[0], rotation[1], rotation[2], scaling[0], scaling[1], scaling[2]);
		numTabs++;

		// Print the node's attributes.
		for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
			PrintAttribute(pNode->GetNodeAttributeByIndex(i));

		// Recursively print the children.
		for (int j = 0; j < pNode->GetChildCount(); j++)
			PrintNode(pNode->GetChild(j));

		numTabs--;
		PrintTabs();
		printf("</node>\n");
	}

	void PrintTabs()
	{
		for (int i = 0; i < numTabs; i++)
			printf("\t");
	}

	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type)
	{
		switch (type)
		{
			case FbxNodeAttribute::eUnknown:
				return "unidentified";
			case FbxNodeAttribute::eNull:
				return "null";
			case FbxNodeAttribute::eMarker:
				return "marker";
			case FbxNodeAttribute::eSkeleton:
				return "skeleton";
			case FbxNodeAttribute::eMesh:
				return "mesh";
			case FbxNodeAttribute::eNurbs:
				return "nurbs";
			case FbxNodeAttribute::ePatch:
				return "patch";
			case FbxNodeAttribute::eCamera:
				return "camera";
			case FbxNodeAttribute::eCameraStereo:
				return "stereo";
			case FbxNodeAttribute::eCameraSwitcher:
				return "camera switcher";
			case FbxNodeAttribute::eLight:
				return "light";
			case FbxNodeAttribute::eOpticalReference:
				return "optical reference";
			case FbxNodeAttribute::eOpticalMarker:
				return "marker";
			case FbxNodeAttribute::eNurbsCurve:
				return "nurbs curve";
			case FbxNodeAttribute::eTrimNurbsSurface:
				return "trim nurbs surface";
			case FbxNodeAttribute::eBoundary:
				return "boundary";
			case FbxNodeAttribute::eNurbsSurface:
				return "nurbs surface";
			case FbxNodeAttribute::eShape:
				return "shape";
			case FbxNodeAttribute::eLODGroup:
				return "lodgroup";
			case FbxNodeAttribute::eSubDiv:
				return "subdiv";
			default:
				return "unknown";
		}
	}

	/**
     * Print an attribute.
     */
	void PrintAttribute(FbxNodeAttribute* pAttribute)
	{
		if (!pAttribute)
			return;

		FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
		FbxString attrName = pAttribute->GetName();
		PrintTabs();
		// Note: to retrieve the character array of a FbxString, use its Buffer() method.
		printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
	}

} // namespace erm
