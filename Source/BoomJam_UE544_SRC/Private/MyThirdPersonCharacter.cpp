// Fill out your copyright notice in the Description page of Project Settings.


#include "MyThirdPersonCharacter.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
//#include "GameFramework/NavMovementComponent.h"
//#include "GameFramework/MovementComponent.h"
//#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMyThirdPersonCharacter::AMyThirdPersonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	nLineLen = 200;
	fClimbMoveDist = 50;
	fOnWallOffset = 30;
	bClimbing = false;
}

// Called when the game starts or when spawned
void AMyThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyThirdPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMyThirdPersonCharacter::ClimbMovement(double fINInputActValue, FVector fv3INVector)
{
	// 获取当前角色位置
	FVector fv3TmpCharLoc = GetActorLocation();

	// 获取射线起始位置
	FVector fv3CapsuleTraceStart = fv3TmpCharLoc;

	// 获取射线结束位置
	FVector fv3CapsuleTraceEnd = (fINInputActValue * 50 * fv3INVector) + fv3TmpCharLoc;
	// 要忽略的对象数组
	TArray<AActor*> IgnoreActors;

	FHitResult fhrTmpHit;

	FVector fv3ClimbPredictLoc;
	// GetWorld()用于获取当前Actor或Component所在的World对象
	// World对象是UE5中的一个核心概念，它代表了游戏世界的环境，包括场景中的所有Actor、地形、光照、音效等
	if (UKismetSystemLibrary::CapsuleTraceSingle(
			GetWorld(),
			fv3CapsuleTraceStart,
			fv3CapsuleTraceEnd,
			20,
			GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
			false,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
			fhrTmpHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5
	)) {
		fv3ClimbPredictLoc = fhrTmpHit.Location;
	}
	else {
		fv3ClimbPredictLoc = fv3CapsuleTraceEnd;
	}
	fv3ClimbPredictLoc -= GetActorLocation();
	// 当前角色的胶囊体的位置在ClimbTest中位置已知，故传入位移向量即可
	FHitResult fhrHipsHit;
	FHitResult fhrHeadHit;
	FVector fv3HeadLoc;
	bool bHeadHit;
	if (ClimbTest(fv3ClimbPredictLoc, fhrHipsHit, fhrHeadHit, bHeadHit, fv3HeadLoc)) {
		if (bHeadHit) {
			//FVector fv3Direction = ((fhrHipsHit.Location + fhrHipsHit.Normal) - GetActorLocation()).GetSafeNormal();
			FVector fv3Direction = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), (fhrHipsHit.Location + fhrHipsHit.Normal * fOnWallOffset));
			AddMovementInput(fv3Direction);
			SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), UKismetMathLibrary::MakeRotFromX(fhrHipsHit.Normal * -1), GetWorld()->GetDeltaSeconds(), 2.0f));
		}
		else {
			FVector fv3TmpVec;
			fv3TmpVec.ZeroVector;
			fv3TmpVec.Z = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			ClimbToTop((fv3HeadLoc + fv3TmpVec));
		}
	}
	
}

void AMyThirdPersonCharacter::ClimbStart()
{
	if (bClimbing) {
		StopClimb();
	}
	FLatentActionInfo tmpActionInfo;

	// 如果执行后不需要回调函数，必须且只需设置ActionInfo.CallbackTarget = this;即可
	tmpActionInfo.CallbackTarget = this;
	tmpActionInfo.ExecutionFunction = "ClimbStartAssit";
	tmpActionInfo.UUID = FMath::Rand();
	tmpActionInfo.Linkage = 0;

	UKismetSystemLibrary::Delay(GetWorld(), 0.2, tmpActionInfo);

}

void AMyThirdPersonCharacter::ClimbStartAssit()
{
	FVector fv3MoveDist = { 0, 0, 0 };
	FHitResult fhrHipsHit;
	FHitResult fhrHeadHit;
	bool bHeadHit = false;
	FVector fv3HeadLoc = { 0, 0, 0 };
	if (ClimbTest(fv3MoveDist, fhrHipsHit, fhrHeadHit, bHeadHit, fv3HeadLoc)) {
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetCharacterMovement()->StopMovementImmediately();
		FVector fv3HitLoc = fhrHeadHit.Location;
		FVector fv3HitLocNorm = fhrHeadHit.Normal;
		FRotator frotActorRot = GetActorRotation();
		FVector fv3TarLoc = fv3HitLoc + fv3HitLocNorm * 30;
		FRotator frotTarRot;
		frotTarRot.Roll = frotActorRot.Roll;
		frotTarRot.Pitch = frotActorRot.Pitch;
		frotTarRot.Yaw = (UKismetMathLibrary::MakeRotFromX(fv3HitLocNorm * -1)).Yaw;
		FLatentActionInfo tmpActionInfo;
		// 如果执行后不需要回调函数，必须且只需设置ActionInfo.CallbackTarget = this;即可
		tmpActionInfo.CallbackTarget = this;
		tmpActionInfo.ExecutionFunction = "";
		tmpActionInfo.UUID = FMath::Rand();
		tmpActionInfo.Linkage = 0;
		UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), fv3TarLoc, frotTarRot, false, false, 0.2, false, EMoveComponentAction::Type::Move, tmpActionInfo);
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bClimbing = true;
	}
}

