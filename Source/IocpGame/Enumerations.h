// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum HostTypeEnum : uint8
{
	NONE,
	CLIENT_HEADLESS,
	CLIENT,
	LOGIC_SERVER_HEADLESS,
	LOGIC_SERVER,
	MIDDLEMAN_SERVER,
};
