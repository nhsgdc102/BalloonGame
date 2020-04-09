// Copyright 2020 Northview High School Game Development Club


#include "CP_MainMenu.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "RichTextBlock.h"
#include "TextBlock.h"
#include "HorizontalBox.h"
#include "CP_BestRecord.h"
#include "Kismet/GameplayStatics.h"

// Constructor--Sets default values
UCP_MainMenu::UCP_MainMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/*Title color scroll*/
	LetterSwitchInterval = 0.5f;
	StyleSwitchInterval = 2.f;
	NumberOfExtraStyles = 3;
	HighlightPosition = 0;
	StyleIndex = 0;

	/*Leaderboard Transition*/
	TransitionDelay = 2.f;
	DisplayTime = 5.f;
}

// Called after widget is constructed
void UCP_MainMenu::NativeConstruct()
{
	TitleText = Title->GetText().ToString();
	/*Starts letter switching*/
	GetWorld()->GetTimerManager().SetTimer<UCP_MainMenu>(LetterSwitch_TimerHandle,
		this, &UCP_MainMenu::SwitchHighlightLetter, LetterSwitchInterval, true, 4.f);

	/*Setup for controlling animations*/
	FillAnimationMap();

	/*Starts prompt flashing animation. 0 loops means indefinite loop*/
	PlayAnimationByName(TEXT("FlashingPrompt"), 0.f, 0, EUMGSequencePlayMode::Forward, 1.f);

	/*Async load saved data for leaderboard*/
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindUObject(this, &UCP_MainMenu::OnAsyncLoadRecord);
	UGameplayStatics::AsyncLoadGameFromSlot(UCP_BestRecord::GetSlotName(),
		UCP_BestRecord::GetUserIndex(), LoadedDelegate);

	/*Calls blueprint construct event*/
	Super::NativeConstruct();
}

