// Fill out your copyright notice in the Description page of Project Settings.


#include "SBatchRenameTabMain.h"

#include "AssetToolsModule.h"
#include "SlateOptMacros.h"
#include "Styling/StyleColors.h"

#include "BatchRenameToolModel.h"
#include "BlueprintActionDatabase.h"
#include "EditorUtilityLibrary.h"
#include "IAssetTools.h"
#include "SBatchRenamingOperationList.h"
#include "SBatchRenamingOperationDetails.h"


static const FName AssetTableTabId("BatchRenameEditorToolkit_AssetTable");
static const FName MethodListTabId("BatchRenameEditorToolkit_MethodList");
static const FName OperationDetailsTabId("BatchRenameEditorToolkit_OperationDetails");

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "BatchRenameTool"

SBatchRenameTabMain::SBatchRenameTabMain():
	Model(MakeShared<FBatchRenameToolModel>())
{
}

void SBatchRenameTabMain::Construct(const FArguments& InArgs)
{

	//原来的分页：
	const TSharedRef<SDockTab> NomadTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);

	//创建TabManager
	if (!TabManager.IsValid())
	{
		TabManager = FGlobalTabmanager::Get()->NewTabManager(NomadTab);
	}
	
	//创建布局：
	if (!TabLayout.IsValid())
	{
		TabLayout = FTabManager::NewLayout("TestLayoutWindow")
			->AddArea
			(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.7f)
				->SetHideTabWell(true)
				->AddTab(AssetTableTabId, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.3f)
				->Split
				(
				FTabManager::NewStack()
					->SetSizeCoefficient(0.6f)
					->SetHideTabWell(true)
					->AddTab(MethodListTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.4f)
					->SetHideTabWell(true)
					->AddTab(OperationDetailsTabId, ETabState::OpenedTab)
			)
		)
			);
	
	}

	//AssetTableTabId的内容：
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(AssetTableTabId, FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs& SpawnTabArgs)
	{
		return
			SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			[

				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.AutoWidth()
					[
						ConstructOperationButton()
					]
				]
				+SVerticalBox::Slot()

				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.AutoHeight()
				[
					ConstructAssetTable()
				]
				
			];
	}))
	.SetDisplayName(LOCTEXT("AssetTableTab", "AssetTableTab"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);

	//MethodListTabId的内容：
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MethodListTabId, FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs& SpawnTabArgs)
		{
			return
				SNew(SDockTab)
				.TabRole(ETabRole::NomadTab)
				[
					SAssignNew(OperationList, SBatchRenamingOperationList, Model)
				];
		}))
		.SetDisplayName(LOCTEXT("MethodListTab", "MethodListTab"))
			.SetMenuType(ETabSpawnerMenuType::Hidden);
	
	//OperationDetailsTabId的内容：
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(OperationDetailsTabId, FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs& SpawnTabArgs)
		{
			return
				SNew(SDockTab)
				.TabRole(ETabRole::NomadTab)
				[
					SNew(SBatchRenamingOperationDetails, Model)
				];
		}))
		.SetDisplayName(LOCTEXT("OperationDetailsTab", "OperationDetailsTab"))
			.SetMenuType(ETabSpawnerMenuType::Hidden);

	// TSharedRef<SWidget> TabContents = TabManager->RestoreFrom(TabLayout.ToSharedRef(), TSharedPtr<SWindow>()).ToSharedRef();
	ChildSlot
	[
		// Populate the widget
		TabManager->RestoreFrom(TabLayout.ToSharedRef(), TSharedPtr<SWindow>()).ToSharedRef()
	];

}

FString SBatchRenameTabMain::ApplyOperations(const FString& Str) const
{
	FString StrCopy = Str;
	for(const TObjectPtr<UBatchRenamingOperation>& Rule: Model->GetOperations())
	{
		StrCopy = Rule->Apply(StrCopy);
	}
	return StrCopy;
}

void SBatchRenameTabMain::OnOperationsChanged()
{
	for (const TSharedPtr<FAssetTableRowData>& Asset : AssetTableRows)
	{
		Asset->NewFilename = ApplyOperations(Asset->Filename);
		UE_LOG(LogTemp, Warning, TEXT("Selected Asset %s"), *Asset->NewFilename);
	}
	AssetTable->SetRows(AssetTableRows);
	
	AssetTable->Refresh();
}

