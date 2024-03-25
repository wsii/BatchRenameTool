// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BatchRenameToolModel.h"
#include "SAssetTable.h"
#include "SBatchRenamingOperationList.h"
#include "Widgets/SCompoundWidget.h"


class FToolBarBuilder;
class FMenuBuilder;

class UBatchRenamingOperationFactory;
class SBatchRenamingOperationList;
class SAssetTable;
struct FAssetTableRowData;
class FBatchRenameToolModel;
/**
 * 
 */
class SBatchRenameTabMain : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBatchRenameTabMain)
	{}
	SLATE_END_ARGS()

	SBatchRenameTabMain();
	
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
	TSharedPtr<class FTabManager> TabManager;
	
	//Tab的布局：
	TSharedPtr<FTabManager::FLayout> TabLayout;
	

	FString ApplyOperations(const FString& Str) const;
	
	void OnOperationsChanged();

	void RefreshAssetTableRows();
	
	TSharedRef<SHorizontalBox> ConstructOperationButton();
	
	TSharedPtr<class FUICommandList> PluginCommands;

	TArray<FAssetData> SelectedAssets;
	TArray<FAssetData> HasSelectedAssets;

	TSharedRef<FBatchRenameToolModel> Model;
	
	TSharedPtr<SBatchRenamingOperationList> OperationList;

	TSharedPtr<SAssetTable> AssetTable;
	TArray<TSharedPtr<FAssetTableRowData>> AssetTableRows;

	TSharedRef<SAssetTable> ConstructAssetTable();
	
	FReply OnAddAssets();
	FReply OnDeleteSelect();
	FReply OnClearTable();
	FReply Run();
	

};
