#include "FurnitureWars/Public/Bhampu/Bhampu.h"

#include "FurnitureWars/Public/Interfaces/InteractionSys.h"
// Sets default values
ABhampu::ABhampu()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Components

	BaseCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComponent"));
	RootComponent = BaseCollision;
	
	BaseSkelMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BhampuMesh"));
	BaseSkelMesh->SetupAttachment(RootComponent);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeRotation(FRotator(-45.0f,0.0f,0.0f));

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	MainCamera->SetupAttachment(CameraBoom);

	// Custom physic delegate
	OnCalculateCustomPhysics.BindUObject(this,&ABhampu::CustomPhysics);

	// tag names
	LongitudinalMoveTagName = "Movement.Directional.Longitudinal";
	LateralMoveTagName = "Movement.Directional.Lateral";
	YawMoveTagName = "Movement.Directional.Yaw";
	CameraPitchTagName = "Movement.Directional.Pitch";
	JumpTagName = "Movement.Special.Jump";
	DashTagName = "Movement.Special.Dash";
	StealthTagName = "State.Stealth";
	InteractionAvailableTagName = "Available.Interact";
	NeutralAreaTagName = "Area.Neutral";
	HostileAreaTagName = "Area.Hostile";

	// float properties
	LongForce = 500000.0f;
	VelocityMagnitudeClamp = 1000.0f;
	CameraSpeed = 0.01f;
	JumpImpulseMagnitude = 350000.0f;
	JumpTraceThreshold = 55.0f;
	DashCooldown = 1.0f;
	DashImpulse = 250.0f;
	DashVelocityClamp = 2000.0f;
	CurrentVelocityClamp = 1000.0f;
	DashCurrentCooldown = 1.0f;
	DashCooldownFactor = 0.5f;
	DashAbilityCooldown = 5.0f;
	InteractionCheckDistance = 250.0f;
	InteractionTraceSphereRadius = 100.0f;

	// Bool properties
	bJumped = false;

	// pointers
	InteractableActor = nullptr;

}

// Called when the game starts or when spawned
void ABhampu::BeginPlay()
{
	Super::BeginPlay();

	// set dash variables
	DashCurrentCooldown = DashCooldown;
	CurrentVelocityClamp = VelocityMagnitudeClamp;
	DashCooldownFactor = DashVelocityClamp - VelocityMagnitudeClamp;
	
}

// Called every frame
void ABhampu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Call physics tick on BaseMesh
	if (BaseCollision->GetBodyInstance())
	{
		BaseCollision->GetBodyInstance()->AddCustomPhysics(OnCalculateCustomPhysics);
	}

	// Check for interactable actors every tick
	FindBestInteractableActor();
}

// Call physics tick from this delegate listener function
void ABhampu::CustomPhysics(float DeltaTime, FBodyInstance* BodyInstance)
{
	PhysicsTick(DeltaTime, *BodyInstance);
}


