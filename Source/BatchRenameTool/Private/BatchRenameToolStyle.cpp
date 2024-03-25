// Copyright Epic Games, Inc. All Rights Reserved.

#include "BatchRenameToolStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FBatchRenameToolStyle::StyleInstance = nullptr;

void FBatchRenameToolStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FBatchRenameToolStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FBatchRenameToolStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("BatchRenameToolStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FBatchRenameToolStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("BatchRenameToolStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("BatchRenameTool")->GetBaseDir() / TEXT("Resources"));
	
	// Style->Set("BatchRenameTool.OpenTool", new IMAGE_BRUSH(TEXT("Icon_16"), Icon16x16));
	
	Style->Set("BatchRenameTool.OpenTool", new IMAGE_BRUSH_SVG("Icon", Icon16x16));


	// FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Rename")

	return Style;
}

void FBatchRenameToolStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FBatchRenameToolStyle::Get()
{
	return *StyleInstance;
}