// Called when widget is destroyed
void UCP_MainMenu::NativeDestruct()
{
	/*Clears timers*/
	GetWorld()->GetTimerManager().ClearTimer(LetterSwitch_TimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(StyleSwitch_TimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(LeaderboardDisplay_TimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(LeaderboardHide_TimerHandle);

	/*Calls blueprint destruct event*/
	Super::NativeDestruct();
}

// Helper function to fill animation map with widget animations
void UCP_MainMenu::FillAnimationMap()
{
	AnimationMap.Empty();

	/*Stores first property that this class is linked to in blueprints*/
	UProperty* Prop = GetClass()->PropertyLink; 

	/*Goes through all properties*/
	while (Prop != nullptr)
	{
		/*Checks if property is of an object type (Animations are of this type)*/
		if (Prop->GetClass() == UObjectProperty::StaticClass())
		{
			/*This cast must work because of the conditional above*/
			UObjectProperty* UObjectProp = Cast<UObjectProperty, UProperty>(Prop);

			/*Filters out anything but animations*/
			if (UObjectProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				/*Stores the animation object inside the UObjectProperty*/
				UObject* Obj = UObjectProp->GetPropertyValue_InContainer(this);

				UWidgetAnimation* WidgetAnim = Cast<UWidgetAnimation, UObject>(Obj);

				/*Double checks validity*/
				if (WidgetAnim != nullptr && WidgetAnim->MovieScene != nullptr)
				{
					FName AnimName = WidgetAnim->MovieScene->GetFName();
					AnimationMap.Add(AnimName, WidgetAnim);
				}
			}
		}

		/*Goes to next property for the next loop*/
		Prop = Prop->PropertyLinkNext;
	}
}

// Getter function for retrieving corresponding animation
UWidgetAnimation* UCP_MainMenu::GetAnimationByName(FName AnimName) const
{
	/* Pointer to a const pointer to a widget animation->https://www.geeksforgeeks.org/difference-between-const-int-const-int-const-and-int-const/ */
	UWidgetAnimation* const* WidgetAnim = AnimationMap.Find(AnimName);
	if (WidgetAnim)
		return *WidgetAnim;
	else
		return nullptr;
}

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

// Retrieves leaderboard data
void UCP_MainMenu::OnAsyncLoadRecord(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	UCP_BestRecord* BestRecordObj = Cast<UCP_BestRecord, USaveGame>(LoadedGameData);
	if (IsValid(BestRecordObj))
	{
		int32 HighestRound = BestRecordObj->GetHighestRound();
		int32 HighScore = BestRecordObj->GetHighScore();

		TopRound->SetText(FText::FromString(FString::FromInt(HighestRound)));
		TopScore->SetText(FText::FromString(FString::FromInt(HighScore)));
	}
}

// Switches highlighted letter in title
void UCP_MainMenu::SwitchHighlightLetter()
{
	/*Checks if the scrolling is done*/
	if (HighlightPosition == TitleText.Len())
	{
		/*Resets highlight scrolling*/
		Title->SetText(FText::FromString(TitleText));
		HighlightPosition = 0;

		/*Stops the loop that switches the highlighted letter*/
		GetWorld()->GetTimerManager().ClearTimer(LetterSwitch_TimerHandle);

		/*Starts the timer to switch the highlight style*/
		GetWorld()->GetTimerManager().SetTimer<UCP_MainMenu>(StyleSwitch_TimerHandle,
			this, &UCP_MainMenu::SwitchHighlightStyle, StyleSwitchInterval, false);
	}
	else {
		FString TitleCopy = TitleText;

		/*Inserts ending sequence.*/
		if(HighlightPosition == TitleText.Len() - 1)
			TitleCopy.Append("</>");
		else
			TitleCopy.InsertAt(HighlightPosition + 1, "</>");

		/*Inserts style before character*/
		TitleCopy.InsertAt(HighlightPosition, "<" + FString::FromInt(StyleIndex) + ">");

		Title->SetText(FText::FromString(TitleCopy));
		HighlightPosition++;
	}
}

// Switches style of highlighting in title
void UCP_MainMenu::SwitchHighlightStyle()
{
	StyleIndex++;
	/*Occurs when all styles have played already*/
	if (StyleIndex == NumberOfExtraStyles) 
	{
		StyleIndex = 0;

		/*Transitions to leaderboard with delay*/
		GetWorld()->GetTimerManager().SetTimer<UCP_MainMenu>(LeaderboardDisplay_TimerHandle,
			this, &UCP_MainMenu::DisplayLeaderboard, TransitionDelay, false);
	}
	else 
		/*Restarts letter switching*/
		GetWorld()->GetTimerManager().SetTimer<UCP_MainMenu>(LetterSwitch_TimerHandle,
			this, &UCP_MainMenu::SwitchHighlightLetter, LetterSwitchInterval, true);
}

// Plays leaderboard transition animation
void UCP_MainMenu::DisplayLeaderboard()
{
	PlayAnimationByName(TEXT("LeaderboardTransition"), 0.f, 1,
		EUMGSequencePlayMode::Forward, 1.f);

	/*Sets timer to end leaderboard display*/
	GetWorld()->GetTimerManager().SetTimer<UCP_MainMenu>(LeaderboardHide_TimerHandle,
		this, &UCP_MainMenu::HideLeaderboard, DisplayTime, false);
}

// Reverses leaderboard transition animation
void UCP_MainMenu::HideLeaderboard()
{
	/*Reverses animation to display title*/
	PlayAnimationByName(TEXT("LeaderboardTransition"), 0.f, 1,
		EUMGSequencePlayMode::Reverse, 1.f);

	/*Starts letter switching again with 2 second delay*/
	GetWorld()->GetTimerManager().SetTimer<UCP_MainMenu>(LetterSwitch_TimerHandle,
		this, &UCP_MainMenu::SwitchHighlightLetter, LetterSwitchInterval, true, 2.f);
}

