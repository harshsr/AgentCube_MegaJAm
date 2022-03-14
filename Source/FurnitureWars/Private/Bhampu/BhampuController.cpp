// Fill out your copyright notice in the Description page of Project Settings.


#include "FurnitureWars/Public/Bhampu/BhampuController.h"

ABhampuController::ABhampuController()
{
	
	bDashed = false;
	bStealthOverlap = false;
	bStealthWarping = false;
	bInteractionAvailable = false;
}

void ABhampuController::BeginPlay()
{
	Super::BeginPlay();

	BhampuTagsInterface = Cast<IBhampuCppInterface>(GetPawn());
	DashAbilityCooldownTime = BhampuTagsInterface->GetDashAbilityCooldownTime();
}

void ABhampuController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(bDashed)
	{
		DashAbilityCooldownTime = DashAbilityCooldownTime - DeltaSeconds;
		if(DashAbilityCooldownTime <0.0f)
		{
			DashAbilityCooldownTime = BhampuTagsInterface->GetDashAbilityCooldownTime();
			bDashed = false;
		}

		UIDashUpdate(DashAbilityCooldownTime/BhampuTagsInterface->GetDashAbilityCooldownTime());
	}

	
	if(BhampuTagsInterface && BhampuTagsInterface->GetBhampuTags().HasTagExact(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetStealthTagName())) && !bStealthWarping)
	{
		// TODO magic number in following if
		if(GetPawn()->GetVelocity().Size()>20.0f)
		{
			// TODO debug message
			GEngine->AddOnScreenDebugMessage(-1,2.0,FColor::Silver,FString::SanitizeFloat(GetPawn()->GetVelocity().Size()),false);
			
			UnHide();
		}
	}
	
	if(BhampuTagsInterface && BhampuTagsInterface->GetBhampuTags().HasTagExact(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetInteractionAvailableTagName())))
	{
		bInteractionAvailable = true;
	}
	else
	{
		bInteractionAvailable = false;
	}
	PromptInteractionUI(bInteractionAvailable);
}

void ABhampuController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis(FName("ForwardAxis"),this,&ABhampuController::ForwardAxis);
	InputComponent->BindAxis(FName("RightAxis"),this,&ABhampuController::RightAxis);
	InputComponent->BindAxis(FName("YawAxis"),this,&ABhampuController::YawAxis);
	InputComponent->BindAxis(FName("PitchAxis"),this,&ABhampuController::PitchAxis);
	InputComponent->BindAction(FName("Jump"),IE_Pressed,this,&ABhampuController::JumpAction);
	InputComponent->BindAction(FName("Dash"),IE_Pressed,this,&ABhampuController::DashAction);
	InputComponent->BindAction(FName("Hide"),IE_Pressed,this,&ABhampuController::HideAction);
	InputComponent->BindAction(FName("Interact"),IE_Pressed,this,&ABhampuController::InteractionAction);
}

void ABhampuController::ForwardAxis(float AxisValue)
{
	if(BhampuTagsInterface && !bStealthWarping)
	{
		if(AxisValue>0.1f || AxisValue<(-0.1f))
		{
			BhampuTagsInterface->GetBhampuTags().AddTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetLongitudinalMoveTagName()));
		}
		else
		{
			BhampuTagsInterface->GetBhampuTags().RemoveTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetLongitudinalMoveTagName()));
		}
		
		BhampuTagsInterface->SetForwardAxisVal(AxisValue);
	}
	
}

void ABhampuController::RightAxis(float AxisValue)
{
	if(BhampuTagsInterface && !bStealthWarping)
	{
		if(AxisValue>0.1f || AxisValue<(-0.1f))
		{
			BhampuTagsInterface->GetBhampuTags().AddTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetLateralMoveTagName()));
		}
		else
		{
			BhampuTagsInterface->GetBhampuTags().RemoveTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetLateralMoveTagName()));
		}
		BhampuTagsInterface->SetRightAxisVal(AxisValue);
	}
}

void ABhampuController::YawAxis(float AxisValue)
{
	if(BhampuTagsInterface)
	{
		if(AxisValue>0.1f || AxisValue < (-0.1f))
		{
			BhampuTagsInterface->GetBhampuTags().AddTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetYawMoveTagName(),false));
		}
		else
		{
			BhampuTagsInterface->GetBhampuTags().RemoveTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetYawMoveTagName(),false));
		}
		
		BhampuTagsInterface->SetYawAxisVal(AxisValue);
	}
}

void ABhampuController::PitchAxis(float AxisValue)
{
	if(BhampuTagsInterface)
	{
		if(AxisValue>0.1f || AxisValue < (-0.1f))
		{
			BhampuTagsInterface->GetBhampuTags().AddTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetCameraPitchTagName(),false));
		}
		else
		{
			BhampuTagsInterface->GetBhampuTags().RemoveTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetCameraPitchTagName(),false));
		}
		
		BhampuTagsInterface->SetPitchAxisVal(AxisValue);
	}
}

void ABhampuController::JumpAction()
{
	if(BhampuTagsInterface)
	{
		if(BhampuTagsInterface->GetBhampuTags().HasTagExact(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetJumpTagName())))
		{}
		else
		{
			BhampuTagsInterface->GetBhampuTags().AddTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetJumpTagName()));
		}
	}
}

void ABhampuController::DashAction()
{
	if(!bDashed)
	{
		if(BhampuTagsInterface)
		{
			if(BhampuTagsInterface->GetBhampuTags().HasTagExact(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetDashTagName())))
			{}
			else
			{
				BhampuTagsInterface->GetBhampuTags().AddTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetDashTagName()));
				bDashed = true;
			}
		}
	}

}

void ABhampuController::HideAction()
{
	if(bStealthOverlap && BhampuTagsInterface)
	{
		if(BhampuTagsInterface->GetBhampuTags().HasTagExact(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetStealthTagName())))
		{}
		else
		{
			BhampuTagsInterface->GetBhampuTags().RemoveTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetLateralMoveTagName()));
			BhampuTagsInterface->GetBhampuTags().RemoveTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetLongitudinalMoveTagName()));

\
			ReadyToHide();
		}
	}
}

void ABhampuController::UnHide_Implementation()
{
	BhampuTagsInterface->GetBhampuTags().RemoveTag(FGameplayTag::RequestGameplayTag(BhampuTagsInterface->GetStealthTagName()));
}


void ABhampuController::InteractionAction()
{
	if(bInteractionAvailable)
	{
		BhampuTagsInterface->PassInteraction();
	}
}

