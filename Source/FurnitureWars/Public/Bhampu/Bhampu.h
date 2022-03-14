// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "FurnitureWars/Public/Interfaces/BhampuCppInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Bhampu.generated.h"

UCLASS()
class FURNITUREWARS_API ABhampu : public APawn, public IBhampuCppInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABhampu();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
	Physics Tick Code
	In physics tick, we use two functions because CustomPhysics() cannot be a UFUNCTION(). So, we can't use it in blueprints.
	That's why we call PhysicsTick() through CustomPhysics()
	*/
	// Physics tick
	UFUNCTION(BlueprintNativeEvent , Category = "SubStepping")
	void PhysicsTick(float SubStepDeltaTime, FBodyInstance& PhysicsBodyInstance);
	
	// Custom physics delegate
	FCalculateCustomPhysics OnCalculateCustomPhysics;
	void CustomPhysics(float DeltaTime, FBodyInstance* BodyInstance);


public:
	// Components
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Components")
	UBoxComponent* BaseCollision;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* BaseSkelMesh;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Components")
	USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Components")
	UCameraComponent* MainCamera;


protected:

	// Input axis values
	UPROPERTY(BlueprintReadOnly, Category = "Input Values")
	float ForwardAxisValue;

	UPROPERTY(BlueprintReadOnly, Category = "Input Values")
	float RightAxisValue;

	UPROPERTY(BlueprintReadOnly, Category = "Input Values")
	float YawAxisValue;

	UPROPERTY(BlueprintReadOnly, Category = "Input Values")
	float PitchAxisValue;

	// movement values
	UPROPERTY(EditAnywhere, Category = "Config Movement")
	float LongForce;

	UPROPERTY(EditAnywhere, Category = "Config Movement")
	float VelocityMagnitudeClamp;

	UPROPERTY(EditAnywhere, Category = "Config Camera")
	float CameraSpeed;

	UPROPERTY(EditAnywhere,Category="Config Jump")
	float JumpImpulseMagnitude;

	// Jump variables
	// Jump bool. Restored on ground hit in BP. Used in PhysicsTick
	UPROPERTY(BlueprintReadWrite, Category = "Config Jump")
	bool bJumped;

	// Jump trace distance check
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Config Jump")
	float JumpTraceThreshold;
	
	// Dash variables
	// Dash cooldown time
	UPROPERTY(EditAnywhere, Category = "Config Dash")
	float DashCooldown;

	// Dash current cooldown. Modify this value to compare with DashCooldown. Set in BeginPlay.
	UPROPERTY(BlueprintReadOnly, Category = "Config Dash")
	float DashCurrentCooldown;
	
	// Dash impulse
	UPROPERTY(EditAnywhere, Category = "Config Dash")
	float DashImpulse;

	// Dash velocity magnitude clamp
	UPROPERTY(EditAnywhere,Category = "Config Dash")
	float DashVelocityClamp;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Config Dash")
	float DashAbilityCooldown;


private:
	// Action cool downs
	
	UPROPERTY()
	float CurrentVelocityClamp;

	// dash cooldown factor
	UPROPERTY()
	float DashCooldownFactor;

	// velocity clamp function
	UFUNCTION()
	void ClampVelocity() const;


protected:
	// tag container
	UPROPERTY(BlueprintReadWrite, Category = "GameplayTagsBhampu")
	FGameplayTagContainer BhampuTags;

	// tag names
	UPROPERTY(BlueprintReadOnly, Category = "GameplayTagsBhampu")
	FName LongitudinalMoveTagName;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayTagsBhampu")
	FName LateralMoveTagName;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayTagsBhampu")
	FName YawMoveTagName;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayTagsBhampu")
	FName CameraPitchTagName;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayTagsBhampu")
	FName JumpTagName;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayTagsBhampu")
	FName DashTagName;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayTagsBhampu")
	FName StealthTagName;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayTagsBhampu")
	FName InteractionAvailableTagName;
	
	UPROPERTY(BlueprintReadOnly, Category = "GameplayTagsBhampu")
	FName NeutralAreaTagName;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayTagsBhampu")
	FName HostileAreaTagName;


public:
	// BhampuCppInterface  Begin

	virtual FName GetLongitudinalMoveTagName() override;
	virtual FName GetLateralMoveTagName() override;
	virtual FName GetYawMoveTagName() override;
	virtual FName GetCameraPitchTagName() override;
	virtual FName GetJumpTagName() override;
	virtual FName GetDashTagName() override;
	virtual FName GetStealthTagName() override;
	virtual FName GetInteractionAvailableTagName() override;
	virtual FGameplayTagContainer& GetBhampuTags() override;
	virtual void SetForwardAxisVal(float AxisValue) override;
	virtual void SetRightAxisVal(float AxisValue) override;
	virtual void SetYawAxisVal(float AxisValue) override;
	virtual void SetPitchAxisVal(float AxisValue) override;
	virtual float GetDashAbilityCooldownTime() override;
	virtual void PassInteraction() override;
	

	// BhampuCppInterface  End


protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Dash")
	void TriedDash(bool bDashed);


protected:
	UFUNCTION()
	void FindBestInteractableActor();

	// move trace upto this distance
	UPROPERTY(EditAnywhere, Category = "Config Interaction")
	float InteractionCheckDistance;

	// trace sphere radius
	UPROPERTY(EditAnywhere, Category = "Config Interaction")
	float InteractionTraceSphereRadius;

	// pointer to hold the acceptable interaction actor
	UPROPERTY()
	AActor* InteractableActor;
};
