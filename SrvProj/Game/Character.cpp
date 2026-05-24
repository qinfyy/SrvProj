#include "CharacterMgr.h"

CharacterCompBin* CharacterStor::GetMutableCharacterCompBin() {
	return this->GetPlayer()->mPlayerSaveData.mutable_charcomp();
}

CharacterInfo* CharacterStor::AddCharacter(int charId) {
	CharacterInfo* charInfo = this->GetMutableCharacterCompBin()->add_charinfolist();
	charInfo->set_charid(charId);
	return charInfo;
}

CharacterInfo* CharacterStor::GetCharacterById(int id) {
	if (id <= 0) {
		return nullptr;
	}

	CharacterCompBin* charCompBin = this->GetMutableCharacterCompBin();
	for (int i = 0; i < charCompBin->charinfolist_size(); i++) {
		CharacterInfo* charInfo = charCompBin->mutable_charinfolist(i);
		if (charInfo->charid() == id) {
			return charInfo;
		}
	}
	return nullptr;
}

bool CharacterStor::HasCharacter(int id) {
	//CharacterCompBin* charCompBin = this->GetMutableCharacterCompBin();
	//for (int i = 0; i < charCompBin->charinfolist_size(); i++) {
	//	CharacterInfo* charInfo = charCompBin->mutable_charinfolist(i);
	//	if (charInfo->charid() == id) {
	//		return true;
	//	}
	//}
	//return false;
	return GetCharacterById(id) != nullptr;
}

CharacterInfo* CharacterStor::AddCharacter(void* data); //CharacterDef
{
	return nullptr;
}