void SBatchRenameTabMain::RefreshAssetTableRows()
{
	//清空 AssetTableRows
	AssetTableRows.Empty();
	for (FAssetData& AssetData : HasSelectedAssets)
	{
		UObject* ActualAsset = AssetData.GetAsset();
		if (ActualAsset)
		{
			TSharedPtr<FAssetTableRowData> RowData = MakeShared<FAssetTableRowData>(ActualAsset->GetName(),ActualAsset->GetName(), ActualAsset->GetPathName());
			
			AssetTableRows.Add(RowData);
			
		}
	}
	
	AssetTable->SetRows(AssetTableRows);
	//重新 应用修改
	OnOperationsChanged();
	
	if(AssetTable.IsValid())
	{
		AssetTable->Refresh();
	}
}
//添加table上面的按钮
TSharedRef<SHorizontalBox> SBatchRenameTabMain::ConstructOperationButton()
{
	Model->OnOperationListModified().AddSP(this, &SBatchRenameTabMain::OnOperationsChanged);

	TSharedRef<SHorizontalBox> HorizontalButton =
		SNew(SHorizontalBox)

		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		[
			SNew(SButton)
			.ContentPadding(FMargin(FVector2D(0.f, 5.f)))
			.OnClicked_Raw(this,&SBatchRenameTabMain::OnAddAssets)
			[
				SNew(SHorizontalBox)
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SImage)
					// .Image(FAppStyle::Get().GetBrush(TEXT("DataTableEditor.Add.Small")))
					.Image(FAppStyle::Get().GetBrush("Icons.Plus"))
					.ColorAndOpacity(FStyleColors::AccentGreen)
				]
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("添加文件")))
				]
			]
		]
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		[
			SNew(SButton)
			.ContentPadding(FMargin(FVector2D(0.f, 5.f)))
			.OnClicked_Raw(this, &SBatchRenameTabMain::OnDeleteSelect)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SImage)
					// .Image(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Minus"))
					.Image(FAppStyle::Get().GetBrush(TEXT("Icons.Minus")))
					.ColorAndOpacity(FStyleColors::AccentBlue)
					
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[

					SNew(STextBlock).Text(FText::FromString(TEXT("移除选中项")))

				]
			]
		]

		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		[
			SNew(SButton)
			.ContentPadding(FMargin(FVector2D(0.f, 5.f)))
			.OnClicked_Raw(this, &SBatchRenameTabMain::OnClearTable)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SImage)

					.Image(FAppStyle::Get().GetBrush(TEXT("Icons.Delete")))
					.ColorAndOpacity(FStyleColors::AccentYellow)
					
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(FMargin(3.0f,0.f,0.f,0.f))
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("清空列表")))
				]
			]
		]

		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		[
			SNew(SButton)
			.ContentPadding(FMargin(FVector2D(0.f, 5.f)))
			.OnClicked_Raw(this, &SBatchRenameTabMain::Run)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SImage)
					// .Image(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Play"))
					.Image(FAppStyle::Get().GetBrush(TEXT("Icons.Play")))
					.ColorAndOpacity(FStyleColors::AccentRed)
					
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(FMargin(3.0f,0.f,0.f,0.f))
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("运行")))
				]
			]
		];
		return HorizontalButton;
}

TSharedRef<SAssetTable> SBatchRenameTabMain::ConstructAssetTable()
{
	SAssignNew(AssetTable, SAssetTable, AssetTableRows);
	
	return AssetTable.ToSharedRef();
}

FReply SBatchRenameTabMain::OnAddAssets()
{
	SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();

	for (FAssetData& AssetData : SelectedAssets)
	{
		UObject* ActualAsset = AssetData.GetAsset();
		if (ActualAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("Selected Asset %s"), *ActualAsset->GetName());
			if (!HasSelectedAssets.Contains(AssetData))
			{
				HasSelectedAssets.Add(AssetData);
			}
		}
	}

	RefreshAssetTableRows();

	return FReply::Handled();
}

FReply SBatchRenameTabMain::OnDeleteSelect()
{
	SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();

	for (FAssetData& AssetData : SelectedAssets)
	{
		UObject* ActualAsset = AssetData.GetAsset();
		if (ActualAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("Remove Asset %s"), *ActualAsset->GetName());
			if (HasSelectedAssets.Contains(AssetData))
			{
				HasSelectedAssets.Remove(AssetData);
			}
		}
	}
	RefreshAssetTableRows();

	return FReply::Handled();
}

FReply SBatchRenameTabMain::OnClearTable()
{
	AssetTableRows.Empty();
	AssetTable->SetRows(AssetTableRows);

	HasSelectedAssets.Empty();
	
	if(AssetTable.IsValid())
	{
		AssetTable->Refresh();
	}
	return FReply::Handled();
}

FReply SBatchRenameTabMain::Run()
{
	if(!Model->GetOperations().IsEmpty())
	{
		const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		TArray<FAssetRenameData> AssetRenameData;

		for (FAssetData& AssetData : HasSelectedAssets)
		{
			UObject* Asset = AssetData.GetAsset();
			const FString PackagePath = FPackageName::GetLongPackagePath(Asset->GetPackage()->GetName());
			AssetRenameData.Add(FAssetRenameData(Asset, PackagePath, ApplyOperations(Asset->GetName())));
			
		}
		//更新资产命名
		AssetToolsModule.Get().RenameAssets(AssetRenameData);
	}
	// 清空列表
	OnClearTable();

	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE