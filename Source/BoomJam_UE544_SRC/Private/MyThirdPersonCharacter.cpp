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
	// ��ȡ��ǰ��ɫλ��
	FVector fv3TmpCharLoc = GetActorLocation();

	// ��ȡ������ʼλ��
	FVector fv3CapsuleTraceStart = fv3TmpCharLoc;

	// ��ȡ���߽���λ��
	FVector fv3CapsuleTraceEnd = (fINInputActValue * 50 * fv3INVector) + fv3TmpCharLoc;
	// Ҫ���ԵĶ�������
	TArray<AActor*> IgnoreActors;

	FHitResult fhrTmpHit;

	FVector fv3ClimbPredictLoc;
	// GetWorld()���ڻ�ȡ��ǰActor��Component���ڵ�World����
	// World������UE5�е�һ�����ĸ������������Ϸ����Ļ��������������е�����Actor�����Ρ����ա���Ч��
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
	// ��ǰ��ɫ�Ľ������λ����ClimbTest��λ����֪���ʴ���λ����������
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

	// ���ִ�к���Ҫ�ص�������������ֻ������ActionInfo.CallbackTarget = this;����
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
		// ���ִ�к���Ҫ�ص�������������ֻ������ActionInfo.CallbackTarget = this;����
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
	// Ҫ���ԵĶ�������
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
		// ���ִ�к���Ҫ�ص�������������ֻ������ActionInfo.CallbackTarget = this;����
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
	// ��ȡ��ǰ��ɫλ��
	FVector fv3TmpCharLoc = GetActorLocation();

	// ��ȡ����������ʼλ��
	FVector fv3HipsLineTraceStart = fv3TmpCharLoc + fv3INMoveDist;
	// ��ȡ�������߳�������
	FVector fv3HipsLineTraceLen = GetActorForwardVector() * nLineLen;
	// ��ȡ�������߽���λ��
	FVector fv3HipsLineTraceEnd = fv3HipsLineTraceStart + fv3HipsLineTraceLen;
	
	// ��ɫ���������߼����
	FHitResult fhrTmpHipsHit;

	// Ҫ���Ե�Actor�б�
	FCollisionQueryParams QueryParams;
	// ���Ա���(ͷ��������������׷�ٶ�����)
	QueryParams.AddIgnoredActor(this);

	// Ҫ���ԵĶ�������
	TArray<AActor*> IgnoreActors;
	// ��ʼ����������׷��
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
		// ������������׷�ٵĽ��
		fhrHipsHit = fhrTmpHipsHit;

		// ��ɫͷ�������߼����
		FHitResult fhrTmpHeadHit;

		// ����ͷ�����λ������
		fv3HeadLoc = GetMesh()->GetSocketLocation("head");
		// ��ȡͷ��������ʼλ��
		FVector fv3HeadLineTraceStart = fv3HeadLoc + fv3INMoveDist;
		// ��ȡͷ�����߳�������
		FVector fv3HeadLineTraceLen = GetActorForwardVector() * nLineLen;
		// ��ȡͷ�����߽���λ��
		FVector fv3HeadLineTraceEnd = fv3HeadLineTraceStart + fv3HeadLineTraceLen;

		// ��ʼͷ��������׷��
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
			// ����ͷ������׷�ٵĽ��
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