void ABhampu::PhysicsTick_Implementation(float SubStepDeltaTime, FBodyInstance& PhysicsBodyInstance)
{
	const FTransform BhampuTransform = PhysicsBodyInstance.GetUnrealWorldTransform_AssumesLocked();

	// longitudinal physics movement
	if(BhampuTags.HasTagExact(FGameplayTag::RequestGameplayTag(LongitudinalMoveTagName,false)))
	{
		PhysicsBodyInstance.AddForce(BhampuTransform.GetRotation().GetForwardVector()*ForwardAxisValue*LongForce,false,false);
	}

	// lateral physics movement
	if(BhampuTags.HasTagExact(FGameplayTag::RequestGameplayTag(LateralMoveTagName,false)))
	{
		PhysicsBodyInstance.AddForce(BhampuTransform.GetRotation().GetRightVector()*RightAxisValue*LongForce,false,false);
	}

	// yaw actor movement
	if(BhampuTags.HasTagExact(FGameplayTag::RequestGameplayTag(YawMoveTagName,false)))
	{
		BaseCollision->AddRelativeRotation(FRotator(0.0f,YawAxisValue*CameraSpeed/SubStepDeltaTime,0.0f),false);
	}

	// pitch camera movement
	if(BhampuTags.HasTagExact(FGameplayTag::RequestGameplayTag(CameraPitchTagName,false)))
	{
		CameraBoom->AddRelativeRotation(FRotator(PitchAxisValue*CameraSpeed/SubStepDeltaTime,0.0f,0.0f),false);
		FRotator BoomRotation = CameraBoom->GetComponentRotation();
		BoomRotation.Pitch = FMath::ClampAngle(BoomRotation.Pitch,-60.0f,10.0f);
		CameraBoom->SetWorldRotation(BoomRotation);
	}

	// jump
	if(BhampuTags.HasTagExact(FGameplayTag::RequestGameplayTag(JumpTagName,false)))
	{
		if(!bJumped)
		{
			// after passing all the above checks, check if we are actually on ground
			FHitResult JumpHit;
			const FVector JumpTraceEnd = GetActorLocation()+(JumpTraceThreshold*(-1.0)*BaseCollision->GetUpVector());

			// ECC_GameTraceChannel1 is the JumpTrace channel. Check DefaultEngine.ini
			if(GetWorld()->LineTraceSingleByChannel(JumpHit,GetActorLocation(),JumpTraceEnd,ECC_GameTraceChannel1))
			{
				
				if(JumpHit.bBlockingHit)
				{
					// Finally, jump.
					PhysicsBodyInstance.AddImpulse(JumpImpulseMagnitude*BhampuTransform.GetRotation().GetUpVector(),false);
					IBhampuCppInterface::Execute_JumpAnim(this);
					bJumped = true;
				}
				
				//TODO Debug message
				//GEngine->AddOnScreenDebugMessage(-1,1,FColor::Blue,FString::SanitizeFloat(JumpHit.Distance));
			}
		}
		
		else
		{
			//TODO Debug message
			//GEngine->AddOnScreenDebugMessage(-1,1,FColor::Magenta,FString("Has Jumped"));
		}
	}
	// whether jump happened or not, get rid of the tag
	BhampuTags.RemoveTag(FGameplayTag::RequestGameplayTag(JumpTagName,true));

	// dash
	if(BhampuTags.HasTagExact(FGameplayTag::RequestGameplayTag(DashTagName,false)))
	{
		//TODO This if statement might give problems if SubStepDeltaTime varies at low frame rates (which it will)
		if(DashCurrentCooldown>DashCooldown - SubStepDeltaTime)
		{
			//TODO magic number in following if
			// only dash if actor has at least some velocity. Otherwise I am getting weird jumps on dash
			if (PhysicsBodyInstance.GetUnrealWorldVelocity_AssumesLocked().Size()>20.0f)
			{
				PhysicsBodyInstance.AddImpulse(DashImpulse * PhysicsBodyInstance.GetUnrealWorldVelocity_AssumesLocked().GetSafeNormal(), true);
				CurrentVelocityClamp = DashVelocityClamp;
				TriedDash(true);
			}
			else
			{
				TriedDash(false);
			}
			// No matter if we dashed or not, change DashCurrentCooldown so we don't enter the loop 2nd time.
			DashCurrentCooldown = DashCurrentCooldown - SubStepDeltaTime;
		}
		else if(DashCurrentCooldown<SubStepDeltaTime)
		{
			DashCurrentCooldown = DashCooldown;
			CurrentVelocityClamp = VelocityMagnitudeClamp;
			BhampuTags.RemoveTag(FGameplayTag::RequestGameplayTag(DashTagName));
		}
		else
		{
			// We enter this branch under the "cooldown" process.
			DashCurrentCooldown = DashCurrentCooldown - SubStepDeltaTime;
			// We reduce velocity clamp slowly down to VelocityMagnitudeCamp using following equation.
			CurrentVelocityClamp = CurrentVelocityClamp - ((DashCooldownFactor) * (SubStepDeltaTime/DashCooldown));
		}
	}

	// world velocity clamp
	ClampVelocity();
	
	// Prevent camera roll and unnecessary yaw.
	CameraBoom->SetWorldRotation(FRotator(CameraBoom->GetComponentRotation().Pitch,BaseCollision->GetComponentRotation().Yaw,0.0f));
}

