#pragma once
#include "ManagerBase.h"
#include "../Proto/ServerProto_Cpp/PlayerData.pb.h"

using namespace ServerProto;

class CharacterStor : public ManagerBase
{
public:
	CharacterCompBin* GetMutableCharacterCompBin();

	CharacterInfo* AddCharacter(int charId);
	CharacterInfo* GetCharacterById(int id);
	bool HasCharacter(int id);
	CharacterInfo* AddCharacter(void* data); //CharacterDef



};