// Fill out your copyright notice in the Description page of Project Settings.


#include "CASInterface.h"

#include "CleverAdsSolutions.h"

UCASInterface* UCASInterface::GetCAS()
{
	return FCleverAdsSolutionsModule::Get().CASInterface;
}