// clamp velocity in x and y direction
void ABhampu::ClampVelocity() const
{
	FVector CurrentVel = BaseCollision->GetBodyInstance()->GetUnrealWorldVelocity();
	FVector NewCurrentVel = CurrentVel;
	if(CurrentVel.Size()>CurrentVelocityClamp)
	{

		CurrentVel = CurrentVel.GetClampedToMaxSize(CurrentVelocityClamp);
		NewCurrentVel = FVector(CurrentVel.X,CurrentVel.Y,NewCurrentVel.Z);
		
		BaseCollision->GetBodyInstance()->SetLinearVelocity(NewCurrentVel,false);
	}
}


// Interface implementations
FName ABhampu::GetLongitudinalMoveTagName()
{
	return  LongitudinalMoveTagName;
}
FName ABhampu::GetLateralMoveTagName()
{
	return LateralMoveTagName;
}
FName ABhampu::GetYawMoveTagName()
{
	return YawMoveTagName;
}
FName ABhampu::GetCameraPitchTagName()
{
	return CameraPitchTagName;
}
FName ABhampu::GetJumpTagName()
{
	return  JumpTagName;
}
FName ABhampu::GetDashTagName()
{
	return DashTagName;
}
FName ABhampu::GetStealthTagName()
{
	return StealthTagName;
}
FName ABhampu::GetInteractionAvailableTagName()
{
	return InteractionAvailableTagName;
}
FGameplayTagContainer& ABhampu::GetBhampuTags()
{
	return BhampuTags;
}
void ABhampu::SetForwardAxisVal(float AxisValue)
{
	ForwardAxisValue = AxisValue;
}
void ABhampu::SetRightAxisVal(float AxisValue)
{
	RightAxisValue = AxisValue;
}
void ABhampu::SetYawAxisVal(float AxisValue)
{
	YawAxisValue = AxisValue;
}
void ABhampu::SetPitchAxisVal(float AxisValue)
{
	PitchAxisValue = AxisValue;
}
float ABhampu::GetDashAbilityCooldownTime()
{
	return DashAbilityCooldown;
}


// Interaction
void ABhampu::FindBestInteractableActor()
{
	// to store hit actors
	TArray<AActor*> HitActors;

	// trace hit results
	TArray<FHitResult> OutHits;

	// shape for trace
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(InteractionTraceSphereRadius);

	//The trace
	// ECC_GameTraceChannel2 is InteractionTrace. This just to avoid hitting the GroundObject
	GetWorld()->SweepMultiByChannel(OutHits,GetActorLocation(),(GetActorLocation()+(RootComponent->GetForwardVector()*InteractionCheckDistance)),FQuat::Identity,ECC_GameTraceChannel2,CollisionSphere);

	if(OutHits.Num()>0)
	{
		for (FHitResult OutHit : OutHits)
		{
			// Interaction interface filter
			if(OutHit.Actor->Implements<UInteractionSys>())
			{
				HitActors.Add(OutHit.Actor.Get());
			}
		}

		if(HitActors.Num()>0)
		{
			// Just to be safe, store first from HitActors to InteractableActor
			InteractableActor = HitActors.Top();
			
			// for distance filter
			float MinDistance = InteractionCheckDistance;
			for (AActor* HitActor : HitActors)
			{
				if(MinDistance>= FVector::Distance(GetActorLocation(),HitActor->GetActorLocation()))
				{
					MinDistance = FVector::Distance(GetActorLocation(),HitActor->GetActorLocation());
					InteractableActor = HitActor;
				}
			}

			if (InteractableActor)
			{
				// add the interaction ability tag so that the controller can prompt for interaction.
				BhampuTags.AddTag(FGameplayTag::RequestGameplayTag(InteractionAvailableTagName));
			}
			else
			{
				// If not, remove interaction tag
				BhampuTags.RemoveTag(FGameplayTag::RequestGameplayTag(InteractionAvailableTagName));
			}
		}
		else
		{
			// If not, remove interaction tag
			BhampuTags.RemoveTag(FGameplayTag::RequestGameplayTag(InteractionAvailableTagName));
		}
	
	}
	else
	{
		// If not, remove interaction tag
		BhampuTags.RemoveTag(FGameplayTag::RequestGameplayTag(InteractionAvailableTagName));
	}
}


// Upon receiving interact input in controller,
// tell this to Bhampu so that Bhampu calls Interact() on the available interaction actor
void ABhampu::PassInteraction()
{
	if(InteractableActor)
	{
		IInteractionSys::Execute_Interact(InteractableActor);
	}
}


