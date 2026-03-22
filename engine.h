#pragma once
#include <windows.h>

#include "unitysdk/Class_2_C53551EA56D92DA8.h" // BattleResultTask

#include "unitysdk/RPG/Client/AdventurePhase.h"
#include "unitysdk/RPG/Client/BaseShaderPropertyTransition.h"
#include "unitysdk/RPG/Client/BattleGamePhase.h"
#include "unitysdk/RPG/Client/BattleUIUtils.h"
#include "unitysdk/RPG/Client/GamePhaseManager.h"
#include "unitysdk/RPG/Client/GamePlayLockModule.h"
#include "unitysdk/RPG/Client/GlobalVars.h"
#include "unitysdk/RPG/Client/ModuleManager.h"
#include "unitysdk/RPG/Client/RPGApplication.h"
#include "unitysdk/RPG/GameCore/AdventureStatic.h"
#include "unitysdk/RPG/GameCore/BattleInstance.h"
#include "unitysdk/RPG/GameCore/EntityManager.h"
#include "unitysdk/RPG/GameCore/GameEntity.h"
#include "unitysdk/RPG/GameCore/GameWorld.h"
#include "unitysdk/RPG/GameCore/NPCComponent.h"
#include "unitysdk/RPG/GameCore/StatisticsComponent.h"
#include "unitysdk/RPG/GameCore/TurnBasedGameMode.h"
#include "unitysdk/System/String.h"
#include "unitysdk/System/Text/Encoding.h"
#include "unitysdk/UnityEngine/Application.h"
#include "unitysdk/UnityEngine/Camera.h"
#include "unitysdk/UnityEngine/GameObject.h"
#include "unitysdk/UnityEngine/Input.h"
#include "unitysdk/UnityEngine/Time.h"
#include "unitysdk/UnityEngine/Transform.h"

System::String* CreateSystemString(LPCSTR String, int Length);
System::String* CreateSystemString(LPCWSTR String, int Length);

#define SYSTEM_STRING(String) CreateSystemString(String, sizeof(String) - sizeof(*String))

template <typename T>
T* GetComponent(RPG::GameCore::GameEntity* GameEntity, DWORD TypeDefinitionIndex)
{
	Il2CppClass* ComponentClass = Il2CppClass::FromTypeDefinitionIndex(TypeDefinitionIndex);

	RPG::GameCore::GameEntity_GameComponentList* ComponentList = GameEntity->_ComponentList;

	if (ComponentList)
	{
		for (int i = 0; i < ComponentList->_size; ++i)
		{
			RPG::GameCore::GameComponentBase* Component = ComponentList->_items->vector[i];

			if (Component && Component->klass == ComponentClass)
			{
				return (T*)Component;
			}
		}
	}

	return NULL;
}
