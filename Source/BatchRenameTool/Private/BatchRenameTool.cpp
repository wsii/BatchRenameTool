// Copyright Epic Games, Inc. All Rights Reserved.

#include "BatchRenameTool.h"

#include "BatchRenameToolStyle.h"
#include "BatchRenameToolCommands.h"

#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "ToolMenus.h"
#include "Styling/StyleColors.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "SBatchRenameMain.h"

static const FName BatchRenameToolTabName("BatchRenameTool");


#define LOCTEXT_NAMESPACE "FBatchRenameToolModule"

void FBatchRenameToolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	
	FBatchRenameToolStyle::Initialize();
	FBatchRenameToolStyle::ReloadTextures();

	FBatchRenameToolCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FBatchRenameToolCommands::Get().OpenTool,
		FExecuteAction::CreateRaw(this, &FBatchRenameToolModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBatchRenameToolModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(BatchRenameToolTabName, FOnSpawnTab::CreateRaw(this, &FBatchRenameToolModule::OnSpawnPluginTab))
	.SetDisplayName(LOCTEXT("FBatchRenameToolTabTitle", "BatchRenameTool"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FBatchRenameToolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FBatchRenameToolStyle::Shutdown();

	FBatchRenameToolCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BatchRenameToolTabName);
}

TSharedRef<SDockTab> FBatchRenameToolModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FBatchRenameToolModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("BatchRenameTool.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SBatchRenameMain)
		];
	
}


void FBatchRenameToolModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(BatchRenameToolTabName);
}


void FBatchRenameToolModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FBatchRenameToolCommands::Get().OpenTool, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FBatchRenameToolCommands::Get().OpenTool));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBatchRenameToolModule, BatchRenameTool)