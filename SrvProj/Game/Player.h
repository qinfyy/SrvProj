#pragma once
#include "../proto/proto_cpp/player_data.pb.h"
#include "../proto/ServerOnly_cpp/playerData.pb.h"

using namespace proto;
using namespace ServerOnly;

class Player
{
public:
	Player() = default;
	~Player() = default;

	void Init();

	PlayerInfo ToProto() const;

	PlayerCompBin mPlayerBin;


};