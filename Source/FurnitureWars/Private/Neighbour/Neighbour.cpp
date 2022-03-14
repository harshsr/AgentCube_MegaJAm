// Fill out your copyright notice in the Description page of Project Settings.


#include "Neighbour/Neighbour.h"

#include "DrawDebugHelpers.h"
#include "Interfaces/BhampuCppInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ANeighbour::ANeighbour()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// component initialization
	DialogWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DialogRender"));
	DialogWidget->SetupAttachment(RootComponent);
	
	SlopeInterpolateSpeed = 10.0f;
	FrontSightRadius = 1500.0f;
	BackSightRadius = 350.0f;
	FrontSightConeAngle = 90.0f;
	bSeeingCustom = false;

	// Initialize here. Assign in BeginPlay
	PawnRef = nullptr;
	PlayerControllerRef = nullptr;
	
}

// Called when the game starts or when spawned
void ANeighbour::BeginPlay()
{
	Super::BeginPlay();

	// Assign to PlayerControllerRe
	PlayerControllerRef = UGameplayStatics::GetPlayerController(this,0);
	// Assign to PawnRef
	if(PlayerControllerRef)
	{
		PawnRef = PlayerControllerRef->GetPawn();
	}
}

// Called every frame
void ANeighbour::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// fix alignment every frame
	FixAlignmentOnSlopes(DeltaTime);

	// force DialogWidget to face camera
	DialogFaceToCamera();

	// custom sight perception
	CustomSight();

}

void ANeighbour::FixAlignmentOnSlopes( float DeltaTime)
{
	// First we perform a trace to get ground normal direction
	const FVector RotationTraceStart = GetActorLocation();
	const FVector RotationTraceEnd = RotationTraceStart + FVector(0.0f,0.0f,-125.0f);		//TODO magic number

	FHitResult RotationTraceHit;

	if(GetWorld()->LineTraceSingleByChannel(RotationTraceHit,RotationTraceStart,RotationTraceEnd,ECC_GameTraceChannel2)) 
	{
		if(RotationTraceHit.bBlockingHit)
		{
			SetActorRotation(FMath::RInterpTo(GetActorRotation(),FRotationMatrix::MakeFromZX(RotationTraceHit.Normal,RootComponent->GetForwardVector()).Rotator(),DeltaTime,SlopeInterpolateSpeed));
			
			const FVector LocationTraceStart = GetMesh()->GetSocketLocation(RootSocketName);
			const FVector LocationTraceEnd = LocationTraceStart + (GetMesh()->GetUpVector()*(-125.0f));		//TODO magic number
			FHitResult LocationTraceHit;
			
			if(GetWorld()->LineTraceSingleByChannel(LocationTraceHit,LocationTraceStart,LocationTraceEnd,ECC_GameTraceChannel2))
			{
				if(LocationTraceHit.bBlockingHit)
				{
					RootComponent->AddLocalOffset(FVector(0.0f,0.0f,(LocationTraceHit.Distance-2.0f)*(-1)),true);		//TODO magic number
				}
			}
		}
	}
	
}

void ANeighbour::DialogFaceToCamera() const
{
	const FVector CameraLocation = PlayerControllerRef->PlayerCameraManager->GetCameraLocation();
	DialogWidget->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(DialogWidget->GetRelativeLocation() + GetActorLocation(),CameraLocation));
}

void ANeighbour::CustomSight()
{
	FHitResult SightHitResult;

	// trace end is directed toward PawnRef up to FrontSightRadius distance
	 FVector TraceEndLocation;

	// angle between this actor's forward direction and the pawn direction
	const float CurrentConeAngle = FMath::RadiansToDegrees(FMath::Acos(GetDotProductTo(PawnRef)));
	
	
	// Check if the pawn lies in FrontSightConeAngle
	if ((((-1)*(FrontSightConeAngle)< CurrentConeAngle) && (CurrentConeAngle < (FrontSightConeAngle))))
	{
		TraceEndLocation = this->GetActorLocation() + ((PawnRef->GetActorLocation() - this->GetActorLocation()).GetSafeNormal()*FrontSightRadius);
	}
	else
	{
		TraceEndLocation = this->GetActorLocation() + ((PawnRef->GetActorLocation() - this->GetActorLocation()).GetSafeNormal()*BackSightRadius);
	}
	
	// Perform the trace
	if(GetWorld()->LineTraceSingleByChannel(SightHitResult,this->GetActorLocation(),TraceEndLocation,ECC_Visibility))
	{
		if (SightHitResult.bBlockingHit)
		{
			if (SightHitResult.GetActor()->Implements<UBhampuCppInterface>())
			{
				IBhampuCppInterface* JonnaTagsInterface = Cast<IBhampuCppInterface>(PawnRef);
				if (JonnaTagsInterface->GetBhampuTags().HasTag(FGameplayTag::RequestGameplayTag(JonnaTagsInterface->GetStealthTagName())))
				{
					bSeeingCustom = false;
				}
				else
				{
					bSeeingCustom = true;
				}
				
				//GEngine->AddOnScreenDebugMessage(-1,0.001,FColor::Cyan,FString::SanitizeFloat(CurrentConeAngle));
			}
			else
			{
				bSeeingCustom = false;
			}
			DrawDebugLine(GetWorld(),GetActorLocation(),SightHitResult.ImpactPoint,FColor::Yellow,false,-1,0,10);
		}
		else
		{
			bSeeingCustom = false;
			DrawDebugLine(GetWorld(),GetActorLocation(),TraceEndLocation,FColor::Yellow,false,-1,0,10);
		}
	}
	else
	{
		bSeeingCustom = false;
		DrawDebugLine(GetWorld(),GetActorLocation(),TraceEndLocation,FColor::Yellow,false,-1,0,10);
	}
	
	
	
}

