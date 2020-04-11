// Copyright 2020 Northview High School Game Development Club


#include "CP_MainMenu.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "RichTextBlock.h"
#include "TextBlock.h"
#include "HorizontalBox.h"
#include "VerticalBox.h"
#include "Button.h"
#include "CP_BestRecord.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "CP_GameInstance.h"

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

	/*Options Menu*/
	ButtonHighlightInterval = 1.f;
	ButtonHighlightIndex = 0;
}

// Called after initialization
void UCP_MainMenu::NativeOnInitialized()
{
	/*Button bindings*/
	NewGameButton->OnReleased.AddDynamic(this, &UCP_MainMenu::OnNewGame);
	ResumeButton->OnReleased.AddDynamic(this, &UCP_MainMenu::OnResumeGame);
	GoBackButton->OnReleased.AddDynamic(this, &UCP_MainMenu::OnGoBack);
	CreditsButton->OnReleased.AddDynamic(this, &UCP_MainMenu::OnShowCredits);

	Super::NativeOnInitialized();
}

// Called after widget is constructed
void UCP_MainMenu::NativeConstruct()
{
	TitleText = Title->GetText().ToString();
	/*Starts letter switching*/
	GetWorld()->GetTimerManager().SetTimer<UCP_MainMenu>(LetterSwitch_TimerHandle,
		this, &UCP_MainMenu::SwitchHighlightedLetter, LetterSwitchInterval, true, 4.f);

	/*Setup for controlling animations*/
	FillAnimationMap();

	/*Starts prompt flashing animation. 0 loops means indefinite loop*/
	PlayAnimationByName(TEXT("FlashingPrompt"), 0.f, 0, EUMGSequencePlayMode::Forward, 1.f);

	/*Async load saved data for leaderboard*/
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindUObject(this, &UCP_MainMenu::OnAsyncLoadRecord);
	UGameplayStatics::AsyncLoadGameFromSlot(UCP_BestRecord::GetSlotName(),
		UCP_BestRecord::GetUserIndex(), LoadedDelegate);

	/*Setup for options menu*/
	OptionButtons.Add(NewGameButton); OptionTextBlocks.Add(NewGameText);
	OptionButtons.Add(ResumeButton); OptionTextBlocks.Add(ResumeText);
	OptionButtons.Add(GoBackButton); OptionTextBlocks.Add(GoBackText);
	OptionButtons.Add(CreditsButton); OptionTextBlocks.Add(CreditsText);

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
void UCP_MainMenu::SwitchHighlightedLetter()
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
			this, &UCP_MainMenu::SwitchHighlightedLetter, LetterSwitchInterval, true);
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
		this, &UCP_MainMenu::SwitchHighlightedLetter, LetterSwitchInterval, true, 2.f);
}

// Handles main menu touch functionality to get to options menu
FReply UCP_MainMenu::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	/*Checks whether or not options is visible*/
	if (OptionsMenu->IsVisible())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not opening options menu"));
		/*Performs base functionality to avoid interfering with buttons*/
		return FReply::Unhandled();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Opening options menu"));
		/*Returns handled and options options menu*/
		OpenOptionsMenu();
		return FReply::Handled();
	}
}

/*Options Menu*/
// Opens options menu, hides title screen
void UCP_MainMenu::OpenOptionsMenu()
{
	/*Stops all title-screen related functionality*/
	Title->SetVisibility(ESlateVisibility::Hidden);
	Prompt->SetVisibility(ESlateVisibility::Hidden);
	Leaderboard->SetVisibility(ESlateVisibility::Hidden);

	GetWorld()->GetTimerManager().ClearTimer(LetterSwitch_TimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(StyleSwitch_TimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(LeaderboardDisplay_TimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(LeaderboardHide_TimerHandle);

	UWidgetAnimation* FlashingPromptAnim = GetAnimationByName("FlashingPrompt");
	if (FlashingPromptAnim)
		StopAnimation(FlashingPromptAnim);

	/*Makes options menu visible and starts highlighting*/
	OptionsMenu->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	GetWorld()->GetTimerManager().SetTimer<UCP_MainMenu>(ButtonSwitch_TimerHandle,
		this, &UCP_MainMenu::SwitchHiglightedOption, ButtonHighlightInterval, true, 2.f);

	/*Also makes sure balloons are unpopped*/
	ReplaceBalloonImages(false);
}

// Hides options menu, opens title screen
void UCP_MainMenu::CloseOptionsMenu()
{
	/*Hides options menu*/
	OptionsMenu->SetVisibility(ESlateVisibility::Hidden);
	GetWorld()->GetTimerManager().ClearTimer(ButtonSwitch_TimerHandle);

	/*Restarts title functionality*/
	Title->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Prompt->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Leaderboard->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	GetWorld()->GetTimerManager().SetTimer<UCP_MainMenu>(LetterSwitch_TimerHandle,
		this, &UCP_MainMenu::SwitchHighlightedLetter, LetterSwitchInterval, true, 4.f);
	PlayAnimationByName(TEXT("FlashingPrompt"), 0.f, 0, EUMGSequencePlayMode::Forward, 1.f);
}

// Switch highlighted option
void UCP_MainMenu::SwitchHiglightedOption()
{
	/*Removes previous highlight (no matter if one exists)*/
	if (ButtonHighlightIndex == 0)
		HighlightButton(OptionButtons.Num() - 1, false);
	else
		HighlightButton(ButtonHighlightIndex - 1, false);

	/*Highlights current button*/
	HighlightButton(ButtonHighlightIndex, true);

	/*Moves to next button*/
	ButtonHighlightIndex++;
	if (ButtonHighlightIndex >= OptionButtons.Num())
		ButtonHighlightIndex = 0;
}

// Helper function for highlighting (or removing highlight) on a button
void UCP_MainMenu::HighlightButton(int32 ButtonIndex, bool bHighlight)
{
	if (OptionButtons.IsValidIndex(ButtonIndex) && OptionTextBlocks.IsValidIndex(ButtonIndex))
	{
		UButton* TargetButton = OptionButtons[ButtonIndex];
		UTextBlock* TargetText = OptionTextBlocks[ButtonIndex];

		if (IsValid(TargetButton) && IsValid(TargetText))
		{
			if (bHighlight)
			{
				/*Sets button background opaque*/
				TargetButton->SetBackgroundColor(
					TargetButton->BackgroundColor.CopyWithNewOpacity(1.f));

				/*Makes text white*/
				TargetText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
			}
			else
			{
				/*Sets background transparent*/
				TargetButton->SetBackgroundColor(
					TargetButton->BackgroundColor.CopyWithNewOpacity(0.f));

				/*Makes text black*/
				TargetText->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
			}
		}
	}
}

/*Button-linked events*/
// Starts a new game by opening the starting level
void UCP_MainMenu::OnNewGame_Implementation()
{
	/*No necessary code here*/
}

// Checks for a save game and before starting
void UCP_MainMenu::OnResumeGame_Implementation()
{
	UCP_GameInstance* GameInstanceObj = GetWorld()->GetGameInstance<UCP_GameInstance>();
	if (GameInstanceObj)
	{
		if (GameInstanceObj->LoadSaveState())
			/*OnNewGame() handles opening next level*/
			OnNewGame();
		else
			/*Fill body with code for error pop-up*/;
	}
}

// Opens title screen back up
void UCP_MainMenu::OnGoBack_Implementation()
{
	CloseOptionsMenu();
}

// Opens up credits
void UCP_MainMenu::OnShowCredits_Implementation()
{
	/*Fill body*/
}