void AMyThirdPersonCharacter::ClimbToTop(FVector fv3INTopLoc)
{
	FVector fv3TraceEnd = GetActorForwardVector() * fClimbMoveDist + fv3INTopLoc;
	FVector fv3TraceStart = fv3INTopLoc;
	// 要忽略的对象数组
	TArray<AActor*> IgnoreActors;
	FHitResult fhrTmpHit;
	if (!UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(),
		fv3TraceStart,
		fv3TraceEnd,
		20,
		30,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,
		fhrTmpHit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5
	)) {
		StopClimb();
		FLatentActionInfo tmpActionInfo;
		// 如果执行后不需要回调函数，必须且只需设置ActionInfo.CallbackTarget = this;即可
		tmpActionInfo.CallbackTarget = this;
		tmpActionInfo.ExecutionFunction = "";
		tmpActionInfo.UUID = FMath::Rand();
		tmpActionInfo.Linkage = 0;
		UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), fv3TraceEnd, GetActorRotation(), false, false, 0.2, false, EMoveComponentAction::Type::Move, tmpActionInfo);
		GetCharacterMovement()->StopMovementImmediately();
	}
}

void AMyThirdPersonCharacter::StopClimb()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bClimbing = false;
}

bool AMyThirdPersonCharacter::ClimbTest(FVector fv3INMoveDist, FHitResult& fhrHipsHit, FHitResult& fhrHeadHit, bool& bHeadHit, FVector& fv3HeadLoc)
{
	// 获取当前角色位置
	FVector fv3TmpCharLoc = GetActorLocation();

	// 获取腰部射线起始位置
	FVector fv3HipsLineTraceStart = fv3TmpCharLoc + fv3INMoveDist;
	// 获取腰部射线长度向量
	FVector fv3HipsLineTraceLen = GetActorForwardVector() * nLineLen;
	// 获取腰部射线结束位置
	FVector fv3HipsLineTraceEnd = fv3HipsLineTraceStart + fv3HipsLineTraceLen;
	
	// 角色腰部的射线检测结果
	FHitResult fhrTmpHipsHit;

	// 要忽略的Actor列表
	FCollisionQueryParams QueryParams;
	// 忽略本身(头部和腰部的射线追踪都适用)
	QueryParams.AddIgnoredActor(this);

	// 要忽略的对象数组
	TArray<AActor*> IgnoreActors;
	// 开始腰部的射线追踪
	if (UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		fv3HipsLineTraceStart,
		fv3HipsLineTraceEnd,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,
		fhrTmpHipsHit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5
	)) {
		// 保留腰部射线追踪的结果
		fhrHipsHit = fhrTmpHipsHit;

		// 角色头部的射线检测结果
		FHitResult fhrTmpHeadHit;

		// 保留头部插槽位置坐标
		fv3HeadLoc = GetMesh()->GetSocketLocation("head");
		// 获取头部射线起始位置
		FVector fv3HeadLineTraceStart = fv3HeadLoc + fv3INMoveDist;
		// 获取头部射线长度向量
		FVector fv3HeadLineTraceLen = GetActorForwardVector() * nLineLen;
		// 获取头部射线结束位置
		FVector fv3HeadLineTraceEnd = fv3HeadLineTraceStart + fv3HeadLineTraceLen;

		// 开始头部的射线追踪
		if (UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			fv3HeadLineTraceStart,
			fv3HeadLineTraceEnd,
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
			false,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
			fhrTmpHeadHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5
		)) {
			// 保留头部射线追踪的结果
			fhrHeadHit = fhrTmpHeadHit;
			bHeadHit = true;
		}
		else {
			bHeadHit = false;
		}
		DrawDebugLine(GetWorld(), fv3HeadLineTraceStart, fv3HeadLineTraceEnd, FColor::Red);
		DrawDebugLine(GetWorld(), fv3HipsLineTraceStart, fv3HipsLineTraceEnd, FColor::Red);
		return true;
	}
	
	DrawDebugLine(GetWorld(), fv3HipsLineTraceStart, fv3HipsLineTraceEnd, FColor::Red);
	return false;
}

