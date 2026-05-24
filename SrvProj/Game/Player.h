#pragma once
#include "../proto/proto_cpp/player_data.pb.h"
#include "../proto/ServerProto_cpp/playerData.pb.h"

using namespace proto;
using namespace ServerProto;

class Player
{
public:
	Player() = default;
	~Player() = default;

	void Init();

	PlayerInfo ToProto();

	PlayerSaveData mPlayerSaveData; // 玩家数据存档，包含玩家基本数据和其他模块数据

	//PlayerBasicCompBin GetPlayerData() {
	//	return mPlayerCompBin.playerdata();
	//}

	PlayerBasicCompBin* GetMutablePlayerData() {
		return mPlayerSaveData.mutable_playerdata();
	}

	int GetUid() const {
		return mUid;
	}
private:
	int mUid;
};
