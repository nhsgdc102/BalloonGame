// Copyright 2020 Northview High School Game Development Club


#include "CP_MainMenu.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "TextBlock.h"
#include "CP_BestRecord.h"
#include "Kismet/GameplayStatics.h"
#include "CP_GameInstance.h"

// Called after widget is constructed
void UCP_MainMenu::NativeConstruct()
{
	/*Synchronously loads saved data for leaderboard*/
	FString SlotName = UCP_BestRecord::GetSlotName();
	int32 UserIndex = UCP_BestRecord::GetUserIndex();

	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		UCP_BestRecord* BestRecordObj = Cast<UCP_BestRecord, USaveGame>
			(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
		if (BestRecordObj)
		{
			int32 HighestRound = BestRecordObj->GetHighestRound();
			int32 HighScore = BestRecordObj->GetHighScore();
			TopRound->SetText(FText::FromString(FString::FromInt(HighestRound)));
			TopScore->SetText(FText::FromString(FString::FromInt(HighScore)));
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("UCP_BestRecord save game not found!"));
	}

	/*Calls blueprint construct event*/
	Super::NativeConstruct();
}

// Helper function to handle modifying title 
FText UCP_MainMenu::GetScrolledTitleText(const int32 InScrollPosition, const int32 InTitleStyle) const
{
	FString TitleString = "BALLOONATIC";
	TitleString =
		TitleString.Left(InScrollPosition) + // Portion of title left of scroll position
		"<" + FString::FromInt(InTitleStyle) + ">" + // Inserts style escape sequence
		TitleString.Mid(InScrollPosition, 1) + // The character at the scroll position
		"</>" + // Inserts ending escape sequence
		TitleString.RightChop(InScrollPosition + 1); // Portion of title to the right (excluding the character at scroll position)
	return FText::FromString(TitleString);
}

// Conviently hides or displays a widget based on its current visibility
void UCP_MainMenu::ToggleVisibility(TArray<UWidget*> InWidgetArray)
{
	for(UWidget* InWidget : InWidgetArray)
	{
		if (InWidget) { // Checks for null pointers
			if (InWidget->IsVisible())
				InWidget->SetVisibility(ESlateVisibility::Hidden);
			else
				InWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible); // Visible but cannot interact with cursor
		}
	}
}

// Conviently clears a list of timer handles
void UCP_MainMenu::ClearTimerHandles(TArray<FTimerHandle> InTimerHandleArray)
{
	for (FTimerHandle InTimerHandle : InTimerHandleArray)
		GetWorld()->GetTimerManager().ClearTimer(InTimerHandle);
}

/*
// Helper function to fill animation map with widget animations
void UCP_MainMenu::FillAnimationMap()
{
	AnimationMap.Empty();

	/*Stores first property that this class is linked to in blueprints
	UProperty* Prop = GetClass()->PropertyLink;

	/*Goes through all properties
	while (Prop != nullptr)
	{
		/*Checks if property is of an object type (Animations are of this type)
		if (Prop->GetClass() == UObjectProperty::StaticClass())
		{
			/*This cast must work because of the conditional above
			UObjectProperty* UObjectProp = Cast<UObjectProperty, UProperty>(Prop);

			/*Filters out anything but animations
			if (UObjectProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				/*Stores the animation object inside the UObjectProperty
				UObject* Obj = UObjectProp->GetPropertyValue_InContainer(this);

				UWidgetAnimation* WidgetAnim = Cast<UWidgetAnimation, UObject>(Obj);

				/*Double checks validity
				if (WidgetAnim != nullptr && WidgetAnim->MovieScene != nullptr)
				{
					FName AnimName = WidgetAnim->MovieScene->GetFName();
					AnimationMap.Add(AnimName, WidgetAnim);
				}
			}
		}

		/*Goes to next property for the next loop
		Prop = Prop->PropertyLinkNext;
	}
}
*/

/*
// Getter function for retrieving corresponding animation
UWidgetAnimation* UCP_MainMenu::GetAnimationByName(FName AnimName) const
{
	/* Pointer to a const pointer to a widget animation->https://www.geeksforgeeks.org/difference-between-const-int-const-int-const-and-int-const/
	UWidgetAnimation* const* WidgetAnim = AnimationMap.Find(AnimName);
	if (WidgetAnim)
		return *WidgetAnim;
	else
		return nullptr;
}
*/
/*
// Wrapper function for playing widget; handles validation
bool UCP_MainMenu::PlayAnimationByName(FName AnimName,
	float StartAtTime,
	int32 NumLoopsToPlay,
	EUMGSequencePlayMode::Type PlayMode,
	float PlaybackSpeed)
{
	UWidgetAnimation* WidgetAnim = GetAnimationByName(AnimName);
	if (WidgetAnim)
	{
		PlayAnimation(WidgetAnim, StartAtTime,
			NumLoopsToPlay, PlayMode, PlaybackSpeed);
		return true;
	}
	return false;
}
*/